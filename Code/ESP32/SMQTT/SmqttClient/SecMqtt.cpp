#include "SecMqtt.h"


SecMqtt::SecMqtt() : PubSubClient() {
    this->_base_topic = "MK";
    this->_ca_cert = NULL;
    this->_cred = NULL;
    this->_iot_pk_key = NULL;
    this->_iot_pk_key_size = 0;
    this->_iot_pr_key = NULL;
    this->_iot_pr_key_size = 0;
    this->_ks_pk_key = NULL;
    this->_ks_pk_key_size = 0;
    this->_sk_counter = 0;
    this->_secmqtt_state = SECMQTT_KS_DISCONNECTED;
}

SecMqtt::SecMqtt(Client& client) : PubSubClient(client) {
    this->_base_topic = "MK";
    this->_ca_cert = NULL;
    this->_cred = NULL;
    this->_iot_pk_key = NULL;
    this->_iot_pk_key_size = 0;
    this->_iot_pr_key = NULL;
    this->_iot_pr_key_size = 0;
    this->_ks_pk_key = NULL;
    this->_ks_pk_key_size = 0;
    this->_sk_counter = 0;
    this->_secmqtt_state = SECMQTT_KS_DISCONNECTED;
}

void SecMqtt::SecConnect() {

    /** 
     *  1. generate a random topic under base topic (MK)
     *  2. subscribe to acknowledgement topic 
     *  3. publish encrypted symmetric key to Key Store
     *  4. check acknowledgement from Key Store
     *  5. OK? publish encrypted session key to Key Store
     *  6. session key expired? negotiate a new session key
     */

    #ifdef DBG_MSG
    Serial.println("*************************************");
    Serial.println("*   Phase 1: negotiate master key   *");
    Serial.println("*************************************");
    Serial.println("Phase 1: Publishing Epk(K1) and nonce");
    #endif

    /* create key store connection topic */
    String random_topic = "clientID-";
    random_topic += String(random(0xffff), HEX);
    strcpy(this->_conn_topic, String((this->_base_topic + "/" + random_topic + "/" + "value")).c_str());
    strcpy(this->_ack_topic, String((String(this->_base_topic) + "/" + random_topic + "/" + "ack")).c_str());
    strcpy(this->_sk_topic, String((String(this->_base_topic) + "/" + random_topic + "/" + "sk")).c_str());
    strcpy(this->_sk_ack_topic, String((String(this->_base_topic) + "/" + random_topic + "/" + "skack")).c_str());

    #ifdef DBG_MSG
    Serial.print("conn topic: ");
    Serial.println(this->_conn_topic);
    Serial.print("ack topic: ");
    Serial.println(this->_ack_topic);
    #endif

    subscribe(this->_ack_topic);
    subscribe(this->_sk_ack_topic);

    using namespace std::placeholders;
    auto callback = std::bind(&SecMqtt::SecCallback, this, _1, _2, _3);
    setCallback(callback);

    /* generate nonce */
    esp_fill_random(this->_nonce, NONCE_SIZE);

    #ifdef DBG_MSG
    Serial.print("nonce generated: ");
    for(int i = 0; i < NONCE_SIZE; ++i) {
        Serial.print(this->_nonce[i], HEX);
        Serial.print(" ");
    }
    Serial.println(" ");
    #endif

    /* generate a symmetric key */
    sym_key_generator(this->_ks_sym_key);
 
    #ifdef DBG_MSG
    Serial.print("master key generated: ");
    for(int i = 0; i < BLOCK_SIZE; ++i) {
        Serial.print(this->_ks_sym_key[i], HEX);
        Serial.print(" ");
    }
    Serial.println(" ");
    #endif

    /* Epk(K1) */
    size_t len;
    size_t m_len;
    size_t c_len = 128;
    unsigned char cipher[c_len] ={0};

    len = rsa_encryption(this->_ks_sym_key, BLOCK_SIZE, cipher);
  
    /* publish Epk(K1) + nonce */
    Serial.printf("cipher length: %d \n", len);
    m_len = len + NONCE_SIZE + 1;
    Serial.printf("m_len: %d\n", m_len);

    unsigned char message[m_len];
    memcpy(message, cipher, len);
    memcpy(message + len, this->_nonce, NONCE_SIZE);
    message[m_len] = '\0';
    
    beginPublish(this->_conn_topic, m_len, false);
    write((byte *)message, m_len);
    endPublish();

    Serial.println("Publishing finished!");
  
    /* wait until received correct nonce from Key Store */
    unsigned long stime = millis();
    unsigned long etime;
    Serial.println("Waiting for Key Store response ...");

    while(this->_secmqtt_state != SECMQTT_CONNECT_GOOD_NONCE) {
        etime = millis();
        if ((etime - stime) > SECMQTT_TIMEOUT) {
            #ifdef DBG_MSG
            cout << "Phase 1 connection timeout!" << endl;
            #endif
            this->_secmqtt_state = SECMQTT_CONNECTION_TIMEOUT;
            return ;
        }

        if(this->_secmqtt_state == SECMQTT_CONNECT_BAD_NONCE) {
            Serial.println(this->_secmqtt_state);
            return ;
        }

        loop();
    }
 
    #ifdef DBG_MSG
    Serial.println("End Phase 1 successfully!");
    #endif

    this->_secmqtt_state = SECMQTT_KS_CONNECTED;
    return ;
}

void SecMqtt::SecPublish(const char* topic, const unsigned char* msg, size_t msg_len) {
 
    /**
     * Phase 3: Publish message encrypted using session key (Esk(msg))
     */
    #ifdef DBG_MSG
    Serial.println("**************************************************");
    Serial.println("*   Phase 3: Publish message using session key   *");
    Serial.println("**************************************************");
    #endif

    /* check if Key Store connection successful */
    if(this->_secmqtt_state != SECMQTT_KS_CONNECTED) {
        return ;
    }

    /* check if session key needs to be updated */
    if(this->_sk_counter >= MAX_SK_USAGE) {
        SecSessionKeyUpdate();
    }

    #ifdef DBG_MSG
    Serial.println("Phase 3: Publishing encrypted message ...");
    #endif

    unsigned char Eskmsg[msg_len];
    unsigned char iv[BLOCK_SIZE];
    esp_fill_random(iv, BLOCK_SIZE);

    aes_encryption(msg, msg_len, this->_session_key, iv, Eskmsg);
    #ifdef DBG_MSG
    Serial.print("Encrypted user message: ");
    PrintHEX(Eskmsg, msg_len);
    #endif    

    beginPublish(topic,  msg_len, true);
    write((byte*)Eskmsg, msg_len);
    endPublish();
    this->_sk_counter += 1;

    #ifdef DBG_MSG
    Serial.printf("End Phase 3! #%d \n", this->_sk_counter);
    #endif
}

/* call plattform-specific  TRNG driver */
int SecMqtt::myrand(void *rng_state, unsigned char *output, size_t len)
{
    size_t olen;
    return mbedtls_hardware_poll(rng_state, output, len, &olen);
}

/*
 * RSA SIGNING
 * input: message to be signed 
 * P_SIZE: the message size
 * sign: the output signature (size = 128)
 */
void SecMqtt::rsa_sign(const unsigned char * input, size_t P_SIZE, unsigned char * sign) {
  mbedtls_pk_context pr; 
  mbedtls_pk_init(&pr);
  int error_code_pr;
  error_code_pr =  mbedtls_pk_parse_key(&pr, this->_iot_pr_key, this->_iot_pr_key_size, NULL, NULL);
  if(error_code_pr != 0) {
    cout << "READING PRIVATE KEY NOT SUCCESSFULL, error code: " << error_code_pr << endl;
    mbedtls_pk_free(&pr); 
    return ;
  } else {
  /* compute the SHA-256 hash for the input*/
    cout << "READING PRIVATE KEY SUCCESSFULL" << endl;
    unsigned char hash[32]; 
    int hash_ret; 
    hash_ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), input, P_SIZE, hash); 
    if( hash_ret != 0 ) {
      cout << "Calculating HASH not SUCCESSFULL, error code: " << hash_ret << endl;
      mbedtls_pk_free(&pr);
      return ; 
    } else {
      cout << "Calculating HASH SUCCESSFULL" << endl;
      int sign_ret;
      size_t olen = 0;
      sign_ret = mbedtls_pk_sign(&pr, MBEDTLS_MD_SHA256, hash, 0, sign, &olen, myrand, NULL);
      if (sign_ret != 0) {
        cout << "Sign not SUCCESSFULL" << endl;
        mbedtls_pk_free(&pr);
        return;
      } else {
        cout << "Sign SUCCESSFULL" << endl;
        mbedtls_pk_free(&pr); 
        return;
      }
    }
  }
}

/* RSA Encryption */
size_t SecMqtt::rsa_encryption(unsigned char* plainText, size_t P_SIZE, unsigned char * cipher) {
  int error_code; 
  mbedtls_pk_context pk; 
  mbedtls_pk_init(&pk);
  error_code =  mbedtls_pk_parse_public_key(&pk, _ks_pk_key, _ks_pk_key_size);
  
  if(error_code != 0) {
    cout << "READING KEY STORE PUBLIC KEY NOT SUCCESSFULL" << endl; 
    mbedtls_pk_free(&pk);
    return  -1;  
  } else {
    cout << "READING KEY STORE PUBLIC KEY SUCCESSFULL" << endl;
    size_t oolen;
    int result = mbedtls_pk_encrypt( &pk, plainText, P_SIZE, cipher, &oolen, C_SIZE, myrand, NULL );
    if (result == 0) {
      cout << "Encryption SUCCESSFULL" << endl;
      mbedtls_pk_free(&pk);
      return oolen;
    } else {
      cout << "Encryption NOT SUCCESSFULL" << endl;
      mbedtls_pk_free(&pk);
      return -1;
    }
  }
}

void SecMqtt::sym_key_generator(unsigned char* symkey) {
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_entropy_context entropy;
  mbedtls_entropy_init( &entropy );
  mbedtls_ctr_drbg_init( &ctr_drbg );
  char *pers = "aes generate key";
  int ret;

  if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, myrand, &entropy,
    (unsigned char *) pers, strlen( pers ) ) ) != 0 ) {
    printf( " failed\n ! mbedtls_ctr_drbg_init returned -0x%04x\n", -ret );
    return;
  }

  if( ( ret = mbedtls_ctr_drbg_random( &ctr_drbg, symkey, BLOCK_SIZE ) ) != 0 ) {
    printf( " failed\n ! mbedtls_ctr_drbg_random returned -0x%04x\n", -ret );
    return;
  }
}

/* aes encryption cbc */
int SecMqtt::aes_encryption(const unsigned char* input, size_t input_len, const unsigned char* key, unsigned char* iv, unsigned char* output) {

    int rc = 0;

    mbedtls_aes_context aes;
    mbedtls_aes_init( &aes );
    mbedtls_aes_setkey_enc( &aes, (const unsigned char*) key, BLOCK_SIZE * 8);
    rc = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, input_len, iv, input, output);
    mbedtls_aes_free( &aes );
    
    return rc;
}

/* aes decrytion cbc */
int SecMqtt::aes_decryption(const unsigned char* input, size_t input_len, const unsigned char* key, unsigned char* iv, unsigned char* output) {

    int rc = 0;

    mbedtls_aes_context aes;
    mbedtls_aes_init( &aes );
    mbedtls_aes_setkey_enc( &aes, (const unsigned char*) key, BLOCK_SIZE * 8);
    rc = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, input_len, iv, input, output);
    mbedtls_aes_free( &aes );

    return rc;
}

int SecMqtt::get_state() {
  return this->_secmqtt_state;
}

void SecMqtt::SecCallback(char* topic, uint8_t* payload, unsigned int payload_length) {

    int rc = 0;

    /* 
     * Phase 1: check encrypted nonce from Key Store
     * payload = encrypted nonce + iv
     */
    if(!strcmp(topic, this->_ack_topic)) {
        cout << "Message received from topic: " << topic << endl;
        
        unsigned char nonce_enc[NONCE_SIZE];
        unsigned char nonce_dec[NONCE_SIZE];
        unsigned char iv[BLOCK_SIZE];
        
        memset(nonce_enc, 0, NONCE_SIZE);
        memset(nonce_dec, 0, NONCE_SIZE);
        memset(iv, 0, BLOCK_SIZE);
        memcpy(nonce_enc, payload, NONCE_SIZE);
        memcpy(iv, payload + NONCE_SIZE, BLOCK_SIZE);
        
        #ifdef DBG_MSG
        Serial.print("Received encrypted nonce: ");
        PrintHEX(nonce_enc, NONCE_SIZE);
        Serial.print("Received iv: ");
        PrintHEX(iv, BLOCK_SIZE);
        Serial.print("Master key used to decrypt: ");
        PrintHEX(this->_ks_sym_key, BLOCK_SIZE);
        #endif        

        rc = aes_decryption(nonce_enc, NONCE_SIZE, this->_ks_sym_key, iv, nonce_dec);
        
        #ifdef DBG_MSG
        Serial.print("decrypted nonce: ");
        PrintHEX(nonce_dec, NONCE_SIZE);
        #endif

        if (rc != 0) {
            cout << "Failed to decrypt the nonce! \n";
        } else {
            if(!memcmp(this->_nonce, nonce_dec, NONCE_SIZE)) {
                this->_secmqtt_state = SECMQTT_CONNECT_GOOD_NONCE;
            } else {
                this->_secmqtt_state = SECMQTT_CONNECT_BAD_NONCE;
            }
        }
    }
}

void SecMqtt::SecSessionKeyUpdate() {
  
    /* negotiate session key 
     * 1. generate session key and encrypt it using Key Store symmetric key
     * 2. sign the credential
     * 3. send encrypted session key and signed credential to Key Store
     */
    #ifdef DBG_MSG
    Serial.println("**************************************");
    Serial.println("*   Phase 2: negotiate session key   *");
    Serial.println("**************************************");
    Serial.println("Phase 2: Publishing Ek(Sk) to Key Store");
    #endif

    int rc = 0;
    this->_sk_counter = 0;
    unsigned char EkSk[BLOCK_SIZE];
    unsigned char iv[BLOCK_SIZE];
    unsigned char iv_tmp[BLOCK_SIZE];
    size_t m_len;

    esp_fill_random(iv, BLOCK_SIZE);
    sym_key_generator(this->_session_key);

    #ifdef DBG_MSG
    Serial.print("Session Key: ");
    PrintHEX(this->_session_key, BLOCK_SIZE);
    #endif
    
    /* Notice: mbedtls cbc mode will change the iv value! */
    memcpy(iv_tmp, iv, BLOCK_SIZE);
    rc = aes_encryption(this->_session_key, BLOCK_SIZE, this->_ks_sym_key, iv_tmp, EkSk);
 
    if (rc != 0) {
        Serial.println("Failed to encrypt the session key!");
        return ;
    } else {
        #ifdef DBG_MSG
        Serial.print("Encrypted Session Key: ");
        PrintHEX(EkSk, BLOCK_SIZE);
        #endif
    }
     
    size_t cred_len; /* credential length*/
    size_t sig_len = 128;  /*signature length*/
    unsigned char signature[sig_len]; 
   
    /*signing the credential*/ 
    cred_len = strlen((char *)this->_cred);
    rsa_sign(this->_cred, cred_len, signature);
   
    /*
    -------------------------------------------------------
    | Encrypted session key | iv | Signature | credential |
    -------------------------------------------------------
    */
    m_len = 2 * BLOCK_SIZE + sig_len + cred_len + 1; 
    unsigned char message[m_len];

    memset(message, 0, m_len);
    memcpy(message, EkSk, BLOCK_SIZE);
    memcpy(message +  BLOCK_SIZE, iv, BLOCK_SIZE);
    memcpy(message + 2 * BLOCK_SIZE, signature, sig_len);
    memcpy(message + 2 * BLOCK_SIZE + sig_len, this->_cred, cred_len);
    message[m_len - 1] = '\0';

    beginPublish(this->_sk_topic, m_len, false);
    write((byte *)message, m_len); 
    endPublish();

    #ifdef DBG_MSG
    Serial.print("Encrypted Session Key: ");
    PrintHEX(message, BLOCK_SIZE);
    Serial.print("iv: ");
    PrintHEX(message + BLOCK_SIZE, BLOCK_SIZE);
    Serial.print("signature: ");
    PrintHEX(message + 2 * BLOCK_SIZE, sig_len);
    Serial.print("credential: ");
    PrintHEX(message + 2 * BLOCK_SIZE + sig_len, cred_len);
    Serial.println("End phase 2");
    #endif
}
    
void SecMqtt::secmqtt_set_cred(const unsigned char* cred) {
  this->_cred = cred;
}

void SecMqtt::secmqtt_set_ca_cert(const unsigned char* ca_cert) {
  this->_ca_cert = ca_cert;
}

void SecMqtt::secmqtt_set_iot_pk_key(const unsigned char* key, unsigned int key_size) {
  this->_iot_pk_key = key;
  this->_iot_pk_key_size = key_size;
}

void SecMqtt::secmqtt_set_iot_pr_key(const unsigned char* key, unsigned int key_size) {
  this->_iot_pr_key = key;
  this->_iot_pr_key_size = key_size;
}

void SecMqtt::secmqtt_set_ks_pk_key(const unsigned char* key, unsigned int key_size) {
  this->_ks_pk_key = key;
  this->_ks_pk_key_size = key_size;
}

void SecMqtt::PrintHEX(unsigned char* arr, int arr_size) {
    for (int i = 0; i < arr_size; ++i) {
        Serial.print(arr[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

/**
 * The implementation of Key Store on a Linux machine
 * 
 * @download: apt install libmosquitto-dev to download the mqtt client c library 
 *            apt install libssl-dev to download openssl c library
 *
 * @compile: gcc KeyStore.c -o ks -lmosquitto -lssl -lcrypto
 *
 * @functions:
 *    mosquitto_connect() 
 *    mosquitto_disconnect()
 *    mosquitto_subscribe()
 *    mosquitto_unsubscribe()
 *    mosquitto_publish()
 *    mosquitto_loop()
 */

#include "KeyStore.h"

static bool run = true;
bool FLAG_ONCE = true;
bool FLAG_ACK_TOPIC = false;

const char* mqttServer =  "192.168.178.38";
const int mqttPort = 1883;
const char* base_topic = "MK/";
const char* mqttConnTopic = "MK/#";
const char* KeyStoreID = "KeyStoreID-1";

char ack_topic[TOPIC_SIZE];
char value_topic[TOPIC_SIZE];
unsigned char session_key[BLOCK_SIZE];
unsigned char ks_symm_key[BLOCK_SIZE]; // Received Symmetric key from client

/* Key Store public key (1024 bits) */
unsigned char ks_pk_key[] = \
"-----BEGIN PUBLIC KEY-----\n" \
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCzvn02mL6PeS0HV4LNogcu545e\n" \
"+rQKo0MLpRv3EbkVL3p1HIb6JdYfEzyTeMfwSDS+P1/9o7SQ796mYacLtIqEvQ5K\n" \
"ppkqv1CRYQYds2kh/tWmqVEulCOPMu1TmWxpaxGt3mEAwM+FX0FUjnpFSC+P83aO\n" \
"o+74Z+8G/ivvfbXRcQIDAQAB\n" \
"-----END PUBLIC KEY-----\n";

/* Key Store private key */
unsigned char ks_pr_key[] = \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIICXAIBAAKBgQCzvn02mL6PeS0HV4LNogcu545e+rQKo0MLpRv3EbkVL3p1HIb6\n" \
"JdYfEzyTeMfwSDS+P1/9o7SQ796mYacLtIqEvQ5Kppkqv1CRYQYds2kh/tWmqVEu\n" \
"lCOPMu1TmWxpaxGt3mEAwM+FX0FUjnpFSC+P83aOo+74Z+8G/ivvfbXRcQIDAQAB\n" \
"AoGBAJoHdkOwFzTZ0ZWGD4hp/h/avK11x42KJK04+0L5hBK8kgMDxENe4yPUF0gE\n" \
"68lkBxa1Nmwjulutwq3v5vdn6n6vC4GqJcrVCjGl8DguRbr5FevPVw+SAci8ugrr\n" \
"wlpq7tLTDRrswBoQDHak7AW2Tt2UYecDL1e32bALDmgSwYslAkEA5X/oEUvzpOGD\n" \
"fyE71sFjNun8fevYAnHdyalap/4upmUybpJsDJjLm/SNV3zwgoLYPaD35lX+AZuV\n" \
"lia7Sbpr9wJBAMh/ymjjRn0FZx/pIU1sjLs8OErTeg4r00lMiy8iptlvkgYYXwHw\n" \
"CA0pHFb46Bv+NjrgPDrq/e5427LPrfNYw9cCQDpmawKDxlNNw62FInoKqhmeP083\n" \
"xbcRjLO1cclQCET1eXjjr7Ug6Kcd+x2pQHalnytHm+20MjfshIs5/ezMEI8CQCQ5\n" \
"upeC4Nx72msQkxzLDQ5TThRu6zGeR3mJ4+frSdwlZWvGgXv3I21FTUKFjGJqZt/y\n" \
"Xc+DX8fzfDZtgY9jLVcCQCpEYRvfvKTP5dS/JZ5Q2xPELKVcmmyrjLg460FXr9O1\n" \
"Gm1IBHgdNTO8KOr7S5QVmoPi4KIIQRYVWlLhCiXYzgk=\n" \
"-----END RSA PRIVATE KEY-----\n";

/* IoT client public key (1024 bits) */
unsigned char iot_pk_key[]= \
"-----BEGIN PUBLIC KEY-----\n" \
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQC007GfgttefDhYI5Mcv7IrPlWp\n" \
"62tCsqBdcfP/UqneljzLl0uk+1sSMCvR7oCtL+kZcCDbkbv4CjFCKkuhwebZ7YNM\n" \
"TRJ6ws37emQfS8IM7XRlYaIQmmOacDvFFjrWBd0pYKXnP8ZnBSPVmKHu8as+mz0i\n" \
"7BnKdUGhRn9atFzZDQIDAQAB\n" \
"-----END PUBLIC KEY-----\n";

unsigned char *iv = (unsigned char*)"012345678912345";

int main(int argc, char* argv[]) {

    int rc = 0;
    
    /** 
     *Start Mosquitto Client 
     */
    printf("Starting Key Store v1.0 \n");
    printf("Waiting connection from IoT node ...\n");

    struct mosquitto* mosq;

    mosquitto_lib_init();
    mosq = mosquitto_new(KeyStoreID, true, 0);
    
    if (mosq) {
        mosquitto_message_callback_set(mosq, KsCallback);
        
        rc = mosquitto_connect(mosq, mqttServer, mqttPort, 60);
        
        if (rc) {
            printf("Error: %s \n", mosquitto_strerror(rc));
            mosquitto_destroy(mosq);
            mosquitto_lib_cleanup();
            return rc;
        }

        rc = mosquitto_subscribe(mosq, NULL, mqttConnTopic, 0);

        while (run) {
            if (rc) {
                printf("Failed to connect to the broker! Trying reconnect ... \n");
                mosquitto_reconnect(mosq);
                sleep(10);
            }

            rc = mosquitto_loop(mosq, 0, 1);
        }
        
        mosquitto_destroy(mosq);
    }

    mosquitto_lib_cleanup();

    return rc;
}

void KsCallback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg) {

    printf("Receive message under topic: %s \n", msg->topic);
    printf("Received message length: %d \n", msg->payloadlen);    

    int rc = 0;

    const char delimiter[2] = "/";
    char* token1 = strtok(msg->topic, delimiter);
    char* token2 = strtok(NULL, delimiter);
    char* token3 = strtok(NULL, delimiter);
    
    if (!strcmp(token3, "value")) {
        printf("*************************************\n");
        printf("*   Phase 1: negotiate master key   *\n");
        printf("*************************************\n");
         
        memset(ack_topic, 0, TOPIC_SIZE);
        memcpy(ack_topic, base_topic, strlen(base_topic));
        memcpy(ack_topic + strlen(base_topic), token2, strlen(token2));
        strcat(ack_topic, "/ack");

        memset(value_topic, 0, TOPIC_SIZE);
        memcpy(value_topic, base_topic, strlen(base_topic));
        memcpy(value_topic + strlen(base_topic), token2, strlen(token2));
        strcat(value_topic, "/value");

        #ifdef DBG
        printf("Seperated topic: %s, length: %ld \n", token2, strlen(token2));
        printf("Generated acknowledgement topic: %s \n", ack_topic);
        #endif

        /* decrypt received message to get the symmetric Key K1 */
        unsigned char cipher[MAX_TEXT_SIZE];
        unsigned char nonce[NONCE_SIZE];
        unsigned char enc_nonce[32];
        int len;
        //size_t cipher_len = msg->payloadlen - NONCE_SIZE - 1; // need to be checked if include the '\0'

        size_t cipher_len = msg->payloadlen - NONCE_SIZE; // need to be checked if include the '\0'
        memset(ks_symm_key, 0, BLOCK_SIZE);
        memset(cipher, 0, MAX_TEXT_SIZE);
        memset(nonce, 0, NONCE_SIZE);
        memcpy(cipher, msg->payload, cipher_len);
        memcpy(nonce, msg->payload + cipher_len, NONCE_SIZE);

        printf("encrypted master key length: %ld \n", cipher_len);
        printf("Received nonce: ");
        PrintHEX(nonce, NONCE_SIZE);

        rc = rsa_decryption(cipher, cipher_len, ks_pr_key, ks_symm_key);
        if (rc == -1) {
            printf("Failed to decrypt the rsa encrypted symmetric key! \n");
        } else {
            #ifdef DBG
            printf("decrypted master key length: %d \n", rc);
            printf("decrypted master key: ");
            PrintHEX(ks_symm_key, rc);
            printf("<<<<<<< Start NONCE ENC >>>>>>> \n");
            printf("iv used to encrypt nonce: ");
            PrintHEX(iv, 16);
            #endif
                
            len = aes_encryption(nonce, NONCE_SIZE, ks_symm_key, iv, enc_nonce);
            if (len == -1) {
                printf("Failed to encrypt the nonce using KeyStore master key! \n");
            } else {
                printf("encrypted nonce length: %d \n", len);
            }
            
            #ifdef DBG
            printf("encrypted nonce: ");
            PrintHEX(enc_nonce, len);
            printf("<<<<<<<  END NONCE ENC  >>>>>>> \n");
            #endif
        }

        /* Send encrypted nonce and iv back to client */
        unsigned char ack_message[MAX_TEXT_SIZE];
        
        memset(ack_message, 0, MAX_TEXT_SIZE);
        memcpy(ack_message, enc_nonce, len);
        memcpy(ack_message + len, iv, 16);
        ack_message[len + 16] = '\0';

        /* publish encrypted nonce back to client */
        rc = mosquitto_publish(mosq, NULL, ack_topic, len + 17, ack_message, 0, false);

        if (rc != 0) {
            printf("Error: %s \n", mosquitto_strerror(rc));
        } else {
            printf("Published encrypted nonce! \n");
        }
    }

    if (!strcmp(token3, "sk")) {
        printf("**************************************\n");
        printf("*   Phase 2: negotiate session key   *\n");
        printf("**************************************\n");

        unsigned char EkSk[BLOCK_SIZE];
        unsigned char iv[BLOCK_SIZE];
        unsigned char signature[128];
        unsigned char cred[MAX_TEXT_SIZE];
        int sig_len = 128;
        int cred_len = msg->payloadlen - 2*BLOCK_SIZE - sig_len;

        memcpy(EkSk, msg->payload, BLOCK_SIZE);
        memcpy(iv, msg->payload + BLOCK_SIZE, BLOCK_SIZE);
        memcpy(signature, msg->payload + 2*BLOCK_SIZE, sig_len);
        memcpy(cred, msg->payload + 2*BLOCK_SIZE + sig_len, cred_len);

        // todo: signature need to be check first
        #ifdef DBG
        printf("Received EkSK: ");
        PrintHEX(EkSk, BLOCK_SIZE);
        printf("Received iv: ");
        PrintHEX(iv, BLOCK_SIZE);
        printf("Received signature: ");
        PrintHEX(signature, sig_len);
        printf("Received credential: \n");
        printf("%s \n", cred);
        #endif
        
        rc = rsa_signature_verify(cred, strlen(cred), signature, sig_len, iot_pk_key);
    
        if (rc == -1) {
            printf("Failed to verify the signature! \n");
        } else {
            printf("Signature verified! \n");
            rc = aes_decryption(EkSk, BLOCK_SIZE, ks_symm_key, iv, session_key);
           
            if (rc == -1) {
                printf("Failed to decrypt session key! \n");
            } else {
                #ifdef DBG
                printf("Session key length: %d \n", rc);
                printf("Decrypted Session Key: ");
                PrintHEX(session_key, rc);
                #endif
            }
        }
    }
}

/* prepare RSA structure for RSA enc/decryption */
RSA* createRSA(unsigned char* key, int pubkey) {

    RSA* rsa = NULL;
    BIO* keybio = NULL;
    keybio = BIO_new_mem_buf(key, -1);

    if (keybio == NULL) {
        printf("Failed to create key BIO \n");
    } else if (pubkey) {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
    } else {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    }

    return rsa;
}

int rsa_encryption(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* cipher) {

    int rc = 0;
    RSA* rsa = createRSA(key, 1);
    
    if (rsa == NULL) {
        rc = -1;
    } else {
        rc = RSA_public_encrypt(plaintext_len, plaintext, cipher, rsa, RSA_PKCS1_PADDING);
    }

    return rc;
}

int rsa_decryption(unsigned char* enc_data, int data_len, unsigned char *key, unsigned char* dec_data) {
    
    int rc = 0;
    RSA* rsa = createRSA(key, 0);

    if (rsa == NULL) {
        rc = -1;
    } else {
        rc = RSA_private_decrypt(data_len, enc_data, dec_data, rsa, RSA_PKCS1_PADDING);
    }

    return rc;
}


int aes_encryption(unsigned char* plaintext, int plaintext_len, unsigned char* key,
                        unsigned char* iv, unsigned char* ciphertext) {

    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    /* create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        printf("ERROR 1: %ld \n", ERR_get_error());
        return -1;
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv)) {
        printf("ERROR 2: %ld \n", ERR_get_error());
    }

    EVP_CIPHER_CTX_set_padding(ctx, 0);

    /* encrypt message */
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
        printf("ERROR 3: %ld \n", ERR_get_error());
        return -1;
    }

    ciphertext_len = len;
    
    /* finalise the encryption. Further ciphertext bytes may be written at this stage */
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
        printf("ERROR 4: %ld \n", ERR_get_error());
        return -1;
    }

    ciphertext_len += len;

    /* clean up */
    EVP_CIPHER_CTX_free(ctx);
    
    return ciphertext_len;
}

int aes_decryption(unsigned char* cipher, int cipher_len, unsigned char* key,
                      unsigned char* iv, unsigned char* plaintext) {
    
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    /* create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        printf("ERROR 1: %ld \n", ERR_get_error());
        return -1;
    }

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv)) {
        printf("ERROR 2: %ld \n", ERR_get_error());
        return -1;
    }
    
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    /* decrypt message */
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, cipher, cipher_len)) {
        printf("ERROR 3: %ld \n", ERR_get_error());
        return -1;
    }
    plaintext_len = len;
    
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        printf("ERROR 4: %ld \n", ERR_get_error());
        return -1;
    }    
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

int rsa_signature_verify(const unsigned char* m, unsigned int m_length, 
                    unsigned char* sigbuf, unsigned int siglen, unsigned char* pub_key) {
    
    int rc = 0;
    
    RSA* rsa = createRSA(pub_key, 1);
    EVP_PKEY* pubkey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pubkey, rsa);   
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    
    /* Initialize `key` with a public key */
    if (1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, pubkey)) {
        printf("Error1: %ld \n", ERR_get_error());
        return -1;
    }
    
    if (1 != EVP_DigestVerifyUpdate(mdctx, m, m_length)) {
        printf("Error2: %ld \n", ERR_get_error());
        return -1;
    }

    if (1 != EVP_DigestVerifyFinal(mdctx, sigbuf, siglen)) {
        printf("Error3: %ld\n", ERR_get_error());
        return -1;
    } else {
        rc  = 0;
    }

    EVP_MD_CTX_free(mdctx);
    
    return rc;
}

void PrintHEX(unsigned char* str, int len) {

    for (int i = 0; i < len; ++i) {
        printf("%.2X ", str[i]);
    }

    printf("\n");
}

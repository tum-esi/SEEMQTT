/**
 * gcc hks.c -o ks -lmosquitto -lssl -lcrypto

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

#include "KeyStore_ibe.h"
#include <openssl/sha.h>

static bool run = true;
bool ibe_mode = false;
bool rsa_mode = false;
bool ksn_mode = false;
bool sss_mode = false;
bool FLAG_ONCE = true;
bool FLAG_ACK_TOPIC = false;

#define QOS          0
#define TIMEOUT     10000L


int id ; 

const char* mqttServer =  "192.168.178.38";
const int mqttPort = 1883;
const char* base_topic = "MK/";
const char* mqttConnTopic = "MK/#";
char KeyStoreID[20];
char ks_ibe_id[50];


int key_len = 6 + 2*BLOCK_SIZE + 1;
char ack_topic[TOPIC_SIZE];
char value_topic[TOPIC_SIZE];
unsigned char sss_share_key[40];
unsigned char session_key[BLOCK_SIZE];
unsigned char ks_symm_key[BLOCK_SIZE];
unsigned char ks_pr_key[1024]; 
unsigned char Didc[ELEMENT_LEN];

/*
 * IOT Keys
 */
/*
const unsigned char iot_pk_key[]= \
"-----BEGIN PUBLIC KEY-----\n" \
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDKjz7exzoJeYecrXXm1D2QP09F\n" \
"S8GS6hwVfQDmAaQIRt8sfDzGdEWIA7MRp2sr8wBKqboU1qLKscBu4tm39KJ74+MA\n" \
"xtWX/V/Hr+TXvBM2jP5sezl8sMzYPSnlF57szlJOadPUlwoWhA43sH/7A1vGQmiC\n" \
"YqYjK+Nu5G9SwSye4QIDAQAB\n" \
"-----END PUBLIC KEY-----\n";
*/

unsigned char *iv = (unsigned char*)"012345678912345";
/*
 * Read private key 
 */ 
void readprivk(char * path)
{
     FILE *fp;
     struct stat st; /*declare stat variable*/
     int len  = 1024; 
    if(stat(path,&st)==0){
      len = st.st_size;
     #ifdef CRYP_DBG
     printf(" len = %d\n", len); 
     #endif
  }
    fp = fopen(path, "r");
    if (fp == NULL){
        printf("Could not open file %s",path);
        return ;
    }
    
    fread(ks_pr_key, 1, len, fp);
    fclose(fp);
}

int main(int argc, char* argv[]) {
    
    if (argc < 5) {
        printf ("very few arguemnts \n"); 
        printf(" you need to isert 2 arguemnt only: id of KS and  path to its private key \n"); 
        return 1; 
        
    }

    if (argc > 5) {
        printf("Too many arguments\n"); 
        
        printf(" you need to isert 2 arguemnt only: id of KS and  path to its private key \n"); 
        return 1;
    }
    
    id = atoi(argv[1]);
    char *privk_path =  argv[2];
    char *enc_mode = argv[3];
    char *key_share_mode = argv[4];

    readprivk(privk_path);

    sprintf(KeyStoreID, "%d", id);
    sprintf(ks_ibe_id, "keystore%d@tum.com", id);

    printf("key store ibe id: %s\n", ks_ibe_id);

    if (!strncmp(key_share_mode, "ksn", 3)) {
        ksn_mode = true;
    }

    if (!strncmp(key_share_mode, "sss", 3)) {
        sss_mode = true;
    }
    
    if (!strncmp(enc_mode, "ibe", 3)) {
        ibe_mode = true;

        pairing_init_set_buf(ibe_param_pub.pairing, TYPEA_PARAMS, strlen(TYPEA_PARAMS));
        if (!pairing_is_symmetric(ibe_param_pub.pairing)) pbc_die("pairing must be symmetric");

        element_init_G1(ibe_param_pub.Kpub, ibe_param_pub.pairing);
        element_init_G1(ibe_param_pub.P, ibe_param_pub.pairing);
        element_init_Zr(ibe_param_pri.Km, ibe_param_pub.pairing);

        element_from_bytes(ibe_param_pub.Kpub, kpub);
        element_from_bytes(ibe_param_pub.P, p);
        element_from_bytes(ibe_param_pri.Km, km);

        #ifdef DBG
        element_printf("++Km: %B\n", ibe_param_pri.Km);
        element_printf("++Kpub: %B\n", ibe_param_pub.Kpub);
        element_printf("++P:  %B\n", ibe_param_pub.P);
        #endif

        #ifndef SELFTEST
        ibe_extract(Didc);
        printf("Didc: \n");
        PrintHEX(Didc, ELEMENT_LEN);
        #endif

        #ifdef SELFTEST
        unsigned char testmsg[32] = {0};
	    int fd = open("/dev/urandom",O_RDONLY);
	    read(fd, testmsg, 32); 
	    close(fd);
        printf("original message: \n");
        PrintHEX(testmsg, 32);
        unsigned char testcipher[32];
        unsigned char testplain[32];
        unsigned char testuc[ELEMENT_LEN];
        ibe_extract(Didc);

        printf("Didc: \n");
        PrintHEX(Didc, 128);

        ibe_encryption(testmsg, 32, testcipher, testuc);
        ibe_decryption(testcipher, 32, testuc, Didc, testplain);
        printf("msg in plaintext: \n");
        PrintHEX(testplain, 32);

        #endif

    }

    if (!strncmp(enc_mode, "rsa", 3)) {
        rsa_mode = true;
    }

    #ifdef CRYP_DBG  
    printf(" private key :\n"); 
    printf("%s", ks_pr_key);
    #endif

    int rc = 0;
    
    /** 
     *Start Mosquitto Client 
     */
    printf("Starting Key Store %d \n", id);
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
    char* token4 = strtok(NULL, delimiter);
    
    int hashlen = 32; 
    
    if (!strcmp(token3, "value") && !strcmp(token4, KeyStoreID)) {
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
            
        unsigned char cipher[MAX_TEXT_SIZE];
        unsigned char nonce[NONCE_SIZE];
        unsigned char iotpk[PKLEN]; 
        int len;

        /* decrypt received message to get the  Master symmetric Key mk */
        if (ibe_mode) {
            int length = 2*BLOCK_SIZE;
            unsigned char Uc[ELEMENT_LEN];
            unsigned char plainmsg[length];
            
            memcpy(cipher, msg->payload, 2*BLOCK_SIZE);
            memcpy(Uc, msg->payload + 2*BLOCK_SIZE, ELEMENT_LEN);
            memcpy(iotpk, msg->payload + 2*BLOCK_SIZE + ELEMENT_LEN, PKLEN);

            printf("keync_enc: \n");
            PrintHEX(cipher, 32);
            printf("Uc: \n");
            PrintHEX(Uc, ELEMENT_LEN);
            printf("iot pk key: \n");
            PrintHEX(iotpk, PKLEN);

            ibe_decryption(cipher, length, Uc, Didc, plainmsg);

            memcpy(ks_symm_key, plainmsg, BLOCK_SIZE);
            memcpy(nonce, plainmsg + BLOCK_SIZE, BLOCK_SIZE);
        }

        if (rsa_mode) {
            int length = BLOCK_SIZE+NONCE_SIZE;
            unsigned char plainmg[length]; 
            
            memset(ks_symm_key, 0, BLOCK_SIZE);
            memset(cipher, 0, MAX_TEXT_SIZE);
            memset(nonce, 0, NONCE_SIZE);
            memset(iotpk, 0, PKLEN);
            
            #ifdef CRYP_DBG
            printf("Received msg: ");
            PrintHEX(msg->payload, msg->payloadlen);
            #endif
            
            size_t cipher_len = msg->payloadlen - PKLEN;
            memcpy(cipher, msg->payload, cipher_len);
            
            memcpy(iotpk,  msg->payload+cipher_len, PKLEN); 
           
            #ifdef CRYP_DBG
            printf("Received IOT PK: ");
            PrintHEX(iotpk, PKLEN);
            #endif

            rc = rsa_decryption(cipher, cipher_len, ks_pr_key, plainmg);
            if (rc == -1) {
                printf("Failed to decrypt the rsa encrypted symmetric key! \n");
                return ;
            } else {
                memcpy(ks_symm_key, plainmg, BLOCK_SIZE); 
                memcpy (nonce,plainmg+BLOCK_SIZE,NONCE_SIZE);
            }
        }

        #ifdef CRYP_DBG
        printf("Received MKEY: ");
        PrintHEX(ks_symm_key, BLOCK_SIZE);
        
        printf("Received Nonce: ");
        PrintHEX(nonce, NONCE_SIZE);
        #endif

        #ifdef DBG
        printf("decrypted symmetric key length: %d \n", rc);
        printf("decrypted symmetric key: ");
        PrintHEX(ks_symm_key, BLOCK_SIZE);
        printf("<<<<<<< Start ACK ENC >>>>>>> \n");
        printf("iv used to encrypt nonce: ");
        PrintHEX(iv, 16);  
        #endif
        
        
        unsigned char hpk[hashlen]; 
        
        openssl_hash(iotpk, PKLEN, hpk); 
        /*
         ******************************
         *    E_MK(H(pk)XOR N )       *
         ******************************
         */
        int akmsglen = hashlen; 
        unsigned char  plainack [akmsglen]; 
        unsigned char encak[akmsglen];
        
        //H(pk) xor Nonce 
         #ifdef CRYP_DBG
        printf("HPK:"); 
        PrintHEX(hpk, hashlen); 
         
        printf("nonce :"); 
        PrintHEX(nonce,NONCE_SIZE);  
        #endif
        
        for (int i= 0 ; i< hashlen; i++)
            plainack[i] = hpk[i] ^ nonce [i %16]; 
            
        #ifdef CRYP_DBG
        printf("XOR :"); 
        PrintHEX(plainack,hashlen);  
        #endif
               
        len = aes_encryption(plainack, akmsglen, ks_symm_key, iv, encak);
        if (len == -1) {
            printf("Failed to encrypt the nonce using KeyStore symmetric key! \n");
        } else {
            printf("encrypted nonce length: %d \n", len);
        }
        
        #ifdef CRYP_DBG
        printf("encrypted ACK: ");
        PrintHEX(encak, len);
        printf("<<<<<<<  END ACK ENC  >>>>>>> \n");
        #endif
        
        /* Send encrypted nonce and iv back to client */
        unsigned char ack_message[MAX_TEXT_SIZE];
    
        memset(ack_message, 0, MAX_TEXT_SIZE);
        memcpy(ack_message, encak, len);
        memcpy(ack_message + len, iv, 16);
        ack_message[len + 16] = '\0';
        
         #ifdef CRYP_DBG
        printf("ack_message: \n");
        PrintHEX(ack_message, len + 16);
        printf("<<<<<<<  END ACK ENC  >>>>>>> \n");
        #endif
        /* publish encrypted nonce back to client */
        char ackt[70]; 
        sprintf(ackt, "%s/%d",ack_topic,id); 
        rc = mosquitto_publish(mosq, NULL, ackt, len + 17, ack_message, 0, false);

        if (rc != 0) {
            printf("Error: %s \n", mosquitto_strerror(rc));
        } else {
            printf("Published encrypted nonce! \n");
        }
    }

    if (!strcmp(token3, "sk")&& !strcmp(token4, KeyStoreID)) {
        printf("**************************************\n");
        printf("*   Phase 2: receive  secret share   *\n");
        printf("**************************************\n");


        if (ksn_mode) {
            unsigned char EkSk[BLOCK_SIZE + 2 *  hashlen];
            unsigned char iv[BLOCK_SIZE];
            unsigned char dmsg [BLOCK_SIZE + 2 *  hashlen];
            unsigned char rhpk[hashlen]; 
            unsigned char rht [hashlen]; 
            
            memcpy(EkSk, msg->payload, BLOCK_SIZE + 2 *  hashlen);
            memcpy(iv, msg->payload + BLOCK_SIZE+ 2 *  hashlen, BLOCK_SIZE);

            #ifdef CRYP_DBG
            printf("Received EkSK: ");
            PrintHEX(EkSk, BLOCK_SIZE + 2 *  hashlen);
            printf("Received iv: ");
            PrintHEX(iv, BLOCK_SIZE);
            #endif
            
            rc = aes_decryption(EkSk, BLOCK_SIZE+ 2 * hashlen, ks_symm_key, iv, dmsg);
               
            if (rc == -1) {
                printf("Failed to decrypt session key! \n");
            } else {
                #ifdef CRYP_DBG
                printf("Decrypted message: ");
                PrintHEX(dmsg, rc);
                #endif
                
                memcpy(session_key,dmsg, BLOCK_SIZE) ; 
                memcpy(rhpk, dmsg + BLOCK_SIZE, hashlen); 
                memcpy(rht, dmsg + BLOCK_SIZE+ hashlen, hashlen); 
                
                 #ifdef CRYP_DBG
               
                printf("Decrypted share Key: ");
                PrintHEX(session_key, BLOCK_SIZE);
                
                printf("Decrypted hpk: ");
                PrintHEX(rhpk, hashlen);
                
                printf("Decrypted ht: ");
                PrintHEX(rht, hashlen);
                #endif
            }
        }

        if (sss_mode) {
            unsigned char EkSk[3*BLOCK_SIZE + 2*hashlen];
            unsigned char iv[BLOCK_SIZE];
            unsigned char dmsg[key_len + 2*hashlen];
            unsigned char rhpk[hashlen];
            unsigned char rht[hashlen];

            memcpy(EkSk, msg->payload, 3*BLOCK_SIZE + 2*hashlen);
            memcpy(iv, msg->payload + 3*BLOCK_SIZE + 2*hashlen, BLOCK_SIZE);

            #ifdef CRYP_DBG
            printf("Received EkSK: ");
            PrintHEX(EkSk, 3*BLOCK_SIZE + 2*hashlen);
            printf("Received iv: ");
            PrintHEX(iv, BLOCK_SIZE);
            #endif

            rc = aes_decryption(EkSk, 3*BLOCK_SIZE+ 2 * hashlen, ks_symm_key, iv, dmsg);
            
            if (rc == -1) {
                printf("Failed to decrypt session key! \n");
            } else {
                #ifdef CRYP_DBG
                printf("decrypted msg length: %d\n", rc);
                printf("Decrypted message: \n");
                PrintHEX(dmsg, rc);
                #endif
                
                memcpy(sss_share_key, dmsg, key_len);
                memcpy(rhpk, dmsg + key_len, hashlen);
                memcpy(rht, dmsg + key_len + hashlen, hashlen);

                #ifdef CRYP_DBG
                sss_share_key[key_len] = '\0';
                printf("Decrypted share Key: ");
                printf("%s", sss_share_key);

                printf("Decrypted hpk: ");
                PrintHEX(rhpk, hashlen);
                
                printf("Decrypted ht: ");
                PrintHEX(rht, hashlen);
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

void ibe_extract(unsigned char *Didc) {

    printf("ibe extract start ... \n");
    unsigned char hash[HASH_LEN] = {0};
    element_t Qid, Did;

    element_init_G1(Qid, ibe_param_pub.pairing);
    element_init_G1(Did, ibe_param_pub.pairing);
  
    printf("extract id: %s\n", ks_ibe_id);
    openssl_hash((unsigned char *)ks_ibe_id, strlen(ks_ibe_id), hash);

    element_from_hash(Qid, hash, HASH_LEN);
    element_mul_zn(Did, Qid, ibe_param_pri.Km);
    element_to_bytes(Didc, Did);

    element_clear(Qid);
    element_clear(Did);

    printf("ibe extract end!\n");
}


void ibe_encryption(unsigned char *plaintext, \
                    size_t plaintext_len , unsigned char *cipher, unsigned char *Uc) {

    unsigned char *gs = NULL, hash[HASH_LEN] = {0};
    element_t r, U, gid, Qid;

    /* Initialize parameters */
    element_init_G1(Qid, ibe_param_pub.pairing);
    element_init_G1(U, ibe_param_pub.pairing);
    element_init_GT(gid, ibe_param_pub.pairing);
    element_init_Zr(r, ibe_param_pub.pairing);

    element_random(r);

    printf("ibe encryption start hashing\n");

    openssl_hash((unsigned char *)ks_ibe_id, strlen((char *)ks_ibe_id), hash);

    printf("ibe enc finish hashing\n");

    element_from_hash(Qid, hash, HASH_LEN);
    
    element_mul_zn(U, ibe_param_pub.P, r);
    element_pairing(gid, Qid, ibe_param_pub.Kpub);
    element_pow_zn(gid, gid, r);
    gs = (unsigned char *)malloc(element_length_in_bytes(gid));
    element_to_bytes(gs, gid);

    int gidlen = element_length_in_bytes(gid);
    printf("gid length: %d\n", gidlen);
   
    openssl_hash(gs, ELEMENT_LEN, hash) ;
    
    int j = 0;
    for (int i = 0; i < plaintext_len; i++) {
        if (j >= HASH_LEN) {
            j = 0;
        }
        cipher[i] = plaintext[i] ^ hash[j];
        j++;
    }

    element_to_bytes(Uc, U);

    free(gs);
    element_clear(r);
    element_clear(U);
    element_clear(gid);
    element_clear(Qid);
}


int ibe_decryption(unsigned char *ciphertext, size_t ciphertext_len, \
                         unsigned char *Uc, unsigned char *Didc, unsigned char *plaintext) {

    printf("ibe decryption start ...\n");    

    unsigned char *gs = NULL, hash[HASH_LEN] = {0};
    element_t U, Did, xt;

    /* Initialize the G1 element */
    element_init_G1(U, ibe_param_pub.pairing);
    element_init_G1(Did, ibe_param_pub.pairing);

    /* Initialize the GT element */
    element_init_GT(xt, ibe_param_pub.pairing);

    element_from_bytes(U, Uc);
    element_from_bytes(Did, Didc);

    element_pairing(xt, Did, U);
    gs = (unsigned char *)malloc(element_length_in_bytes(xt));
    element_to_bytes(gs, xt);

    openssl_hash(gs, ELEMENT_LEN, hash);

    int j = 0;
    for (int i = 0; i < ciphertext_len; i++) {
        if (j >= HASH_LEN) {
            j = 0;
        }
        plaintext[i] = ciphertext[i] ^ hash[j];
        j++;
    }

    free(gs);
    element_clear(U);
    element_clear(Did);
    element_clear(xt);

    printf("ibe decryption end!\n");
}

/*
 * Hash 
 */ 


 void openssl_hash(unsigned char  * m, int len, unsigned char *hash)
 {

    SHA256_CTX ctx;

    SHA256_Init(&ctx);
    SHA256_Update(&ctx, m, len);
    SHA256_Final(hash, &ctx);

         /*
     * unsigned char *SHA256(const unsigned char *d, size_t n,
      unsigned char *md);
      
      
     SHA256(m, len, hash);
     */
 }
     
void PrintHEX(unsigned char* str, int len) {

    for (int i = 0; i < len; ++i) {
        printf("%.2X ", str[i]);
    }

    printf("\n");
}

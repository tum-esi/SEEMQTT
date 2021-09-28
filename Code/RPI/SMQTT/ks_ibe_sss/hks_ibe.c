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




static bool run = true;
// KEy Sippliting (KSN), Shamir's Secret Sharing (SSS)
enum SHARING_MODE {KSN, SSS}; 
// Public Key Encryption (PKE) (using rsa), Identoty Based Encryption (IBE)
enum ENC_MODE {PKE, IBE}; 

enum SHARING_MODE sharing_mode; 
enum ENC_MODE enc_mode ; 

//bool ibe_mode = false;
//bool rsa_mode = false;
//bool ksn_mode = false;
//bool sss_mode = false;
bool FLAG_ONCE = true;
bool FLAG_ACK_TOPIC = false;

#define QOS          0
#define TIMEOUT     10000L
#define SSS_PATH  "/home/pi/Desktop/SSSS/"


LIST  lst ; 

int id ; 

const char* mqttServer =  "192.168.0.110";
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
    

    
    if (argc >5 || argc <4 ) {
        printf ("Wrong number of argument\n"); 
        printf(" KS Id  enc_mode(ibe, rsa) [private key path (only for rsa mode)  sharing mode (sss,ksn) \n"); 
        return 1; 
        
    }
    
    id = atoi(argv[1]);
    
    sprintf(KeyStoreID, "%d", id);
    sprintf(ks_ibe_id, "keystore%d@tum.com", id);

    printf("key store ibe id: %s\n", ks_ibe_id);
    
    if (!strncmp(argv[2], "ibe", 3)){
       
        enc_mode = IBE ; 
        if (!strncmp(argv[2], "ksn", 3))
            sharing_mode = KSN;
        else 
            sharing_mode = SSS;
    }
     else 
     {
        enc_mode = PKE ;
        if (!strncmp(argv[4], "ksn", 3))
            sharing_mode = KSN;
        else 
            sharing_mode = SSS;
    }
    
     // initie db 
     Initdb();
     
    
     /* 
      *Encryption mode is IBE 
      */
     if (enc_mode) {
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

    if (!enc_mode) {
        char *privk_path =  argv[3];
        readprivk(privk_path);
        #ifdef CRYP_DBG  
        printf(" private key :\n"); 
        printf("%s", ks_pr_key);
        #endif

    }

  
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
        printf("*   Phase 1: negotiate master symetric key   *\n");
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
        //IBE mode 
        if (enc_mode){
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

        // PKE mode 
        if ( !enc_mode) {
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
        
        /* update the lst*/ 
         int rid = AddsymK(hpk,ks_symm_key); 
        #ifdef CRYP_DBG
        if (rid != -1) {
            printf(" The Master key was saved into db");
            //Printlst(db[rid]); }
            Printdb(); 
        }
        else 
            printf(" Error, master key was not saved\n"); 
        #endif
        
        //memcpy(lst.hiot, hpk, HASH_LEN) ; 
        //memcpy (lst.msymk, ks_symm_key, BLOCK_SIZE); 
        //Printlst(lst); 
    }

    if (!strcmp(token3, "sk")&& !strcmp(token4, KeyStoreID)) {
        printf("**************************************\n");
        printf("*   Phase 2:  Topic Key Distribution *\n");
        printf("**************************************\n");
        
        
        // Key splitting mode 
        if (!sharing_mode) {
            #ifdef CRYP_DBG
            printf("**************************************\n");
            printf("*   Sharing Mode: Key splitting mode *\n");
            printf("**************************************\n");
            #endif
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

        // Shamir's Secret Sharing 
        if (sharing_mode) {
             #ifdef CRYP_DBG
            printf("**************************************\n");
            printf("*Sharing Mode: Shamir Secret Sharing *\n");
            printf("**************************************\n");
            #endif
            PrintHEX(msg->payload,  3*BLOCK_SIZE + 2*hashlen +BLOCK_SIZE + BLOCK_SIZE );
            int mlen = 3*BLOCK_SIZE + hashlen;
            unsigned char EkSk[3*BLOCK_SIZE + hashlen];
            unsigned char iv[BLOCK_SIZE];
            //unsigned char dmsg[key_len + 2*hashlen];
            unsigned char dmsg[3*BLOCK_SIZE+ hashlen];
            unsigned char rhpk[hashlen];
            unsigned char rht[hashlen];
            unsigned char rtag[hashlen];
            const char  * aad = "";
            
            
             unsigned char msymK[BLOCK_SIZE] ; 

            memcpy(EkSk, msg->payload,  3*BLOCK_SIZE + hashlen);
            memcpy(iv,   msg->payload + 3*BLOCK_SIZE + hashlen, BLOCK_SIZE);
            memcpy(rtag, msg->payload + 3*BLOCK_SIZE + hashlen +BLOCK_SIZE, BLOCK_SIZE);
            memcpy(rhpk, msg->payload + 3*BLOCK_SIZE + hashlen +BLOCK_SIZE + BLOCK_SIZE , hashlen);

            #ifdef CRYP_DBG
            printf("Received EkSK: ");
            PrintHEX(EkSk, 3*BLOCK_SIZE + hashlen);
            printf("Received iv: ");
            PrintHEX(iv, BLOCK_SIZE);
               printf("Received tag: ");
            PrintHEX(rtag, BLOCK_SIZE);
            printf("Key to decrypt message: "); 
            PrintHEX(ks_symm_key,BLOCK_SIZE );
            
            printf("recived iotpk: "); 
            PrintHEX(rhpk,HASH_LEN );
            #endif

            
            /* Get the master symmetric key based on the H(iotpk) */ 
             int rowid = GetSymK(rhpk, msymK) ; 
            #ifdef CRYP_DBG
             if (memcmp(msymK, ks_symm_key,BLOCK_SIZE)==0)
                printf ("Key extracted succefully\n" ); 
            else 
                printf (" Key extracted NOT succefully \n " ); 
            
            #endif
            
            int res =   aes_gcm_decrypt(EkSk, mlen,(unsigned char *)aad, strlen(aad),rtag, msymK, iv, BLOCK_SIZE, dmsg); 
            
            if (rc == -1) {
                printf("Failed to decrypt the shared secret! \n");
            } else {
                #ifdef CRYP_DBG
                printf("decrypted msg length: %d\n", rc);
                printf("Decrypted message: \n");
                PrintHEX(dmsg, rc);
                #endif
                
                memcpy(sss_share_key, dmsg, key_len);
                memcpy(rht, dmsg + key_len , hashlen);
                
                

                #ifdef CRYP_DBG
                sss_share_key[key_len] = '\0';
                printf("Decrypted share Key: ");
                printf("%s", sss_share_key);

                printf("Decrypted hpk: ");
                PrintHEX(rhpk, hashlen);
                
                printf("Decrypted ht: ");
                PrintHEX(rht, hashlen);
                #endif
                
                /* update the db to add the share*/ 
                //memcpy(lst.shar, dmsg, key_len) ; 
                //memcpy(lst.htpk, rht, HASH_LEN); 
                // Printlst(lst); 
                // SaveShar(id,lst); 
                int rown = Addshare(rhpk,rht, dmsg); 
                //Printlst(db[rown]); 
                Printdb();
                SaveShar(id,db[rown]); 
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



	
void handleErrors()
	{
		printf("Error"); 		
}

/*
 * source: https://wiki.openssl.org/index.php/EVP_Authenticated_Encryption_and_Decryption
 */	
int aes_gcm_encrypt(unsigned char *plaintext, int plaintext_len,
                unsigned char *aad, int aad_len,
                unsigned char key[],
                unsigned char *iv, int iv_len,
                unsigned char *ciphertext,
                unsigned char *tag)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;


    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /* Initialise the encryption operation. */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL))
        handleErrors();

    /*
     * Set IV length if default 12 bytes (96 bits) is not appropriate
     */
    if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL))
        handleErrors();

    /* Initialise key and IV */
    if(1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv))
        handleErrors();

    /*
     * Provide any AAD data. This can be called zero or more times as
     * required
     */
    if(1 != EVP_EncryptUpdate(ctx, NULL, &len, aad, aad_len))
        handleErrors();

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    /*
     * Finalise the encryption. Normally ciphertext bytes may be written at
     * this stage, but this does not occur in GCM mode
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    /* Get the tag */
    if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag))
        handleErrors();

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}


int aes_gcm_decrypt(unsigned char *ciphertext, int ciphertext_len,
                unsigned char *aad, int aad_len,
                unsigned char *tag,
                unsigned char *key,
                unsigned char *iv, int iv_len,
                unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    int ret;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /* Initialise the decryption operation. */
    if(!EVP_DecryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL))
        handleErrors();

    /* Set IV length. Not necessary if this is 12 bytes (96 bits) */
    if(!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL))
        handleErrors();

    /* Initialise key and IV */
    if(!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv))
        handleErrors();

    /*
     * Provide any AAD data. This can be called zero or more times as
     * required
     */
    if(!EVP_DecryptUpdate(ctx, NULL, &len, aad, aad_len))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    if(!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    /* Set expected tag value. Works in OpenSSL 1.0.1d and later */
    if(!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag))
        handleErrors();

    /*
     * Finalise the decryption. A positive return value indicates success,
     * anything else is a failure - the plaintext is not trustworthy.
     */
    ret = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    if(ret > 0) {
        /* Success */
        plaintext_len += len;
        return plaintext_len;
    } else {
        /* Verify failed */
        return -1;
    }
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

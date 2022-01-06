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


bool FLAG_ONCE = true;
bool FLAG_ACK_TOPIC = false;

#define QOS          0
#define TIMEOUT     10000L



LIST  lst ; 
int id ; 

const char* mqttServer =  "192.168.0.102";
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
     #ifdef DDBG
     printf(" len = %d\n", len); 
     #endif
    }
    fp = fopen(path, "r");
    if (fp == NULL){
        #ifdef DBG
        printf("Could not open file %s",path);
        #endif
        return ;
    }
    
    fread(ks_pr_key, 1, len, fp);
    fclose(fp);
}

int main(int argc, char* argv[]) {
    

    if (argc >5 || argc <4 ) {
        printf ("Wrong number of argument\n");
        printf("ksibe ID  Enc_Mode [prK] Sharing_Mode\n");
        printf("Id:\t\t Keystore id (1,2,..,etc.)\n");
        printf("Enc_Mode:\t ibe  or rsa \n");
        printf("prk:\t\t The path of the rsa private key. Only need if Enc_Mode is rsa\n"); 
        printf("Sharing_Mode:\t sss for Shamir Secret Sharing  or ksn for secret key splitting\n"); 
        return 1; 
        
    }
    
    id = atoi(argv[1]);
    
    sprintf(KeyStoreID, "%d", id);
    sprintf(ks_ibe_id, "keystore%d@tum.com", id);

    
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
    
     /* initie db */
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

        #ifdef DDBG
        element_printf("++Km: %B\n", ibe_param_pri.Km);
        element_printf("++Kpub: %B\n", ibe_param_pub.Kpub);
        element_printf("++P:  %B\n", ibe_param_pub.P);
        #endif

        #ifdef SELF_TEST
        ibe_extract(Didc);
        
        #ifdef DDBG
        printf("Didc: \n");
        BIO_dump_fp(stdout,Didc, ELEMENT_LEN);
        #endif
         
        unsigned char testmsg[32] = {0};
	    int fd = open("/dev/urandom",O_RDONLY);
	    read(fd, testmsg, 32); 
	    close(fd);
        #ifdef DDBG
        printf("original message: \n");
         BIO_dump_fp(stdout,testmsg, 32);
         #endif
        unsigned char testcipher[32];
        unsigned char testplain[32];
        unsigned char testuc[ELEMENT_LEN];
        ibe_extract(Didc);
        #ifdef DDBG
        printf("Didc: \n");
         BIO_dump_fp(stdout,Didc, 128);
         #endif
        ibe_encryption(testmsg, 32, testcipher, testuc);
        ibe_decryption(testcipher, 32, testuc, Didc, testplain);
        #ifdef DDBG
        printf("msg in plaintext: \n");
        BIO_dump_fp(stdout,testplain, 32);
        #endif
        #endif

    }

    if (!enc_mode) {
        char *privk_path =  argv[3];
        readprivk(privk_path);
        #ifdef DDBG 
        printf("The KseyStroe private key:\n"); 
        printf("%s", ks_pr_key);
        #endif

    }

  
    int rc = 0;
    
    /** 
     *Start Mosquitto Client 
     */

    
    #ifdef DBG
    printf("The KeyStroe ID is %s\n", ks_ibe_id);
    printf("Starting KeyStore-%d...", id);
    #endif

    struct mosquitto* mosq;

    mosquitto_lib_init();
    mosq = mosquitto_new(KeyStoreID, true, 0);
    
    if (mosq) {
        mosquitto_message_callback_set(mosq, KsCallback);
        
        rc = mosquitto_connect(mosq, mqttServer, mqttPort, 60);
        
        if (rc) {
            #ifdef DBG
            printf("Error: %s \n", mosquitto_strerror(rc));
            #endif
            mosquitto_destroy(mosq);
            mosquitto_lib_cleanup();
            return rc;
        }
        #ifdef DBG
            printf("Connected!\n");
        #endif
        rc = mosquitto_subscribe(mosq, NULL, mqttConnTopic, 0);
        #ifdef DBG
            printf("KeyStore-%d subscribed to topic (%s)\n", id, mqttConnTopic);
        #endif

        while (run) {
            if (rc) {
                #ifdef DBG
                printf("Failed to connect to the broker! Trying reconnect ... \n");
                #endif
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

    #ifdef DBG
    printf("Received a message under topic (%s)\n", msg->topic);
    printf("The the length of the received message is (%d)\n", msg->payloadlen);    
    #endif
    int rc = 0;

    const char delimiter[2] = "/";
    char* token1 = strtok(msg->topic, delimiter);
    char* token2 = strtok(NULL, delimiter);
    char* token3 = strtok(NULL, delimiter);
    char* token4 = strtok(NULL, delimiter);
    
    int hashlen = 32; 
    
    if (!strcmp(token3, "value") && !strcmp(token4, KeyStoreID)) {
        
        #ifdef DBG
        printf("***************************************\n");
        printf("*Phase I:negotiate master symetric key*\n");
        printf("***************************************\n");
        #endif
        
        memset(ack_topic, 0, TOPIC_SIZE);
        memcpy(ack_topic, base_topic, strlen(base_topic));
        memcpy(ack_topic + strlen(base_topic), token2, strlen(token2));
        strcat(ack_topic, "/ack");

        memset(value_topic, 0, TOPIC_SIZE);
        memcpy(value_topic, base_topic, strlen(base_topic));
        memcpy(value_topic + strlen(base_topic), token2, strlen(token2));
        strcat(value_topic, "/value");

        #ifdef DDBG
        printf("Seperated topic: %s, length: %ld \n", token2, strlen(token2));
        printf("Generated acknowledgement topic: %s \n", ack_topic);
        #endif
            
        unsigned char cipher[MAX_TEXT_SIZE];
        unsigned char nonce[NONCE_SIZE];
        unsigned char r_hiotpk[HASH_LEN]; 
        unsigned char iotpk[PKLEN]; 
        int len;

        /* decrypt received message to get the  Master symmetric Key mk 
         * IBE mode */
        if (enc_mode){
            int length = 2*BLOCK_SIZE+HASH_LEN;
            unsigned char Uc[ELEMENT_LEN];
            unsigned char plainmsg[length];
            
            memcpy(cipher, msg->payload, 2*BLOCK_SIZE+HASH_LEN);
            memcpy(Uc, msg->payload + 2*BLOCK_SIZE+HASH_LEN, ELEMENT_LEN);
            memcpy(iotpk, msg->payload + 2*BLOCK_SIZE+HASH_LEN + ELEMENT_LEN, PKLEN);
            
            #ifdef DDBG
            printf("keync_enc: \n");
            BIO_dump_fp(stdout,cipher,32); 
            printf("Uc: \n");
            BIO_dump_fp(stdout,Uc,ELEMENT_LEN); 
            printf("Publisher Public Key:\n");
            BIO_dump_fp(stdout,iotpk,PKLEN);
            #endif 
            
            #ifdef T_DBG
            struct timespec start , end;
            clock_gettime(CLOCK_MONOTONIC,&start);
            #endif

            ibe_decryption(cipher, length, Uc, Didc, plainmsg);
            
            #ifdef T_DBG
            clock_gettime(CLOCK_MONOTONIC,&end);
            double t_ibe_dec= (double)(end.tv_sec-start.tv_sec) *1000000000 + (double)(end.tv_nsec-start.tv_nsec);
            printf ("Time to decrypt the message = %f  (ms) \n", t_ibe_dec / 1000000);
            #endif 

            memcpy(ks_symm_key, plainmsg, BLOCK_SIZE);
            memcpy(nonce, plainmsg + BLOCK_SIZE, BLOCK_SIZE);
            memcpy(r_hiotpk, plainmsg +2 *BLOCK_SIZE, HASH_LEN); 
        }

        // PKE mode 
        if ( !enc_mode) {
            int length = BLOCK_SIZE+NONCE_SIZE;
            unsigned char plainmg[length]; 
            
            memset(ks_symm_key, 0, BLOCK_SIZE);
            memset(cipher, 0, MAX_TEXT_SIZE);
            memset(nonce, 0, NONCE_SIZE);
            memset(iotpk, 0, PKLEN);
            
            #ifdef DDBG
            printf("The received msg: ");
            BIO_dump_fp(stdout,msg->payload,msg->payloadlen); 
            #endif
            
            size_t cipher_len = msg->payloadlen - PKLEN;
            memcpy(cipher, msg->payload, cipher_len);
            memcpy(iotpk,  msg->payload+cipher_len, PKLEN); 
           
            #ifdef DDBG
            printf("Received Publisher public Key:\n");
            BIO_dump_fp(stdout,iotpk, PKLEN);
            #endif

            rc = rsa_decryption(cipher, cipher_len, ks_pr_key, plainmg);
            if (rc == -1) {
                #ifdef DBG
                printf("Failed to decrypt the rsa encrypted symmetric key! \n");
                #endif
                return ;
            } 
            else {
                memcpy(ks_symm_key, plainmg, BLOCK_SIZE); 
                memcpy (nonce,plainmg+BLOCK_SIZE,NONCE_SIZE);
            }
        }

        #ifdef DDBG
        printf("Received MKEY:\n");
        BIO_dump_fp(stdout,ks_symm_key, BLOCK_SIZE);
        printf("Received Nonce:\n");
        BIO_dump_fp(stdout,nonce, NONCE_SIZE);
        printf("decrypted symmetric key length: %d \n", rc);
        printf("decrypted symmetric key: ");
        BIO_dump_fp(stdout,ks_symm_key, BLOCK_SIZE);
        #endif


        #ifdef DBG 
            printf("The master key was received succefully!\n");
            printf("Prepear the acknowledgment....\n");
        #endif
        #ifdef DDBG 
        printf("the IV used to encrypt nonce: ");
        BIO_dump_fp(stdout,iv, 16);  
        #endif
        
        unsigned char hpk[hashlen]; 
        
        openssl_hash(iotpk, PKLEN, hpk); 
        
        /* check if the message was maipulated*/ 
         if(memcmp(hpk,r_hiotpk, HASH_LEN)!=0)
         {
            #ifdef DBG
                printf("the H(pkK_Pub) that was recived is different from the one expected\n"); 
                printf("Received hiotpk:\n");
                BIO_dump_fp(stdout,r_hiotpk, HASH_LEN);
                printf("calculated hiotpk:\n");
                BIO_dump_fp(stdout,hpk, HASH_LEN);
            #endif
            return; 
         }
         
          
      
        
        /*
         ******************************
         *    E_MK(H(pk)XOR N )       *
         ******************************
         */
        int akmsglen = hashlen; 
        unsigned char  plainack [akmsglen]; 
        unsigned char encak[akmsglen];
        
        /*H(pk) xor Nonce */
        #ifdef DDBG
        printf("the H(pkK_Pub):\n"); 
        BIO_dump_fp(stdout,hpk, hashlen); 
        printf("The nonce:\n"); 
        BIO_dump_fp(stdout,nonce,NONCE_SIZE);  
        #endif
        
        for (int i= 0 ; i< hashlen; i++)
            plainack[i] = hpk[i] ^ nonce [i %16]; 
            
        #ifdef DDBG
        printf("XOR :"); 
         BIO_dump_fp(stdout,plainack,hashlen);  
        #endif
               
        len = aes_encryption(plainack, akmsglen, ks_symm_key, iv, encak);
        if (len == -1) {
            #ifdef DDBG
            printf("Failed to encrypt the nonce using KeyStore symmetric key! \n");
            #endif
        } else {
            #ifdef DDBG
            printf("encrypted nonce length: %d\n", len);
            #endif
        }
        
        #ifdef DDBG
        printf("encrypted ACK: ");
        BIO_dump_fp(stdout,encak, len);
        #endif
      
        
        /* Send encrypted nonce and iv back to client */
        unsigned char ack_message[MAX_TEXT_SIZE];
    
        memset(ack_message, 0, MAX_TEXT_SIZE);
        memcpy(ack_message, encak, len);
        memcpy(ack_message + len, iv, 16);
        ack_message[len + 16] = '\0';
        
        #ifdef DDBG
         printf("The Ack_message: \n");
         BIO_dump_fp(stdout,ack_message, len + 16);
        #endif
        /* publish encrypted nonce back to client */
        char ackt[70]; 
        sprintf(ackt, "%s/%d",ack_topic,id); 
        rc = mosquitto_publish(mosq, NULL, ackt, len + 17, ack_message, 0, false);

        if (rc != 0) {
            #ifdef DBG
            printf("Error: %s \n", mosquitto_strerror(rc));
            #endif
        } else {
            #ifdef DBG
            printf("the Acknowledgment was Published! \n");
            #endif
        }


        /* update the lst*/ 
         int rid = AddsymK(hpk,ks_symm_key); 
        #ifdef DBG
        if (rid != -1) {
            printf("The Master key was saved into db\n");
            #ifdef DDBG
            Printdb(); 
            #endif
        }
        else 
            printf(" Error, master key was not saved\n"); 
        #endif
    }

    if (!strcmp(token3, "sk")&& !strcmp(token4, KeyStoreID)) {
        
        #ifdef DBG
        printf("**************************************\n");
        printf("*   Phase 2:  Topic Key Distribution *\n");
        printf("**************************************\n");
        #endif
        
        // Key splitting mode 
        if (!sharing_mode) {
            #ifdef DBG 
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

            #ifdef DDBG
            printf("Received encrypted share:\n");
            BIO_dump_fp(stdout,EkSk, BLOCK_SIZE + 2 *  hashlen);
            printf("Received IV: ");
            BIO_dump_fp(stdout,iv, BLOCK_SIZE);
            #endif
            
            rc = aes_decryption(EkSk, BLOCK_SIZE+ 2 * hashlen, ks_symm_key, iv, dmsg);
               
            if (rc == -1) {
                #ifdef DBG
                printf("Failed to decrypt session key! \n");
                #endif
            } else {
                #ifdef DDBG
                printf("Decrypted message: ");
                 BIO_dump_fp(stdout,dmsg, rc);
                #endif
                
                memcpy(session_key,dmsg, BLOCK_SIZE) ; 
                memcpy(rhpk, dmsg + BLOCK_SIZE, hashlen); 
                memcpy(rht, dmsg + BLOCK_SIZE+ hashlen, hashlen); 
                
                #ifdef DDBG
                printf("Decrypted share Key:\n");
                BIO_dump_fp(stdout,session_key, BLOCK_SIZE);
                
                printf("Decrypted H(pkK_Pub):\n");
                BIO_dump_fp(stdout,rhpk, hashlen);
                
                printf("Decrypted H(tp_i):\n");
                BIO_dump_fp(stdout,rht, hashlen);
                #endif
            }
        }

        // Shamir's Secret Sharing 
        if (sharing_mode) {
             #ifdef DBG
            printf("**************************************\n");
            printf("*Sharing Mode: Shamir Secret Sharing *\n");
            printf("**************************************\n");
            #endif
            #ifdef DDBG
             printf(" The received Message:\n"); 
             BIO_dump_fp(stdout,msg->payload,  3*BLOCK_SIZE + 2*hashlen +BLOCK_SIZE + BLOCK_SIZE );
             #endif
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

            #ifdef DDBG
            printf("Received encrypted share:\n");
            BIO_dump_fp(stdout,EkSk, 3*BLOCK_SIZE + hashlen);
            printf("Received IV:\n");
            BIO_dump_fp(stdout,iv, BLOCK_SIZE);
            printf("Received H(tp_i):\n");
            BIO_dump_fp(stdout,rtag, BLOCK_SIZE);
            printf("Key to decrypt message: "); 
            PrintHEX(ks_symm_key,BLOCK_SIZE );
            printf("Recived H(pkK_Pub):\n"); 
            BIO_dump_fp(stdout,rhpk,HASH_LEN );
            #endif

            /* Get the master symmetric key based on the H(iotpk) */ 
             int rowid = GetSymK(rhpk, msymK) ; 
            #ifdef DDBG
             if (memcmp(msymK, ks_symm_key,BLOCK_SIZE)==0)
                printf ("Key extracted succefully\n" ); 
            else 
                printf (" Key extracted NOT succefully \n " ); 
            #endif
            
            int res =   aes_gcm_decrypt(EkSk, mlen,(unsigned char *)aad, strlen(aad),rtag, msymK, iv, BLOCK_SIZE, dmsg); 
            
            if (rc == -1) {
                #ifdef DBG 
                printf("Failed to decrypt the shared secret! \n");
                #endif
            } else {
                #ifdef DDBG
                printf("decrypted msg length: %d\n", rc);
                printf("Decrypted message: \n");
                BIO_dump_fp(stdout,dmsg, rc);
                #endif
                
                memcpy(sss_share_key, dmsg, key_len);
                memcpy(rht, dmsg + key_len , hashlen);
                
                #ifdef DDBG
                sss_share_key[key_len] = '\0';
                printf("Decrypted share Key:\n");
                printf("%s", sss_share_key);

                printf("Decrypted hpk:\n");
                BIO_dump_fp(stdout,rhpk, hashlen);
                
                printf("Decrypted ht:\n");
                BIO_dump_fp(stdout,rht, hashlen);
                #endif
                
                /* update the db to add the share*/ 
                int rown = Addshare(rhpk,rht, dmsg); 
                #ifdef DDBG
                Printdb();
                #endif
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
        #ifdef DDBG
        printf("Failed to create key BIO \n");
        #endif
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
    #ifdef DBG
    printf("Error"); 
    #endif		
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
        #ifdef DDBG
        printf("ERROR 1: %ld \n", ERR_get_error());
        #endif
        return -1;
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv)) {
        #ifdef DDBG
        printf("ERROR 2: %ld \n", ERR_get_error());
        #endif
    }

    EVP_CIPHER_CTX_set_padding(ctx, 0);

    /* encrypt message */
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
        #ifdef DDBG
        printf("ERROR 3: %ld \n", ERR_get_error());
        #endif
        return -1;
    }

    ciphertext_len = len;
    
    /* finalise the encryption. Further ciphertext bytes may be written at this stage */
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
        #ifdef DDBG
        printf("ERROR 4: %ld \n", ERR_get_error());
        #endif
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

    #ifdef DDBG
    printf("ibe extract start ... \n");
    #endif
    unsigned char hash[HASH_LEN] = {0};
    element_t Qid, Did;

    element_init_G1(Qid, ibe_param_pub.pairing);
    element_init_G1(Did, ibe_param_pub.pairing);
  
    #ifdef DDBG
    printf("extract id: %s\n", ks_ibe_id);
    #endif
    openssl_hash((unsigned char *)ks_ibe_id, strlen(ks_ibe_id), hash);

    element_from_hash(Qid, hash, HASH_LEN);
    element_mul_zn(Did, Qid, ibe_param_pri.Km);
    element_to_bytes(Didc, Did);

    element_clear(Qid);
    element_clear(Did);
    #ifdef DDBG
    printf("ibe extract end!\n");
    #endif
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
    #ifdef DDBG
    printf("ibe encryption start hashing\n");
    #endif

    openssl_hash((unsigned char *)ks_ibe_id, strlen((char *)ks_ibe_id), hash);

    #ifdef DDBG
    printf("ibe enc finish hashing\n");
    #endif

    element_from_hash(Qid, hash, HASH_LEN);
    
    element_mul_zn(U, ibe_param_pub.P, r);
    element_pairing(gid, Qid, ibe_param_pub.Kpub);
    element_pow_zn(gid, gid, r);
    gs = (unsigned char *)malloc(element_length_in_bytes(gid));
    element_to_bytes(gs, gid);

    int gidlen = element_length_in_bytes(gid);
    #ifdef DDBG
    printf("gid length: %d\n", gidlen);
    #endif
   
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

    #ifdef DDBG
    printf("ibe decryption start ...\n");  
    #endif  

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

    #ifdef DDBG 
    printf("ibe decryption end!\n");
    #endif
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

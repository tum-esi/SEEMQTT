/**
 * gcc hks.c -o ks -lmosquitto -lssl -lcrypto

 * The implementation subscriber on a Linux machine
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


# include "sub.h"
static bool run = true;



bool FLAG_ONCE = true;
bool FLAG_ACK_TOPIC = false;

#define QOS          0
#define TIMEOUT     10000L


int id ;




const char* mqttServer =  "192.168.0.110";
const int mqttPort = 1883;
const char* base_topic = "MK/";
const char* mqttConnTopic = "iot_data";
const char* Credtopic = "CR";
char * subname = "sub1";

#define MSG_MAX 5120


unsigned char * session_key  = NULL;



int main(int argc, char* argv[]) {

    int rc = 0;

    /**
     *Start Mosquitto Client
     */
    printf("Startin Subscriber \n", id);


    struct mosquitto* mosq;

    mosquitto_lib_init();
    mosq = mosquitto_new(subname, true, 0);

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
        rc = mosquitto_subscribe(mosq, NULL, Credtopic, 0);
        printf("Subscribe to Topics: %s and %s  :\n", mqttConnTopic, Credtopic );

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
     if (!strcmp(msg->topic, mqttConnTopic))
     {

         int rc = 0;
        //BIO_dump_fp(stdout, msg->payload, msg->payloadlen);
        unsigned char  RTAG [16];
        unsigned char  RIV [16];
        int msglen;
        unsigned char  RCT [MSG_MAX];
        unsigned char  plain [MSG_MAX];

        memcpy(RTAG, msg->payload, 16);
        memcpy(RIV, msg->payload+16, 16);
        memcpy(&msglen, msg->payload+32, sizeof(int));
        memcpy(RCT, msg->payload+32+ sizeof(int),msglen);

        const char *aad = "";
        //BIO_dump_fp(stdout, RCT, msglen);

        //unsigned char * key = ReadShare();

        if(session_key != NULL)
        {
            unsigned char * key = ReadShare();
            printf("Key:\n");
            PrintHEX(key, BLOCK_SIZE);
            int res =   aes_gcm_decrypt(RCT, 16,(unsigned char *)aad, strlen(aad),RTAG, key, RIV, 16, plain);
            printf(" res = %d \n", res);
            if (res >0)
            {
                printf("plain text:\n");
                BIO_dump_fp(stdout, plain, res);
            }
            else
                handleErrors();

        }
    }

    if (!strcmp(msg->topic, Credtopic))
     {
         session_key = ReadShare();
         PrintHEX (session_key, BLOCK_SIZE) ;

    }

}


void handleErrors()
{
    printf("Error");
}

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

unsigned char * ReadShare()
{
    int i = 0 ;
    int lenght  =KSN_NUM *(SSS_SIZE +2);
    int sharlenght = SSS_SIZE+1 ;
    char *  strshares = (char *) malloc (1024);
    memset(strshares, '\0', 1024) ;


    for (; i<SSS_T;i++)
    {
        char name[5];
        sprintf(name, "%d.txt", i+1);
        char path [124] ="/home/pi/Desktop/SSSS/" ;
        strcat(path,name);
        FILE *fptr;
        fptr = fopen(path,"r");
        if (fptr == NULL)
            exit(EXIT_FAILURE);

        char * line ;
        int len = 0 ;
        int read = getline(&line, &len, fptr);
        if (read != -1)
        {
            //printf("Retrieved line of length %zu :\n", read);
            //printf("%s", line);
        }

        char * token = strtok(line, "|");

        int index = 0 ;
        for (; index<3;index++)
            token = strtok(NULL, "|");
        //printf("%s\n", token);

        //memcpy(shares[i],(unsigned char *)token, SSS_SIZE);
        sprintf(strshares + (i * strlen(token)), "%s\n", token);
        //PrintHEX(shares[i], SSS_SIZE);
        // printf  ("%s", strshares);


        free(line);
        fclose(fptr);
    }

    char *  result = extract_secret_from_share_strings(strshares);
    //PrintHEX (result, BLOCK_SIZE) ;
    return result;

}



void PrintHEX(unsigned char* str, int len) {

    for (int i = 0; i < len; ++i) {
        printf("%.2X ", str[i]);
    }

    printf("\n");
}

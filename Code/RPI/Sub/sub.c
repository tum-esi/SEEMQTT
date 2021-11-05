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
    #ifdef DBG
    printf("Starting Subscriber %d...", id);
    #endif

    struct mosquitto* mosq;

    mosquitto_lib_init();
    mosq = mosquitto_new(subname, true, 0);

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
        printf("started \n");
        printf("Subscribe to Topics: (%s) and (%s)...", mqttConnTopic, Credtopic );
        #endif

        rc = mosquitto_subscribe(mosq, NULL, mqttConnTopic, 0);
        rc = mosquitto_subscribe(mosq, NULL, Credtopic, 0);

         #ifdef DBG
        printf("done \n");
        printf("Waiting for messges on subscribed topics...\n", mqttConnTopic, Credtopic );
        #endif

        while (run) {
            if (rc) {
                #ifdef DBG
                printf("Failed to connect to the broker! Trying reconnect...\n");
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



void SaveCred(const char * buf, int len)
{
    FILE * fptr;
    fptr = fopen("CR-Pub-CA1", "w");
    if (fptr ==NULL)
    {
        #ifdef DBG
        printf("Error: can not open file %s \n", CR1);
        #endif
            exit(1);
    }

    for (int i =0 ; i<len ; i++)
        fprintf(fptr, "%c", buf[i]);
    fclose(fptr);
}



void KsCallback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg) {

    #ifdef DBG
    printf("Receive message under topic:(%s)\n", msg->topic);
    printf("The recived message length=%d\n", msg->payloadlen);
    #endif
    #ifdef DDBG
    BIO_dump_fp(stdout, msg->payload, msg->payloadlen);
    #endif

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
        #ifdef DDBG
         BIO_dump_fp(stdout, RCT, msglen);
        #endif

        if(session_key != NULL)
        {
            int res =   aes_gcm_decrypt(RCT, 16,(unsigned char *)aad, strlen(aad),RTAG, session_key, RIV, 16, plain);
            #ifdef DBG
             printf("aes_gcm_decrypt: res = %d \n", res);
            #endif
            if (res >0)
            {
                printf("Plain text:\n");
                BIO_dump_fp(stdout, plain, res);
            }
            else
                handleErrors();

        }
    }

    if (!strcmp(msg->topic, Credtopic))
     {

        int len = msg->payloadlen;
        char *  buf = (char *) malloc(len);
        memcpy(buf, msg->payload, len);
        SaveCred(buf, len);
        session_key = ComShare();
        #ifdef DDBG
            BIO_dump_fp(stdout, session_key, BLOCK_SIZE);
            //PrintHEX (session_key, BLOCK_SIZE) ;
        #endif
    }
}

void handleErrors()
{
    #ifdef DBG
        printf("Error");
    #endif
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
/*
 * aes_gcm_decrypt: Decrypt using AES-GCM-128
 */
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
/*
 * ComShare: get the share from the KS and Combine them
 */
unsigned char * ComShare()
{
    #ifdef DBG
     printf("Getting the shares and combine them...\n");
    #endif
    int ksid = 0;
    char * strshares = (char * )malloc(1024);
    memset(strshares, '\0', 1024);
    int rs = 0 ;
    while(rs<SSS_T && ksid <KSN_NUM)
    {
        int len = 0;
        char * share = GetShare(KSnames[ksid],KSports[ksid], &len);
        if (len>0)
        {
            #ifdef DBG
                printf("       Share[%d] was recived !\n", ksid);
                #ifdef DDBG
                printf("       Share[%d]:<%s>\n",ksid,share);
                #endif
            #endif
            sprintf(strshares+ rs * len, "%s", share);
            rs++;
        }
        ksid++;
    }
    if (rs <SSS_T)
        return NULL;

    char *  result = extract_secret_from_share_strings(strshares) ;
    #ifdef DBG
        printf("Key was combined seuucefully!\n");
        #ifdef DDBG
            BIO_dump_fp(stdout, result, BLOCK_SIZE);
            //PrintHEX (result, BLOCK_SIZE);
        #endif
    #endif
    return result ;
}
/*
 * PrintHex: print in Hex
 */
void PrintHEX(unsigned char* str, int len) {

    for (int i = 0; i < len; ++i) {
        printf("%.2X ", str[i]);
    }

    printf("\n");
}

void PrintChar( const char * buf, int len)
{
    printf("---------------------------------------------------------------------------\n");
    for (int i =0 ; i < len; i++)
        printf("%c",buf[i]);
    printf("\n----------------------------------------------------------------------------\n");
}

/*
 * GetShares:  get the share for the KS
 * srv_hostname: ks IP address or name
 * port: port number
 * len: lenght of the the share
 */
char * GetShare(char * srv_hostname, int port, int * len)
{
    char * topic= "car_loc";
	char * lication = "munich";
	int sock;
	struct sockaddr_in server;
	struct hostent *hp;
	//char *srv_hostname;
	int plen;

	char pbuf[100] ={'\0'};

	//srv_hostname = ksname;
	/* Create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
        #ifdef DBG
	      perror("  opening stream socket\n");
        #endif
	     *len = -1;
         return NULL ;
	}

	/* Connect socket using name specified by command line. */
	server.sin_family = AF_INET;
	hp = gethostbyname(srv_hostname);
	if (hp == 0) {
        #ifdef DBG
	      fprintf(stderr, "%s: unknown host\n", srv_hostname);
        #endif
        *len = -1;
        return NULL;
	}

	memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
	server.sin_port = htons(port);


    #ifdef DBG
     printf("    Connecting  to %s:%d .......",srv_hostname,port);
    #endif
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        #ifdef DBG
	      printf("  Connection was refused whilt trying to connect to: %d", port );
        #endif
          *len = -1;
          return NULL ;
	}

    #ifdef DBG
    printf("Connected\n");
    printf("    Prepearing to send the request...\n");
    #endif


    /*
    * *************************************************************
    *  Totla_len |c|cr_len|cr|.......|k|pk_l|PK|r|R_l|R | s_l | S *
    * *************************************************************
    */

    /* load Credentials */

    int cr1_len ;
    char * cr1= getCredentials(CR1,&cr1_len);

    int cr2_len ;
    char*  cr2= getCredentials(CR2,&cr2_len);

    #ifdef DDBG
     printf("       credential 1 length = %d \n", cr1_len);
     PrintChar(cr1, cr1_len);
     printf("       credential 2 length = %d \n", cr2_len);
     PrintChar(cr2, cr2_len);
     #endif

    int pk_l ;
    char * pk = getpk(&pk_l);

    #ifdef DDBG
     printf("       pk length = %d \n", pk_l);
      PrintChar(pk, pk_l);
     #endif

    char * asrt = "topic=\"car_loc\"\n"\
        "location=\"munich\"\n";

    int asrt_l = strlen(asrt);

    #ifdef DDBG
        PrintChar(asrt, asrt_l);
    #endif

    /* calculate the whole len of the message */
    int cr_l = cr1_len + cr2_len;
    int msg_l = sizeof (int)  + cr_l  +  sizeof(int) *2 + asrt_l+ sizeof (int)+  pk_l +  +sizeof (int);


    char *  msg= (char *)malloc(msg_l);
    memset(msg,0,msg_l);
    memcpy(msg, &msg_l, sizeof(int));  // total length
    memcpy(msg+sizeof(int), &cr1_len, sizeof(int));
    memcpy(msg+2*sizeof(int), cr1, cr1_len);
    memcpy(msg+2*sizeof(int)+ cr1_len, &cr2_len, sizeof (int) );
    memcpy(msg+3*sizeof(int)+ cr1_len, cr2, cr2_len);
    memcpy(msg+3*sizeof(int)+ cr_l, &pk_l, sizeof (int) );
    memcpy(msg+3*sizeof(int)+ cr_l+ sizeof(int), pk, pk_l );
    memcpy(msg+3*sizeof(int)+ cr_l+ sizeof(int) + pk_l , &asrt_l, sizeof (int) );
    memcpy(msg+3*sizeof(int)+ cr_l+ sizeof(int) + pk_l + sizeof(int) , asrt, asrt_l);

    #ifdef DDBG
        printf("        Totla Message size = %d \n" , msg_l);
    #endif

    int  sent =  0 ;
       int remin = msg_l ;
     while (remin >0 )
     {
        int s = write( sock, msg+sent , remin);

        sent += s;
        remin -=s ;
     }

    #ifdef DBG
            printf("       Sent\n");
    #endif
    int re = 0;
    char *  share = malloc (1024);
    int result = read(sock, share, 1024);
    close(sock);
    *len = result;
    return share;
}

/*
 * Read Credentials
 *
 */
char* getCredentials( char * path, int * len)
{
    FILE *fp;
    long cre_Size;
    char *result;
    fp= fopen(path,"r");
    if (fp == NULL)
    {
        #ifdef DBG
        fprintf(stderr, "       Credential file is missing\n");
        #endif
        exit(1);
    }
    fseek(fp,0,SEEK_END);
    cre_Size = ftell(fp);
    #ifdef DBG
         printf("       Credentials Size %d\n",cre_Size);
    #endif

    rewind(fp);
    result = malloc(cre_Size+1);
    fread(result,cre_Size,1,fp);
    fclose(fp);
    result[cre_Size]='\0';
    *len =cre_Size;
    return result;
}
/*
 * Get publick key
 */

char* getpk(int * len)
{
    FILE *fp;
    long pol_Size;
    char *result;
    fp= fopen(SUBPK,"r");
    if (fp == NULL)
    {
        fprintf(stderr, "       Public Key file is missing\n");
        exit(1);
    }
    fseek(fp,0,SEEK_END);
    pol_Size = ftell(fp);
    rewind(fp);
    result = malloc(pol_Size+1);
    fread(result,pol_Size,1,fp);
    fclose(fp);
    //result[pol_Size]='\0';
    *len =pol_Size;
    return result;
}

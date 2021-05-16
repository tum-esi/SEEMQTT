
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "MQTTClient.h"


#include "config.h"





 /* symetric key to encrypt the date*/
// uint8_t key[BLOCK_SIZE] = { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  uint8_t key[BLOCK_SIZE];

 /* ID. Hash of PK  */ 
 int id = 1; 
 

 

/*
 * IOT Keys
 */
const unsigned char iot_pk_key[]= \
"-----BEGIN PUBLIC KEY-----\n" \
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDKjz7exzoJeYecrXXm1D2QP09F\n" \
"S8GS6hwVfQDmAaQIRt8sfDzGdEWIA7MRp2sr8wBKqboU1qLKscBu4tm39KJ74+MA\n" \
"xtWX/V/Hr+TXvBM2jP5sezl8sMzYPSnlF57szlJOadPUlwoWhA43sH/7A1vGQmiC\n" \
"YqYjK+Nu5G9SwSye4QIDAQAB\n" \
"-----END PUBLIC KEY-----\n";




const unsigned char iot_pr_key[]= \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIICXAIBAAKBgQDKjz7exzoJeYecrXXm1D2QP09FS8GS6hwVfQDmAaQIRt8sfDzG\n" \
"dEWIA7MRp2sr8wBKqboU1qLKscBu4tm39KJ74+MAxtWX/V/Hr+TXvBM2jP5sezl8\n" \
"sMzYPSnlF57szlJOadPUlwoWhA43sH/7A1vGQmiCYqYjK+Nu5G9SwSye4QIDAQAB\n" \
"AoGAAveWNC027GybT/A6iecR/8rtnlY9v6kuQCJUoQnlHTnsivpG6aFKY3Oqa9fH\n" \
"+hzVJELYNvXt7IouB3HLPqaWurMpDXKD3ZgGZEjzGX755+u+ExPepE+s1ky3HtXk\n" \
"9OESnysHdmxtc5ZJInOwVo+SbIsAzpUzDQaBXU3xuqhxAAECQQD5MgQy0V29dv5r\n" \
"9FnzkJI0q1E4Vu+b1bpgoQOBmN9+NMbbyyuJqCqcueRmoNE0paQKlCtNEDW4nvmv\n" \
"aaZkRJPBAkEA0Bc5flVD4PrWPbQbGKt4LA8nivWz7kJSx+Oq6liDK/tU+zLwjFli\n" \
"X6uYZv6QOz+z+IlAJjd3s7JgDuE+++5TIQJAdX06c4c4ziJzVAyx+F8aP9DiC1EZ\n" \
"yXxcUkC3+XtNLCDP/ULuNQnfhWMzp5t853iNPhubysqwuWniyOxj8fRPQQJBAIxc\n" \
"fg13GhgxWj24tOZJEwA1IKkc3seBuTDXorv8BE6wczwVuigRSsfZiS0/3K36ZDzq\n" \
"53hfxcBSO3UN/lwU/8ECQGqRJrNkOCOho0mqcqNC+RTFcATBmhZwzGMCdtQ7xKmT\n" \
"oAY+p2EM44ihrC/W/8R8PQAaw8Jt4FXQSlrlWsotuh4=\n" \
"-----END RSA PRIVATE KEY-----\n";

	




/*
* Generate  random text
*/
static uint8_t  *rand_string(uint8_t  *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXZY";
    if (size) {
        for (size_t n = 0; n < size; n++) {
            int key = rand() % 16;
            str[n] = key;
	    //printf( " id = %d  , %u \n",n,   str[n]);
        }
    }
    return str;
}

uint8_t * rand_string_alloc(size_t size)
{
     uint8_t  *s =(uint8_t  *) malloc(size);
     if (s) {
         rand_string(s, size);
     }
     return s;
}

/* 
 * Read publik  key of KSs
 */ 
void loadpubK( int i , char * path)
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
        return 1;
    }
    
    fread(LIST[i].ks_pk, 1, len, fp);
    fclose(fp);
}

/*
 * Setup the Keystores Info
 */
void setup_list(char ** paths)
{
    for (int i = 0 ; i< NKS; i++)
    {
	loadpubK (i , paths[i+1]); 
	uint8_t  * mk = rand_string_alloc(BLOCK_SIZE); 
	memcpy(LIST[i].masterkey, mk, BLOCK_SIZE); 
	uint8_t  * nc = rand_string_alloc(BLOCK_SIZE); 
	memcpy(LIST[i].nonce , nc, BLOCK_SIZE);
    }
}

/*
 * Caluclate Hpc, Ht
 */ 
void setup_h()
{
    openssl_hash(iot_pk_key,PKLEN,calhpk);
    openssl_hash(DATA,strlen(DATA),calht);
    #ifdef DBUG
	printf("calculated h(pk) : \n");
	Print(calhpk, hashlen,0);
	 
	printf("calculated h(t) : \n");
	Print(calht, hashlen,0);
    #endif
}

/*
* transmitting the encrypted message to subscriber
*/
 void Datatransmission(unsigned char * msg, int value, MQTTClient client)
 {
      
   
	
      struct timeval start, end;
      MQTTClient_message pubmsg = MQTTClient_message_initializer;
      MQTTClient_deliveryToken token;
      int rc ;

     unsigned char  cip[Size_Byte];
     uint8_t iv[BLOCK_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
     gettimeofday(&start, NULL);
     openssl_aes_encryption(msg,Size_Byte,key, iv, cip);
      gettimeofday(&end, NULL);
      /* lenght of the encryptted message + one interger to hold that  lenght*/
      int len = Size_Byte + sizeof(Size_Byte);
      unsigned char buf [len ];

      /*
       * ******************************
       *           |                  |
       * msg_lenght|      MDG         |
       *           |                  |
       * ******************************
       */
      memcpy ( buf, &Size_Byte, sizeof(Size_Byte));
      memcpy ( buf + sizeof(Size_Byte), cip, Size_Byte);

      pubmsg.payload = buf;
      pubmsg.payloadlen = len;
      pubmsg.qos = QOS;
      pubmsg.retained = 0;
      MQTTClient_publishMessage(client, DATA, &pubmsg, &token);
     // rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    

      #ifdef TIME_DEBUG
       printf("\t%d\t\t",(end.tv_usec - start.tv_usec));
     #endif

 }

void  Split()
{
     /* Generate NKS - 1 random keys */ 
     for ( int i = 0 ; i< NKS-1 ; i++)
     {
	 uint8_t  * mk = rand_string_alloc(BLOCK_SIZE); 
	 memcpy	(shares[i] , mk, BLOCK_SIZE); 
     }
     
     /* creat the NKSth shares*/
     for ( int j = 0 ; j< BLOCK_SIZE ; j++)
     {
	 shares[NKS-1][j] = key[j]; 
	 for ( int i = 0 ; i< NKS-1; i++)
	    shares[NKS-1][j] =  shares[NKS-1][j] ^ shares[i][j] ; 
     }
}



/*
 *Send the shares genrated based on the message key to Keystores
 * 
 *
       **********************
       *               |    |  
       *  En(sk,hc, ht)| iv |
       *               |    |
       * ********************
*/ 


 void SesKeytransmission(int value, MQTTClient client)
 {
   	
	struct timeval start, end, esign, senck,eenck;
	MQTTClient_message pubmsgkey = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;
	int rc ;
	int lenght  = BLOCK_SIZE + 2 * hashlen + BLOCK_SIZE ; 
	unsigned char buff[lenght];
	
	
	
	gettimeofday(&start, NULL);
	
	uint8_t iv[BLOCK_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t  * k = rand_string_alloc(BLOCK_SIZE);  
	memcpy (key, k , BLOCK_SIZE); 
	Split(); 
	
	gettimeofday(&senck, NULL);
	
	for ( int i = 0 ; i<NKS; i++)
	{
	    #ifdef CRY_DEBUG
		printf("shares %d: ", i);
		Print(shares[i], BLOCK_SIZE, 0);
	    #endif
	    
	    
	    unsigned char cip [BLOCK_SIZE+2*hashlen];
	    unsigned char msg [BLOCK_SIZE+2*hashlen]; 
	    
	    memcpy(msg, shares[i], BLOCK_SIZE); 
	    memcpy(msg+BLOCK_SIZE, calhpk, hashlen); 
	    memcpy(msg+BLOCK_SIZE+hashlen,calht, hashlen); 
	    
	    openssl_aes_encryption(msg, BLOCK_SIZE+2*hashlen,LIST[i].masterkey, iv, cip);
	  
	    

	    #ifdef CRY_DEBUG
		printf("Encryptted share (%d):\n", i);
		Print (cip, BLOCK_SIZE+2*hashlen, 0);
	    #endif
	    
	  
	    memcpy(buff,cip,BLOCK_SIZE + 2 * hashlen );
	    memcpy(buff+BLOCK_SIZE + 2 * hashlen,iv,BLOCK_SIZE);
	    
	    
	    #ifdef CRY_DEBUG
		printf("Transmitted message  for (%d):\n", i);
		Print (buff, lenght, 0);
	    #endif
	    
	    pubmsgkey.payload = buff;
	    pubmsgkey.payloadlen = lenght;
	    pubmsgkey.qos = QOS;
	    pubmsgkey.retained = 0;
	    char kst[50]; 
	    sprintf(kst, "MK/ClientID1/sk/%d", i+1); 
	    MQTTClient_publishMessage(client, kst, &pubmsgkey, &token);

	}
	gettimeofday(&eenck, NULL);
	
	/* Transmit the credintial*/
	
	int siglength = 128;
        int maxlen = 2048; 
	
	
	char * credintial  = (char *) malloc (maxlen); 
	char * condition = "(app_domain==\"KS\")-> { (Process) < 2500) -> _MAX_TRUST";
	 
	sprintf(credintial, "KeyNote-Version: 2\nAuthorizer: \"%s\" \nLicensees: \"%s\" \nConditions:%s\nSignature: ",iot_pk_key,LIST[0].ks_pk,condition);
	 #ifdef DBUG
	 printf("credintial : %s",credintial);
	 #endif
	 
	 unsigned char signature [siglength];

	/*sign the credintial*/
	openssl_rsa_sign(credintial,strlen(credintial),  signature,  siglength, iot_pr_key);
       
	int cr_len = siglength + strlen(credintial); 
	unsigned char cre_buff[cr_len];
	memcpy(cre_buff,signature,siglength);
	memcpy(cre_buff+siglength,credintial,strlen(credintial));
	#ifdef CRY_DEBUG
	    printf("T signed Credential\n");
	    Print (cre_buff, cr_len, 0);
	#endif
	
	pubmsgkey.payload = cre_buff;
	pubmsgkey.payloadlen = cr_len;
	pubmsgkey.qos = QOS;
	pubmsgkey.retained = 0;
	MQTTClient_publishMessage(client, CR, &pubmsgkey, &token);
	
	gettimeofday(&esign, NULL);
	
	gettimeofday(&end, NULL);

      #ifdef TIME_DEBUG
	printf("\t%d\t\t%d\t\t",(esign.tv_usec - eenck.tv_usec), (eenck.tv_usec - senck.tv_usec));
      #endif
 }

 
  /*
   *Exchange the Master key with  key store.
   * MK will be used to derive a session key between the keystore and the IoT node to exchange  data session key
   * nonce is also sent
   *  */
  void MKregister(MQTTClient client)
 {
	struct timeval start, end;
	struct timeval es, ee;
	MQTTClient_message pubmsgkey = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;
	int rc ;
	
	gettimeofday(&start, NULL);

      /*
          ********************************
          *   |               |          |
          * pk| c=Enc(Ks,N),PK| sig(c)   |
          *   |               |          |
          ********************************
       */
	
	for ( int i = 0 ; i< NKS ; i++)
	{
	     gettimeofday(&es, NULL);
	    unsigned char cipher[512] = {0};
	    int len = 0;
	    int lenght  = sizeof(LIST[i].masterkey) + sizeof(LIST[i].nonce);
	    char msg[lenght];
	    
	    #ifdef CRY_DEBUG
		printf("Master Key: ");
		Print(LIST[i].masterkey, 16, 0);
	
		printf("publick Key (%d):",sizeof(iot_pk_key));
		Print(iot_pk_key, sizeof(iot_pk_key), 0);
	    #endif
	    
	    memcpy(msg, LIST[i].masterkey, sizeof(LIST[i].masterkey)); 
	    memcpy(msg+sizeof(LIST[i].masterkey), LIST[i].nonce, sizeof(LIST[i].nonce)); 
	    
	    #ifdef CRY_DEBUG
		printf("Message (%d):\n", lenght);
		Print (msg, lenght, 0);
	    #endif 
	    
	    len = openssl_rsa_encryption(msg, lenght,LIST[i].ks_pk, cipher );
	    
	    gettimeofday(&ee, NULL);
	    
	    #ifdef CRY_DEBUG
		printf(" Ecrypted Message (%d):\n", len);
		Print (cipher, len, 0);
	    #endif
	
	    int totallen = len + sizeof(iot_pk_key);
	    unsigned char enpk[totallen]; 
	    memset(enpk,0,totallen); 
	    
	   
	    memcpy(enpk, cipher, len); 
	    memcpy(enpk+len, iot_pk_key, sizeof(iot_pk_key)); 
	    
	    #ifdef CRY_DEBUG
		printf("Message (%d):\n", totallen);
		Print (enpk, totallen, 0);
	    #endif 
	    
	    pubmsgkey.payload = enpk;
	    pubmsgkey.payloadlen = totallen;
	    pubmsgkey.qos = QOS;
	    pubmsgkey.retained = 0;
	    char  t[50]; 
	    sprintf(t, "MK/ClientID1/value/%d",i+1); 
	    MQTTClient_publishMessage(client, t, &pubmsgkey, &token);

	    #ifdef TIME_DEBUG

		printf("\t\Time to encrypt the Master key %d us \n", (ee.tv_usec - es.tv_usec));
	    #endif
	    
	}
         gettimeofday(&end, NULL);
	 
         #ifdef TIME_DEBUG
		printf("\tTime to send  %d  Master key = %d us \n", NKS, (end.tv_usec - start.tv_usec));
	    #endif
 }
 
 
 
 
 
 /*
  * receive the aknowlgment 
  * E_MK(nonce)
  */ 
   int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{


    int i;
    struct timeval start, end;
    struct timeval es, ee;
    char* payloadptr;
    
    
    #ifdef DBUG
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    #endif
    
     const char delimiter[2] = "/";
    char* token1 = strtok(topicName, delimiter);
    char* token2 = strtok(NULL, delimiter);
    char* token3 = strtok(NULL, delimiter);
    char* token4 = strtok(NULL, delimiter);
    
    
    
    gettimeofday(&start, NULL);
    
    payloadptr = message->payload;
    int kytopic= strcmp(token3, "ack");
   
    if (kytopic ==0)
    {
	int ksid = atoi(token4) -1 ; /*ks ids start with 1 while in the list with 0 */
	unsigned char  hpk[hashlen];
	unsigned char  cipmsg[hashlen];
	unsigned char  plainmsg[hashlen];
	unsigned char  plaintext_nonce[BLOCK_SIZE];
	unsigned char  iv[BLOCK_SIZE];
	#ifdef DBUG
	printf(" ACK msg : \n");
	Print((uint8_t *) payloadptr, BLOCK_SIZE+hashlen, 0);
	#endif
	
	memcpy(cipmsg, payloadptr, hashlen); 
	memcpy(iv, payloadptr+hashlen, BLOCK_SIZE); 
	#ifdef DBUG
	printf(" encrpted  nonce XOR hpk : \n");
	Print(cipmsg, hashlen,0); 
	
	printf(" IV :\n"); 
	Print(iv, BLOCK_SIZE,0); 
	#endif
	  
	gettimeofday(&es, NULL);
	int l = openssl_aes_decryption(cipmsg,hashlen,LIST[ksid].masterkey, iv, plainmsg);
	gettimeofday(&ee, NULL); 
	
	#ifdef DBUG
	
	printf("XORing \n"); 
	Print(plainmsg, l, 0); 
	#endif
	
	
	//calculate nonce||nonce XOR plainmsg 
	for (int i =0 ; i< hashlen ; i++)
	    hpk[i] = LIST[ksid].nonce[i%16] ^plainmsg[i]; 
	
    
	#ifdef DBUG
	 printf(" LIST[%d].nonce  : \n", ksid);
	 Print(LIST[ksid].nonce, BLOCK_SIZE,0);
	
	 printf("calculated h(pk) : \n");
	 Print(calhpk, hashlen,0);
	#endif
	
        if (memcmp(hpk, calhpk,hashlen)==0)
	{
	    gettimeofday(&end, NULL);
	    
	    #ifdef TIME_DEBUG
	    printf("\tTime to process the AK from KS = %d us \n", (end.tv_usec - start.tv_usec));
	    printf("\t\Time to decrypt the Nonce = %d us \n", (ee.tv_usec - es.tv_usec));
	    #endif
		setup[ksid] = 1; 
		#ifdef DBUG
		    printf(" Nonce  and hpk are equal\n");  
		#endif
	  }
	    else 
	    {
		setup[ksid] = 0;
		#ifdef DBUG
		    printf(" hpk  and nonce are not equal\n");  
		#endif 
	    }
	
    } 
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
 

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    #ifdef DBUG
    printf("Message with token value %d delivery confirmed\n", dt);
    #endif
}

void connlost(void *context, char *cause)
{
    #ifdef DBUG
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
    #endif
}




/*
 * check howm many  correct ack publiser has received
 */ 
int recevedak()
{
    int count = 0 ;
    for (int i = 0; i< NKS; i++)
	count +=setup[i]; 
    return count; 
}

/*
 * Publisher
 * input:  ks1_PK ks2_pk, ... , ksNKS_pk
 */
int main(int argc, char* argv[])
{
 
   
    int value = 0 ;
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_create(&client, ADDRESS, CLIENTID,
    MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    int rc;

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
     

    struct timeval curTime, lastTime;
    struct timeval start, end, ssndk,esndk,ssnddata,esnddata; 
    struct timeval rs, ce,p1e,p2e, re;
    


    rs = (struct timeval){0};
    ce = (struct timeval){0};
    p1e = (struct timeval){0};
    p2e = (struct timeval){0};
    re = (struct timeval){0}; 
    
   
    setup_list(argv);
    setup_h();
     
    if (!MQTTClient_isConnected(client))
      {
	
	
	gettimeofday(&rs, NULL);
	
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	    {
	       printf("Failed to connect, return code %d\n", rc);
	       exit(-1);
	    }

	}
	gettimeofday(&ce, NULL);
	#ifdef TIME_DEBUG
	  printf("Time to connect with broker= %d us \n", (ce.tv_usec - rs.tv_usec));
        #endif
	printf("connected.! IOT devices will register with KS...\n");
	MQTTClient_subscribe(client, "MK/ClientID1/ack/#", 0);
	
	MKregister(client);
	gettimeofday(&p1e, NULL);
	#ifdef TIME_DEBUG
	  printf("\tTime to phase1 = %d us \n", (p1e.tv_usec - ce.tv_usec));
        #endif
	printf("Waiting the aknolgment from KS....\n");
	
	 while (recevedak()!=NKS)
	{  
	  sleep(0.001);
	 
	}
	gettimeofday(&p2e, NULL);
	gettimeofday(&re, NULL);
	#ifdef TIME_DEBUG
	  printf("\tTime to phase2 = %d us \n", (p2e.tv_usec - p1e.tv_usec));
        	
	  printf("Time to setup connection with KS:%d \n", re.tv_usec-rs.tv_usec);
	#endif
	
	MQTTClient_disconnect(client, 10000);
	

    gettimeofday(&lastTime, NULL);

    printf("Size\tMSG_Id\tTTconnect\tTTsigncre\tTTenckey\tTTsendkey\tTTencydata\tTTsendDate\n", rc);
    int sizes[] = {1024, 5120,10240,15360,20480,25600};
    for (int ind= 0; ind<6; ind++)
    {
	Size_Byte = sizes[ind];

	uint8_t * mymsg  =rand_string_alloc(Size_Byte);
	value =0;

       while(1==1)
       {
	    gettimeofday(&curTime, NULL);
	    if ( curTime.tv_sec - lastTime.tv_sec  > MT)
	    {
		    lastTime = curTime;
		    value ++;
		    if ( !MQTTClient_isConnected(client))
		    {
			    gettimeofday(&start, NULL);
			    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
			    {
				    printf("Failed to connect, return code %d\n", rc);
				    exit(-1);
			    }
			    gettimeofday(&end, NULL);
			    #ifdef TIME_DEBUG
				printf("%d\t%d\t%d\t",Size_Byte, value,(end.tv_usec - start.tv_usec));
			    #endif
		    }

		    //if  (value ==1)
		  //  {
		    gettimeofday(&ssndk, NULL);
		    SesKeytransmission(value, client);
		    gettimeofday(&esndk, NULL);
		   #ifdef TIME_DEBUG
			printf("%d\t",esndk.tv_usec - ssndk.tv_usec); 
		   #endif
		//    }
		 //   else 
		//    printf(" \t\t \t\t \t\t"); 
		  
		     
		    gettimeofday(&ssnddata, NULL);
		    Datatransmission(mymsg, value, client);
		    gettimeofday(&esnddata, NULL);
		      #ifdef TIME_DEBUG
			printf("%d\t",esnddata.tv_usec - ssnddata.tv_usec); 
		    #endif


		    MQTTClient_disconnect(client, 10000);
		    printf("\n");

	}
	    if(value >100)
		break;

	}
	printf("--------------------------------\n");
	free(mymsg);
    }


     MQTTClient_destroy(&client);
    return rc;
}



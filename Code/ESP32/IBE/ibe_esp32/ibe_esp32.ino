#include <math.h>
#include <string.h>
#include <pbc.h>
#include <mbedtls/ssl.h>
#include <mbedtls/md.h>
#include <mbedtls/error.h>


#define TYPEA_PARAMS \
"type a\n" \
"q 87807107996633125224377819847540498158068831994142082" \
"1102865339926647563088022295707862517942266222142315585" \
"8769582317459277713367317481324925129998224791\n" \
"h 12016012264891146079388821366740534204802954401251311" \
"822919615131047207289359704531102844802183906537786776\n" \
"r 730750818665451621361119245571504901405976559617\n" \
"exp2 159\n" \
"exp1 107\n" \
"sign1 1\n" \
"sign0 1\n"

//#define DBG_MSG


#define HASH_LEN 32
#define PRNT 8


int sizelist[9]={1,8, 16,32,64,128, 256,512, 1024}; 
unsigned long lastMsg = 0;
int loop_times  = 30;
unsigned long stime, etime, p1time, p2time, p3time, p4time, sumtime, sumtimep, sumtimexor, sumtimepow, sumtimemul;
char *id = "test@tum.de";

void setup() {

    Serial.begin(115200);
    printf("MsgSZ\t Setup \t\t Extract \t  Enc(OTO) \t Enc \t\t Dec\n");


    for (int indx = 0 ; indx < 10; indx++)
    {
      printf("********************************************************************************\n");
      int msgsize = sizelist[indx]; 
      char m[msgsize] = {0}; 
      char mv[] = {0}, c[msgsize] = {0};

 
    /***initialize the message which is going to be signed***/
    esp_fill_random(m, msgsize);
  

    for (int loopindx = 0 ; loopindx< loop_times; loopindx++)
    {
      //Serial.println(">>>>>>>>>> Time Summary >>>>>>>>>>");
      int i = 0, j = 0;
      /***
       * PKG initialization
      ***/
      #ifdef DBG_MSG
      Serial.println(">>>>>>>>>> Phase 1: Setup >>>>>>>>>>");
      #endif

      stime = micros();
      pairing_t pairing;
      element_t P, Ppub, Did, Qid, U, s, r, xt, gid;
      unsigned char hash[HASH_LEN] = {0};
      unsigned char *gs = NULL;

      /***
       * pairing function initalization from the input file which contains the pairing parameters
      ***/
      pairing_init_set_buf(pairing, TYPEA_PARAMS, strlen(TYPEA_PARAMS));
      if (!pairing_is_symmetric(pairing)) pbc_die("pairing must be symmetric");

      /***
       * initialization of G1 elements
      ***/
      element_init_G1(P, pairing);
      element_init_G1(Ppub, pairing);
      element_init_G1(Qid, pairing);
      element_init_G1(Did, pairing);
      element_init_G1(U, pairing);

      /***
       * initialization of Zr elements
      ***/
      element_init_Zr(s, pairing);
      element_init_Zr(r, pairing);

      /***
       * initialization of GT elements
      ***/
      element_init_GT(gid, pairing);
      element_init_GT(xt, pairing);

      /***
      * PKG generation of P, s and Ppub
      ***/
      element_random(P);
      element_random(s);
    
      element_mul_zn(Ppub, P, s);

      etime = micros();
      p1time = etime - stime;
    
   
      #ifdef DBG_MSG 
      element_printf("++s: %B\n",s);
      element_printf("++P:  %B\n", P);
      element_printf("++Ppub: %B\n", Ppub);
      #endif

      //Serial.printf("Setup time:\t\t\t%lu (us)\n", p1time);

      /***
       * key generation
       ***/
      #ifdef DBG_MSG
      Serial.println(">>>>>>>>>> Phase 2: Extract >>>>>>>>>>");
      #endif

      stime = micros();
      int hash_ret;

      unsigned long qtime = micros();
      hash_ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (unsigned char *)id, strlen(id), hash);
      if(hash_ret != 0) {
        Serial.println("failed to hash ID!");
      }

     element_from_hash(Qid, hash, HASH_LEN);
   
      element_mul_zn(Did, Qid, s);

      etime = micros();
      p2time = etime - stime;
   
      #ifdef DBG_MSG 
      element_printf("++Qid: %B\n", Qid);
      element_printf("++Did: %B\n", Did);
      #endif

      //Serial.printf("Extract time:    \t\t%lu (us)\n", p2time);
      /***
       * encryption
       ***/
      #ifdef DBG_MSG
       Serial.println(">>>>>>>>>> Phase 3: Encryption >>>>>>>>>>");
      #endif

      unsigned long start = micros();
      element_pairing(gid, Qid, Ppub); //Happen once 
      unsigned long onccover = micros() - start; 
      //Serial.printf("Onetime Enc overhead:      \t%lu (us)\n", onccover);
      start = micros();
      element_random(r);

      element_mul_zn(U, P, r);

      element_pow_zn(gid, gid, r);

      gs = (unsigned char *)malloc(element_length_in_bytes(gid));
      element_to_bytes(gs, gid);

      #ifdef DBG_MSG
      Serial.printf("gid length: %d\n", element_length_in_bytes(gid));
      Serial.printf("string length of gs: %d\n", strlen((char *)gs));
      Serial.printf("id length: %d\n", strlen(id));
      Serial.printf("size of gs: %d\n", sizeof(gs));
      #endif
      hash_ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (unsigned char *)gs, element_length_in_bytes(gid), hash);
      if (hash_ret != 0) {
        Serial.println("failed to hash gs!");
        }

        j = 0;
        for (i = 0; i < msgsize; i++) {
            if (j >= HASH_LEN) {
                j = 0;
            }
            c[i] = m[i] ^ hash[j];
            j++;
           
        }
        free(gs);
        
        unsigned long endenc = micros() - start;
        //Serial.printf("Encryption time:\t\t%lu (us)\n", endenc);

      #ifdef DBG_MSG
       element_printf("++r: %B\n",r);
       element_printf("++U: %B\n",U);
       element_printf("++gid: %B\n",gid);
      #endif
      /***
    decryption
    ***/
    #ifdef DBG_MSG
    Serial.println(">>>>>>>>>> Phase 4: Decryption >>>>>>>>>>");
    #endif
    
    sumtime = 0;
    unsigned long p4time = micros();
    element_pairing(xt, Did, U);
    gs = (unsigned char *)malloc(element_length_in_bytes(xt));
    element_to_bytes(gs, xt);
    hash_ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (unsigned char *)gs, element_length_in_bytes(xt), hash);
    if(hash_ret != 0) {
      Serial.println("failed to hash gs in decryption");
      }
     j = 0;
     for (i = 0; i < msgsize; i++) {
       if (j >= HASH_LEN) {
              j = 0;
            }
        mv[i] = c[i] ^ hash[j];
        j++;
        }
        free(gs);

        etime = micros();
        p4time = etime - p4time;
       // Serial.printf("Decryption time: \t\t%lu (us)\n", p4time);

        #ifdef DBG_MSG
    element_printf("e(Did,U):%B\n",xt);
    #endif

    //printf("Result: %d\n",strncmp(m,mv,msgsize));

    #ifdef DBG_MSG
    printf("message in plaintext: \n");
    PrintHEX((unsigned char *)m, msgsize-32, msgsize);

    printf("decrypted message: \n");
    PrintHEX((unsigned char *)mv, msgsize-32, msgsize);

    printf("encrypted message: \n");
    PrintHEX((unsigned char *)c, msgsize-32, msgsize);
    #endif

    printf("%d \t %lu \t%lu \t %lu \t %lu \t %lu\n", msgsize, p1time,p2time,onccover,endenc,p4time );


    /***free mem***/
    element_clear(P);
    element_clear(Ppub);
    element_clear(Qid);
    element_clear(Did);
    element_clear(U);
    element_clear(gid);
    element_clear(r);
    element_clear(xt);
    element_clear(s);
    pairing_clear(pairing);
     }
  }  
}


void loop() {

}

void PrintHEX( unsigned char* str, int start_byte, int end_byte) {
    if (start_byte < 0)
        start_byte = 0;
    
    for (int i = start_byte; i < end_byte; ++i) {
        printf("%.2X ", str[i]);
      if (i+1 %PRNT ==0)
         printf("\n");    
    }
    printf("\n");
}

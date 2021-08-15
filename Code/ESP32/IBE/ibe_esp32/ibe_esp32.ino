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

#define DBG_MSG


#define enc_dec_times 1
#define MSG_SIZE 6
#define HASH_LEN 32
#define PRNT 8

unsigned long lastMsg = 0;
int value = 0;
int loop_times;
unsigned long stime, etime, p1time, p2time, p3time, p4time, sumtime, sumtimep, sumtimexor, sumtimepow, sumtimemul;
char m[MSG_SIZE] = {0}; 
char mv[MSG_SIZE] = {0}, c[MSG_SIZE] = {0};

void setup() {

    Serial.begin(115200);

    
    int i = 0, j = 0;
    char *id = "test@tum.de";


    /***initialize the message which is going to be signed***/
    esp_fill_random(m, MSG_SIZE);

    Serial.printf("Message:"); 
    PrintHEX((unsigned  char *)m, 0,MSG_SIZE );

    /***
    PKG initialization
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
    pairing function initalization from the input file which contains the pairing parameters
    ***/
    pairing_init_set_buf(pairing, TYPEA_PARAMS, strlen(TYPEA_PARAMS));
    if (!pairing_is_symmetric(pairing)) pbc_die("pairing must be symmetric");

    /***
    initialization of G1 elements
    ***/
    element_init_G1(P, pairing);
    element_init_G1(Ppub, pairing);
    element_init_G1(Qid, pairing);
    element_init_G1(Did, pairing);
    element_init_G1(U, pairing);

    /***
    initialization of Zr elements
    ***/
    element_init_Zr(s, pairing);
    element_init_Zr(r, pairing);

    /***
    initialization of GT elements
    ***/
    element_init_GT(gid, pairing);
    element_init_GT(xt, pairing);

    /***
    PKG generation of P, s and Ppub
    ***/
    element_random(P);
    element_random(s);
    
    unsigned long multime = micros();
    element_mul_zn(Ppub, P, s);

    etime = micros();
    p1time = etime - stime;
    multime = etime - multime;
   
    #ifdef DBG_MSG 
    element_printf("++s: %B\n",s);
    element_printf("++P:  %B\n", P);
    element_printf("++Ppub: %B\n", Ppub);
    #endif

    Serial.printf("Setup time:\t\t%lu (us)\n", p1time);
    Serial.printf("Mul time:\t\t%lu (us)\n", multime);

    /***
    key generation
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
    unsigned long hqtime = micros() - qtime;

    element_from_hash(Qid, hash, HASH_LEN);

    multime = micros();
   
    element_mul_zn(Did, Qid, s);

    etime = micros();
    p2time = etime - stime;
    qtime = multime - qtime;
    multime = etime - multime;
   
    #ifdef DBG_MSG 
    element_printf("++Qid: %B\n", Qid);
    element_printf("++Did: %B\n", Did);
    #endif

    Serial.printf("Extract time:    \t\t%lu (us)\n", p2time);
    Serial.printf("Hash Qid time:   \t\t%lu (us)\n", hqtime);
    Serial.printf("Element from hash time: \t%lu (us)\n", qtime - hqtime);
    Serial.printf("Extract Did time:\t\t%lu (us)\n", multime);

    /***
    encryption
    ***/
    #ifdef DBG_MSG
    Serial.println(">>>>>>>>>> Phase 3: Encryption >>>>>>>>>>");
    #endif

    loop_times = enc_dec_times;

   //unsigned long p3pair, p3xor, p3time, p3times, p3pow, p3mul;
    unsigned long start = micros();
    element_pairing(gid, Qid, Ppub); //Happen once 
    unsigned long onccover = micros() - start; 
    Serial.printf("One time overhead for Encryption: %d\n", onccover);
    while(loop_times) {
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
        for (i = 0; i < MSG_SIZE; i++) {
            if (j >= HASH_LEN) {
                j = 0;
            }
            c[i] = m[i] ^ hash[j];
            j++;
           
        }
        Serial.printf("Cipher Text:"); 
        PrintHEX((unsigned char *)c, 0,MSG_SIZE );
        free(gs);

         unsigned long endenc = micros() - start;
        Serial.printf("time p3: \t%lu (us)\n", endenc);

       // Serial.printf("iteration: %d\n", loop_times);
          //Serial.printf("time pairing: \t%lu (us)\n", p3pair);
          //Serial.printf("time mul: \t%lu (us)\n", p3mul);
          //Serial.printf("time pow: \t%lu (us)\n", p3pow);
          //Serial.printf("time xor: \t%lu (us)\n", p3xor);
          //Serial.printf("time p3: \t%lu (us)\n", p3time);

        loop_times -= 1;
    }
    

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

    loop_times = enc_dec_times;
    sumtime = 0;
    sumtimep = 0;
    sumtimexor = 0;
    unsigned long p4time;
    unsigned long p4times;
    unsigned long p4pair;
    unsigned long p4xor;

    while(loop_times) {

        p4times = micros();
        element_pairing(xt, Did, U);
        p4pair = micros() - p4times;

        gs = (unsigned char *)malloc(element_length_in_bytes(xt));
        element_to_bytes(gs, xt);

        hash_ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (unsigned char *)gs, element_length_in_bytes(xt), hash);
        if(hash_ret != 0) {
            Serial.println("failed to hash gs in decryption");
        }

        stime = micros();
        j = 0;
        for (i = 0; i < MSG_SIZE; i++) {
            if (j >= HASH_LEN) {
                j = 0;
            }
            mv[i] = c[i] ^ hash[j];
            j++;
        }
        Serial.printf("Plain text:"); 
        PrintHEX((unsigned char *)mv, 0,MSG_SIZE );
        free(gs);

        etime = micros();
        p4xor = etime - stime;
        p4time = etime - p4times;

        sumtime = sumtime + p4time;
        sumtimep = sumtimep + p4pair;
        sumtimexor = sumtimexor + p4xor;

        Serial.printf("iteration: %d\n", loop_times);
        Serial.printf("time pairing: \t%lu (us)\n", p4pair);
        Serial.printf("time xor: \t%lu (us)\n", p4xor);
        Serial.printf("time p4: \t%lu (us)\n", p4time);

        loop_times -= 1;
    }
    
    p4pair = sumtimep/enc_dec_times;
    p4xor = sumtimexor/enc_dec_times;
    p4time = sumtime/enc_dec_times;

    #ifdef DBG_MSG
    element_printf("e(Did,U):%B\n",xt);
    #endif

    printf("Result: %d\n",strncmp(m,mv,MSG_SIZE));

    #ifdef DBG_MSG
    printf("message in plaintext: \n");
    PrintHEX((unsigned char *)m, MSG_SIZE-32, MSG_SIZE);

    printf("decrypted message: \n");
    PrintHEX((unsigned char *)mv, MSG_SIZE-32, MSG_SIZE);

    printf("encrypted message: \n");
    PrintHEX((unsigned char *)c, MSG_SIZE-32, MSG_SIZE);
    #endif

    Serial.println(">>>>>>>>>> Time Summary >>>>>>>>>>");
    Serial.printf("Setup time:      \t\t%lu (us)\n", p1time);
    Serial.printf("Extract time:    \t\t%lu (us)\n", p2time);
    Serial.printf("Encryption time: \t\t%lu (us)\n", p3time);
    Serial.printf("Encryption Hash Qid time: \t%lu (us)\n", qtime);
    //Serial.printf("Encryption pairing time: \t%lu (us)\n", p3pair);
    //Serial.printf("Encryption mul time: \t\t%lu (us)\n", p3mul);
      //Serial.printf("Encryption pow time: \t\t%lu (us)\n", p3pow);
      //Serial.printf("Encryption xor time: \t\t%lu (us)\n", p3xor);
    Serial.printf("Decryption time: \t\t%lu (us)\n", p4time);
    Serial.printf("Decryption pairing time: \t%lu (us)\n", p4pair);
    Serial.printf("Decryption xor time: \t\t%lu (us)\n", p4xor);

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

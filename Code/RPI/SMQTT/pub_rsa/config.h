
#ifndef CONFIG_H_
#define CONFIG_H_

#include "../lib/dbg.h"
#include "../lib/ossl.h"

/* print dbg messages*/
//#define DBUG

/* print the time measurment */
#define TIME_DEBUG

/* by enable   that we enforrce the connection to be closed after  each transmission */
//#define RECON

/* print crypto debug messages*/
//#define CRY_DEBUG

/* print message reklted to AES*/
//#define CRY_DEBUG_AES
//#define CRY_DEBUG_RSA_SIGN



//#define OSLOENABLE


#define ADDRESS     "tcp://192.168.0.109:1883"
#define CLIENTID    "ExampleClientPub"
#define DATA         "data"
#define MK           "MK"
#define SK           "SK"
#define CR           "CR"
#define QOS          0
#define TIMEOUT     10000L
#define MT 	    3




#define PKLEN 273
#define  hashlen  32

int Size_Byte =0;



/* H(iot_pk) */
unsigned char calhpk[hashlen]; 

/* H(t) */
unsigned char calht[hashlen]; 

/* 
 * Number of KS. at leass we should have 1 KS
 */
#define NKS   2


/*
 * Tuples <>
 */ 

typedef struct tuples {
 unsigned char  ks_pk[PKLEN]; 
 uint8_t masterkey[BLOCK_SIZE] ; 
 uint8_t nonce[BLOCK_SIZE] ;
} TUPLE ;

TUPLE LIST[NKS]; 

/* 
 * Array to trace the received and validated ack
 */ 
 int setup[NKS] = {0}; 
 
 
 /*
  * List of shares 
  */
   uint8_t  shares[NKS][BLOCK_SIZE] = {0};
   
   
   
   /* in case we will use BEARSSL: 
    *  To extract this: use PEM to XML to get all this parameters(https://superdry.apphb.com/tools/online-rsa-key-converter), then Base64 to Hex (https://base64.guru/converter/decode/hex).
    */ 

#endif

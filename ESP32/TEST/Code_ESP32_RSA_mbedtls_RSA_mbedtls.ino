#include "mbedtls/pk.h"
#include "mbedtls/rsa.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/entropy_poll.h"

#define BLOCK_SIZE 16
#define P_SIZE 16 
#define C_SIZE 128

//#define DBG_PRINT 


/* public key 1024 bit */
 unsigned char internal_key[]= "-----BEGIN PUBLIC KEY-----\n"\
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDlIE7AG22mns40Zv6XIsKie61w\n"\
"xZy6NmAON1mFU3bgYy5lEA3X0VHRJtsAwnMTGNzBc/hw8VL/2xARn7Xh7oOCLm+I\n"\
"eCqDnfJUE9u7u1ayZfUNojoNbj7B8AT2j3O/LPsna67l97zspD11DTe/cpL8IKlj\n"\
"acXs2HjxLGXtX2Tv8wIDAQAB\n"\
"-----END PUBLIC KEY-----\n";

unsigned char internal_pr_key[]= "-----BEGIN RSA PRIVATE KEY-----\n"\
"MIICXgIBAAKBgQDlIE7AG22mns40Zv6XIsKie61wxZy6NmAON1mFU3bgYy5lEA3X\n"\
"0VHRJtsAwnMTGNzBc/hw8VL/2xARn7Xh7oOCLm+IeCqDnfJUE9u7u1ayZfUNojoN\n"\
"bj7B8AT2j3O/LPsna67l97zspD11DTe/cpL8IKljacXs2HjxLGXtX2Tv8wIDAQAB\n"\
"AoGBAKJv+ySazWv43fusoQWI8S+8OsPedTRW5FZvyxmIiJif9/5SGV5N2ZFL2Kt7\n"\
"L5X0ZF0J2LJJhCw7o/K6pXmdXMEQkrKoTwhObwWfbHXmb6KDBQJ2cFoV1BwO/K6O\n"\
"xQtPOC4C4Qg0GeVxkK3U1Isr8CKuK9w4huuSUbCZIVkBrWYxAkEA+gXgpTaTwVvQ\n"\
"NrBRRvY/HQooSm0oJvgQWvAoWMS7Psrq80nyrFq+L80SMbFrNizUVqJkDTJ5vCrR\n"\
"2zFHl/UBfwJBAOqaix9/cZbWKjc7zlJcgAJRDUgXvSpeyJMVJk7+1L4LwQg6v++n\n"\
"jahRwSrU9HuLy6a6YtAUK8A0wsTNn4OhY40CQEiTjDJpNVrbnAVECVPgB1IWsLtq\n"\
"FE27lIPRUuTxtqjznmAhByCg9rRJgw6JweVn/ftoCEMMc+n7RA+srmud42ECQQCT\n"\
"jkhas0m6WVGME+2dF9jbnCcCZQvxjlQRTogdv2IHdlUe2/TEw1nxuBoK2o469p5M\n"\
"a4ai4UzzQzwNlFbz75oxAkEAgu4TyqG8mr0vAlA3RWVtd41tulzNTN0N85ia5SnV\n"\
"17399lTOp6j/qyMlhQVZZKP9T7ndYCqJdjHPNStOpuAWiA==\n"\
"-----END RSA PRIVATE KEY-----\n";


void Print(uint8_t * arr, int start, int size)
{
  Serial.println();
 // int start = size -32; //defualt = 0
  int row =0 ;
  Serial.print(row); 
  Serial.print(" >> ");  
  for (int i =start; i<size; i++)
  {
     if(i> 0 && i % BLOCK_SIZE ==0)
     {
      row++;
      Serial.println(); 
      Serial.print(row); 
      Serial.print(" >> ");
      
     }
    Serial.print(arr[i],HEX); 
    Serial.print(" "); 
   
  }
  Serial.println();
    
  }
static int myrand(void *rng_state, unsigned char *output, size_t len)
{
    size_t olen;
    return mbedtls_hardware_poll(rng_state, output, len, &olen);
}

void rsa_encryption(unsigned char * plainText, unsigned char * cipher)
{
  int error_code; 
  mbedtls_pk_context  pk; 
  mbedtls_pk_init(&pk);
   error_code =  mbedtls_pk_parse_public_key(&pk, internal_key, sizeof(internal_key));
  
   if( error_code!= 0)
   {
     #ifdef DBG_PRINT  
      Serial.println( "READING NOT SUCCESSFULL");
     #endif
     return ;  
   }
   
  else
  {
    #ifdef DBG_PRINT  
       Serial.println( "READING SUCCESSFULL");
       Serial.println(sizeof(plainText)); 
       Serial.println("Plain Text"); 
       Print(plainText, 0 ,P_SIZE); 
    #endif
    size_t oolen ;
    int result = mbedtls_pk_encrypt( &pk, plainText, P_SIZE, cipher, &oolen, C_SIZE, myrand, NULL );
    if (result == 0)
    {
      #ifdef DBG_PRINT 
        Serial.println( "Encryption SUCCESSFULL");
        Serial.println("Encrypted Text");
        Print(cipher, 0 ,oolen);
      #endif
    } 
    else
    {
      #ifdef DBG_PRINT 
      Serial.println( "Encryption NOT SUCCESSFULL");
      #endif
    }
  }
   
}
void rsa_decryption(unsigned char * cipher, unsigned char * cleartext)
{
    mbedtls_pk_context  pr; 
    mbedtls_pk_init(&pr);
    int error_code_pr ; 
    #ifdef DBG_PRINT 
      Serial.println("Cipher Text");
      Print(cipher, 0 ,C_SIZE); 
    #endif
    error_code_pr =  mbedtls_pk_parse_key(&pr, internal_pr_key, sizeof(internal_pr_key),NULL,NULL);
    if( error_code_pr != 0)
    {
       #ifdef DBG_PRINT 
       Serial.println( "READING NOT SUCCESSFULL");
       #endif
       return ;
    }
    else
    {
       #ifdef DBG_PRINT 
       Serial.println( "READING SUCCESSFULL");
       #endif
       size_t oplen;
       int resultdec  = mbedtls_pk_decrypt(&pr ,cipher,C_SIZE,cleartext, &oplen, P_SIZE, myrand, NULL); 
       if (resultdec == 0)
       {
         #ifdef DBG_PRINT
         Serial.println( "Decryption SUCCESSFULL");
         Serial.println("Decrypted Text");
         Print(cleartext, 0 ,oplen);
         #endif
       }
    else
    {
      #ifdef DBG_PRINT
       Serial.println( "Decryption NOT SUCCESSFULL");
       #endif
    }
  }
}




 
void setup() {
  Serial.begin(115200);
  
  unsigned char plainText[P_SIZE] = "Hello world";
  
  unsigned char cp[C_SIZE] ={}; 
  unsigned char out[C_SIZE]= {};
   
  
  unsigned long senc= micros(); 
  rsa_encryption(plainText, cp);
  unsigned long eenc= micros(); 
  rsa_decryption(cp, out);
  unsigned long edec= micros();
  Serial.print(" Time to encryption (us):"); 
  Serial.println( eenc -senc); 
  Serial.print(" Time to decryption (us):"); 
  Serial.println( edec -eenc); 


}

void loop() {
  // put your main code here, to run repeatedly:

}

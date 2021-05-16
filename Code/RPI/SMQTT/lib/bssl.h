#ifndef BSSL_H_
#define BSSL_H_
#include  "../../BearSSL/inc/bearssl.h"

#include "../lib/dbg.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>


#define BLOCK_SIZE 16   /* key size */
#define ENC_SESSKEY_SIZE 128




int  aes_encryption(uint8_t * pt , uint8_t * key, int len, uint8_t * iv);
int aes_decryption(uint8_t * cipher, uint8_t * key , int len, uint8_t * iv ) ;

int  rsa_encrypt(unsigned char * plaintext, int plainlen, unsigned char * output, unsigned char * n,int ln,  unsigned char * e, int le);
int rsa_decrypt(unsigned char * cipher,int module, char * parr , char * qarr , char * dprr, char * dqarr, char * iqarr);

void rsa_sign( char * msg ,  unsigned char * signedhash, int modula, char * parr , char * qarr , char * dparr, char * dqarr, char * iqarr);
#endif 

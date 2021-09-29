#ifndef SUB_H_
#define SUB_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <sys/stat.h>
#include <mosquitto.h>

#include "config.h"





int aes_encryption(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* iv, unsigned char* ciphertext);
int aes_decryption(unsigned char* cipher, int cipher_len, unsigned char* key, unsigned char* iv, unsigned char* plaintext);
void KsCallback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg);
void PrintHEX(unsigned char* str, int len);

unsigned char *  ReadShare(); 

#endif /* end of SUB_H_ */


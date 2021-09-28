#ifndef KEYSTORE_H_
#define KEYSTORE_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h> 
#include <fcntl.h>
#include <math.h>   
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <mosquitto.h>
#include <pbc/pbc.h>

#include "typ.h"
#include "db.h"

/* definition of curve parameters */
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

unsigned char kpub[ELEMENT_LEN] = {
0x33, 0x14, 0x25, 0x4B, 0x28, 0x8C, 0xEB, 0xDD, 0x0B, 0x3F, 0xDE, 0xCB, 0xF3, 0x61, 0x97, 0x77, 0x57, 0x42, 0xEE, 0x44, 0x61, 0xB4, 0x16, 0xDB, 0x4F, 0xA9, 0x7F, 0xCE, 0x80, 0x36, 0xD3, 0xCF, 0xB5, 0x1D, 0x3D, 0xD7, 0xC7, 0x78, 0x54, 0x1C, 0x1C, 0xF5, 0x10, 0x98, 0x08, 0x37, 0x78, 0x01, 0xC6, 0xB0, 0x87, 0x27, 0xC0, 0xC8, 0xA2, 0xE9, 0x7B, 0x2A, 0x67, 0x34, 0x2E, 0x90, 0x69, 0x04, 0x2A, 0xB8, 0x61, 0x04, 0xEE, 0x91, 0x9A, 0x3D, 0x26, 0xDA, 0x04, 0x20, 0x08, 0xD6, 0xF7, 0x19, 0x00, 0x15, 0x63, 0xF4, 0x65, 0x2F, 0x28, 0xE3, 0x38, 0xAB, 0xD9, 0x79, 0x60, 0xC3, 0x17, 0x07, 0xB2, 0xA8, 0x02, 0xE4, 0xF1, 0x0C, 0x26, 0xCE, 0x37, 0xA3, 0x57, 0xC2, 0x67, 0xA4, 0xBF, 0xB8, 0xAC, 0xEE, 0x74, 0x8C, 0x61, 0xEC, 0xCA, 0x70, 0x52, 0x25, 0xF9, 0xDC, 0x80, 0x20, 0x0D, 0x17};

unsigned char p[ELEMENT_LEN] = {
0x8C, 0x3E, 0xAC, 0x37, 0x06, 0x8D, 0x74, 0x57, 0x1C, 0x3C, 0xB6, 0x18, 0x0B, 0xE5, 0xE8, 0xCD, 0xAC, 0x20, 0xCD, 0x2D, 0x87, 0x87, 0xCE, 0xCF, 0x99, 0x51, 0xE4, 0x44, 0x75, 0x88, 0x31, 0x4D, 0x43, 0xDE, 0xDC, 0xFA, 0x51, 0xC8, 0xD6, 0x64, 0x0F, 0x24, 0x4A, 0x04, 0x2E, 0x26, 0xCE, 0xC1, 0x98, 0xF6, 0x7E, 0x39, 0x91, 0xED, 0x88, 0xA6, 0x1B, 0xF4, 0x9E, 0x83, 0x80, 0xCB, 0x28, 0x57, 0x0E, 0x66, 0x4F, 0x5C, 0x82, 0x85, 0x8C, 0xBF, 0xC5, 0xCC, 0x32, 0x3F, 0x73, 0x55, 0x7F, 0x1A, 0x16, 0xD5, 0xF4, 0xA0, 0x94, 0x1C, 0x75, 0x29, 0x25, 0x46, 0x0E, 0x9E, 0x68, 0x72, 0xD5, 0x87, 0x62, 0x57, 0x3D, 0x8F, 0x19, 0x5D, 0xAB, 0xBB, 0x21, 0xA8, 0x4B, 0x3A, 0x84, 0xB4, 0x62, 0x80, 0xA3, 0x9B, 0xC1, 0x94, 0x40, 0xDC, 0x71, 0xA5, 0x77, 0xF3, 0xD4, 0x0F, 0x47, 0xB2, 0xA2, 0x78};

unsigned char km[20] = {
0x04, 0xCB, 0x38, 0x51, 0x61, 0xC3, 0x0E, 0x1B, 0x77, 0x49, 0xB8, 0x98, 0x7D, 0x7B, 0x43, 0x16, 0x46, 0x09, 0x7A, 0x09};


typedef struct ibe_sys_param_public {
    element_t Kpub;
    element_t P;
    pairing_t pairing;
} ibe_sys_param_public_t;

typedef struct ibe_sys_param_private {
    element_t Km;
} ibe_sys_param_private_t;

ibe_sys_param_public_t ibe_param_pub;
ibe_sys_param_private_t ibe_param_pri;

int aes_encryption(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* iv, unsigned char* ciphertext);
int aes_decryption(unsigned char* cipher, int cipher_len, unsigned char* key, unsigned char* iv, unsigned char* plaintext);
RSA* createRSA(unsigned char* key, int pubkey);
int rsa_encryption(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* cipher);
int rsa_decryption(unsigned char* enc_data, int data_len, unsigned char *key, unsigned char* dec_data);
int rsa_signature_verify(const unsigned char* m, unsigned int m_length, unsigned char* sigbuf, unsigned int siglen, unsigned char* pub_key);
void ibe_extract(unsigned char *Didc);
void ibe_encryption(unsigned char *plaintext, size_t plaintext_len , unsigned char *cipher, unsigned char *Uc);
int ibe_decryption(unsigned char *ciphertext, size_t ciphertext_len, unsigned char *Uc, unsigned char *Didc, unsigned char *plaintext);
void KsCallback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg);
void openssl_hash(unsigned char  * m, int len, unsigned char * hash);
void PrintHEX(unsigned char* str, int len);

#endif /* end of KEYSTORE_H_ */

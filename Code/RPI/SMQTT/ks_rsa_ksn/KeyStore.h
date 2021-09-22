#ifndef KEYSTORE_H_
#define KEYSTORE_H_

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



//#define CRYP_DBG
//#define DBG
#define SELF_TEST
#define TOPIC_SIZE 50
#define BLOCK_SIZE 16
#define NONCE_SIZE 16
#define MAX_TEXT_SIZE 1024
#define PKLEN 273

int aes_encryption(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* iv, unsigned char* ciphertext);
int aes_decryption(unsigned char* cipher, int cipher_len, unsigned char* key, unsigned char* iv, unsigned char* plaintext);
RSA* createRSA(unsigned char* key, int pubkey);
int rsa_encryption(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* cipher);
int rsa_decryption(unsigned char* enc_data, int data_len, unsigned char *key, unsigned char* dec_data);
int rsa_signature_verify(const unsigned char* m, unsigned int m_length, unsigned char* sigbuf, unsigned int siglen, unsigned char* pub_key);
void KsCallback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg);
 void openssl_hash(unsigned char  * m, int len, unsigned char * hash);
void PrintHEX(unsigned char* str, int len);

#endif /* end of KEYSTORE_H_ */


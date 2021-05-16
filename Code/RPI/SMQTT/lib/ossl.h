#ifndef OSSL_H_
#define OSSL_H_

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


#define SELF_TEST
#define TOPIC_SIZE 50
#define BLOCK_SIZE 16
#define NONCE_SIZE 16
#define MAX_TEXT_SIZE 1024


#define BLOCK_SIZE 16   /* key size */
#define ENC_SESSKEY_SIZE 128
#define ENC_SESSKEY_SIZE 128




int openssl_aes_encryption(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* iv, unsigned char* ciphertext);
int openssl_aes_decryption(unsigned char* cipher, int cipher_len, unsigned char* key, unsigned char* iv, unsigned char* plaintext);
RSA* openssl_createRSA(unsigned char* key, int pubkey);
int openssl_rsa_encryption(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* cipher);
int openssl_rsa_decryption(unsigned char* enc_data, int data_len, unsigned char *key, unsigned char* dec_data);
int openssl_rsa_signature_verify(const unsigned char* m, unsigned int m_length, unsigned char* sigbuf, unsigned int siglen, unsigned char* pub_key);
int openssl_rsa_sign(const unsigned char * m , unsigned int len, unsigned char * sign, unsigned int siglen,  unsigned char * key); 
int openssl_rsa_verfiy(const unsigned char * m , unsigned int len, unsigned char * sign, unsigned int siglen,  unsigned char * pubky);
void openssl_hash(unsigned char  * m, int len, unsigned char *hash);


#endif /* end of OSSL_H_ */

#include "ossl.h"

#include "dbg.h"

RSA* openssl_createRSA(unsigned char* key, int pubkey) {

    RSA* rsa = NULL;
    BIO* keybio = NULL;
    keybio = BIO_new_mem_buf(key, -1);

    if (keybio == NULL) {
        printf("Failed to create key BIO \n");
    } else if (pubkey) {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
    } else {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    }

    return rsa;
}


int openssl_rsa_encryption(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* cipher) {

    int rc = 0;
    RSA* rsa = openssl_createRSA(key, 1);

    if (rsa == NULL) {
        rc = -1;
    } else {
        rc = RSA_public_encrypt(plaintext_len, plaintext, cipher, rsa, RSA_PKCS1_PADDING);
    }

    return rc;
}

int openssl_rsa_decryption(unsigned char* enc_data, int data_len, unsigned char *key, unsigned char* dec_data) {

    int rc = 0;
    RSA* rsa = openssl_createRSA(key, 0);

    if (rsa == NULL) {
        rc = -1;
    } else {
        rc = RSA_private_decrypt(data_len, enc_data, dec_data, rsa, RSA_PKCS1_PADDING);
    }

    return rc;
}


int openssl_aes_encryption(unsigned char* plaintext, int plaintext_len, unsigned char* key,
                        unsigned char* iv, unsigned char* ciphertext) {


 
    struct timeval start, end;
    gettimeofday(&start, NULL);
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    /* create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        printf("ERROR 1: %ld \n", ERR_get_error());
        return -1;
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv)) {
        printf("ERROR 2: %ld \n", ERR_get_error());
    }

    EVP_CIPHER_CTX_set_padding(ctx, 0);

    /* encrypt message */
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
        printf("ERROR 3: %ld \n", ERR_get_error());
        return -1;
    }

    ciphertext_len = len;

    /* finalise the encryption. Further ciphertext bytes may be written at this stage */
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
        printf("ERROR 4: %ld \n", ERR_get_error());
        return -1;
    }

    ciphertext_len += len;

    /* clean up */
    EVP_CIPHER_CTX_free(ctx);
   
    gettimeofday(&end, NULL);
     #ifdef TIME_DEBUG
      printf("\t%d\t", (end.tv_usec - start.tv_usec));
     #endif
    return ciphertext_len;
}


int openssl_aes_decryption(unsigned char* cipher, int cipher_len, unsigned char* key,
                      unsigned char* iv, unsigned char* plaintext) {
    
    
      struct timeval start, end;
    gettimeofday(&start, NULL);
    
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    /* create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        printf("ERROR 1: %ld \n", ERR_get_error());
        return -1;
    }

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv)) {
        printf("ERROR 2: %ld \n", ERR_get_error());
        return -1;
    }

    EVP_CIPHER_CTX_set_padding(ctx, 0);

    /* decrypt message */
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, cipher, cipher_len)) {
        printf("ERROR 3: %ld \n", ERR_get_error());
        return -1;
    }
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        printf("ERROR 4: %ld \n", ERR_get_error());
        return -1;
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    gettimeofday(&end, NULL);
     #ifdef TIME_DEBUG
      printf("\t%d\t", (end.tv_usec - start.tv_usec));
     #endif
    return plaintext_len;
}

/*

*/

int openssl_rsa_signature_verify(const unsigned char* m, unsigned int m_length,
                    unsigned char* sigbuf, unsigned int siglen, unsigned char* pub_key) {

    int rc = 0;
    
      struct timeval start, end;
   gettimeofday(&start, NULL);
    RSA* rsa = openssl_createRSA(pub_key, 1);
    EVP_PKEY* pubkey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pubkey, rsa);
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();

    /* Initialize `key` with a public key */
    if (1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, pubkey)) {
        printf("Error1: %ld \n", ERR_get_error());
        return -1;
    }

    if (1 != EVP_DigestVerifyUpdate(mdctx, m, m_length)) {
        printf("Error2: %ld \n", ERR_get_error());
        return -1;
    }

    if (1 != EVP_DigestVerifyFinal(mdctx, sigbuf, siglen)) {
        printf("Error3: \n");
        return -1;
    } else {
        rc  = 0;
    }

    EVP_MD_CTX_free(mdctx);
    gettimeofday(&end, NULL);
     #ifdef TIME_DEBUG
      printf("\t%d\t", (end.tv_usec - start.tv_usec));
     #endif
    return rc;
}

/*
 * Sign message m usign key k 
 */
int openssl_rsa_sign(const unsigned char * m , unsigned int len, unsigned char * sign, unsigned int siglen,  unsigned char * prikey)
{
    
      struct timeval start, end;
     gettimeofday(&start, NULL);
    
    RSA* priv_rsa = openssl_createRSA(prikey, 0);
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
	int     ret;

	SHA256(m, len, hash);

	/* Sign */
	ret = RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH, sign,
				   &siglen, priv_rsa);
   
	#ifdef DBUG
	printf("Signature length = %d\n", siglen);
	printf("RSA_sign: %s\n", (ret == 1) ? "OK" : "NG");
    #endif
    gettimeofday(&end, NULL);
     #ifdef TIME_DEBUG
      printf("\t%d\t", (end.tv_usec - start.tv_usec));
     #endif
	return ret;
}

/*
 * Sign message m usign key k 
 */
int openssl_rsa_verfiy(const unsigned char * m , unsigned int len, unsigned char * sign, unsigned int siglen,  unsigned char * pubky)
{
    
      struct timeval start, end;
     gettimeofday(&start, NULL);
    RSA* pk_rsa = openssl_createRSA(pubky, 1);
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
	int     ret;

	SHA256(m, len, hash);
    
    /* Verify */
	ret = RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH, sign,
					 siglen, pk_rsa);
	#ifdef DBUG
    printf("RSA_Verify: %s\n", (ret == 1) ? "true" : "false");
    #endif
    gettimeofday(&end, NULL);
     #ifdef TIME_DEBUG
      printf("\t%d\t", (end.tv_usec - start.tv_usec));
     #endif
	return ret;
}

/*
 * Hash 
 */ 
 
 void openssl_hash(unsigned char  * m, int len, unsigned char * hash)
 {
        SHA256_CTX ctx;

    SHA256_Init(&ctx);
    SHA256_Update(&ctx, m, len);
    SHA256_Final(hash, &ctx);
     //SHA256(m, len, hash);
 }
     


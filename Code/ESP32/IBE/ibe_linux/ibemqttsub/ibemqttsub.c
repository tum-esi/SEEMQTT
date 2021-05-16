/*
 * Boneh & Franklin encryption scheme as presented in the article "Id-based encryption from the Weil pairing"
 *
 * @download: sudo apt install libgmp3-dev
 *            sudo apt install libssl-dev
 *            pbc library: https://crypto.stanford.edu/pbc/
 *
 * @compile: gcc ibemqtt.c -o bfibe -lpbc -lgmp -lssl -lcrypto -Wl,-rpath=/usr/local/lib
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<assert.h> 
#include<fcntl.h>
#include<math.h>   
#include<string.h>
#include<openssl/sha.h>
#include<openssl/err.h>
#include<openssl/ssl.h>
#include<pbc/pbc.h>
#include<pbc/pbc_test.h>

void PrintHEX(unsigned char* str, int len) {

    for (int i = 0; i < len; ++i) {
        printf("%.2X ", str[i]);
    }

    printf("\n");
}

int main(int argc, char **argv) {

    /*
     * P,Q,R -> G1, with Q = aP, R = bP and the set D = {abP, rP} with r random be given,
     *
     * 1. Setup:
     *  corresponding pairing e,
     *  a random private key Km = s,
     *  a public key Kpub = sP,
     *  public hash functions H1 and H2.
     *
     * 2. Extract:
     *  the PKG computes the public key Qid = H1(ID) and
     *  the private key Did = sQid which is given to the user
     *
     * 3. Encrypt:
     *  Qid = H1(ID) -> G1*,
     *  choose random r -> Zp*,
     *  compute gid = e(Qid, Kpub), note that Kpub is PKG's public key
     *  set c = <rP, m XOR H2(gid^r)>
     *
     * 4. Decrypt:
     *  Given c = <u, v>, m = v XOR H2(e(Did, u))
     */

	pairing_t pairing;
	element_t P, Ppub, Did, Qid, U, s, r, xt, gid;
	int fd=0,i=0;
	char mv[80]={0}, c[80]={0}, id[20]="157.157.157.157", hash[20]={0}, err[80]={0}, *gs=NULL;

    char m[80]="0123456789012345678901234567890123456789012345678901234567890123456789012345678";

	/***
	errors strings initialization for SHA1 & clock initialization for times computation
	***/
	ERR_load_crypto_strings();
	SSL_load_error_strings();

	/***initialize the message which is going to be signed***/
	//fd = open("/dev/urandom",O_RDONLY);
	//read(fd, m, 80); 
	//close(fd);

	/***
	pairing function initalization from the input file which contains the pairing parameters
	***/
	pbc_demo_pairing_init(pairing, argc, argv);
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
  	element_mul_zn(Ppub, P, s);
	element_printf("++s: %B\n",s);
	element_printf("++P:  %B\n", P);
	element_printf("++Ppub: %B\n", Ppub);

	/***
	key generation
	***/
	if(SHA1(id, 20, (unsigned char *)hash) == NULL) {
			ERR_error_string(ERR_get_error(),err);
			printf("%s\n",err);			
    }

	element_from_hash(Qid, hash, strlen(hash));
	element_mul_zn(Did, Qid, s);
	element_printf("++Qid: %B\n", Qid);
	element_printf("++Did: %B\n", Did);
    printf("Did length: %d\n", element_length_in_bytes(Did));

	/***
	encryption
	***/	
	element_random(r);
	element_mul_zn(U, P, r);
	element_pairing(gid, Qid, Ppub);
	element_pow_zn(gid, gid, r);
	gs=malloc(element_length_in_bytes(gid));
	element_to_bytes(gs,gid);

	if(SHA1(gs , 20, (unsigned char *)hash)==NULL) {
		ERR_error_string(ERR_get_error(),err);
		printf("%s\n",err);			
	}

	for(i=0;i<20;i++) {
	    c[i]=m[i]^hash[i];
	    c[i+20]=m[i+20]^hash[i];
	    c[i+40]=m[i+40]^hash[i];
	    c[i+60]=m[i+60]^hash[i];
	}

	free(gs);
	
	element_printf("++r: %B\n",r);
	element_printf("++U: %B\n",U);
	element_printf("++gid: %B\n",gid);	
	
	/***
	decryption
	***/
	element_pairing(xt, Did, U);
	gs=malloc(element_length_in_bytes(xt));
	element_to_bytes(gs,xt);

	if (SHA1(gs , 20, (unsigned char *)hash)==NULL) {
	    ERR_error_string(ERR_get_error(),err);
		printf("%s\n",err);			
	}

	for (i=0;i<20;i++) {
	    mv[i]=c[i]^hash[i];
		mv[i+20]=c[i+20]^hash[i];
		mv[i+40]=c[i+40]^hash[i];
		mv[i+60]=c[i+60]^hash[i];
	}
	
	element_printf("e(Did,U):%B\n",xt);
	for (i=0;i<80;i++) {
		printf(":%d %d %d\n",i,m[i],mv[i]);
	}
	
	printf("\n%d\n",strncmp(m,mv,80));

    printf("message in plaintextP: \n");
    PrintHEX(m, 80);

    printf("decrypted message: \n");
    PrintHEX(mv, 80);

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
	free(gs);
	ERR_free_strings();

    return 0;
}

struct ibe_sys_param_public {
    pairing_t pairing;
    element_t Kpub;
    element_t P;
} ibe_sys_param_public_t;

struct ibe_sys_param_private {
    element_t Km;
} ibe_sys_param_private_t;


/**
 * \brief                This function will be called once by PKG to setup system parameters.
 * 
 * \param sys_param_pub  A struct contains system parameters, which will be made public.
 *                       Parameters: pairing, PKG public key Kpub and random P.
 * \param_sys_param_pri  A struct contains system parameters, which should be kept secret.
 *                       Parameters: Km (PKG private key)
 */
void ibe_setup(ibe_sys_param_public_t *sys_param_pub, ibe_sys_param_private_t *sys_param_pri) {

    pairing_init_set_buf(sys_param_pub->pairing, TYPEA_PARAMS, strlen(TYPEA_PARAMS));
    if (!pairing_is_symmetric(sys_param_pub->pairing)) pbc_die("pairing must be symmetric");

    /* initialize G1 elements */
    element_init_G1(sys_param_pub->P, pairing);
    element_init_G1(sys_param_pub->Kpub, pairing);
    
    /* initialize Zr elements */
    element_init_Zr(sys_param_pri->Km, pairing);
    
    element_random(sys_param_pub->P);
    element_random(sys_param_pri->Km);
    element_mul_zn(sys_param_pub->Kpub, sys_param_pub->P, sys_param_pri->Km);
}


/**
 * \brief                   This function will be called by PKG, when a user requests the private key
 *                          of corresponding ID.
 * 
 * \param sys_param_pub     A struct contains IBE public system parameters given by PKG
 * \param Km master         key of PKG (should be kept secret)
 * \param id                ID given by user
 * \param key_id            Extracted private key of given ID, which will be sent to user.
 */
void ibe_extract(ibe_sys_param_public_t *sys_param_pub, ibe_sys_param_private_t *sys_param_pri, unsigned char *id, element_t Did) {

    int hash_ret;
    unsigned char hash[20];
    element_t Qid, Did;

    element_init_G1(Qid, sys_param->pairing);
    element_init_G1(Did, sys_param->pairing);

    hash_ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), id, 20, hash);
    if(hash_ret != 0) {
        Serial.println("failed to hash ID!");
    }

    element_from_hash(Qid, hash, strlen(hash));
    element_mul_zn(Did, Qid, sys_param_pri->Km);
    Serial.print("Did length: ");
    Serial.println(element_length_in_bytes(Did)); // 128 Bytes
}


/**
 * \brief                   This function will be called by user to encrypt message using receiver ID
 *
 * \param sys_param_pub     Public system parameters given by PKG
 * \param rec_id            Receiver ID, which is used as key to encrypt the message
 * \param plaintext         Message in plaintext
 * \param cipher            Encrypted message
 */


void ibe_encryption(ibe_sys_param_public_t *sys_param_pub, unsigned char* rec_id, unsigned char *plaintext, \
                    size_t plaintext_len , unsigned char *cipher) {

    int hash_ret;
    element_t r, U, gid, Qid;

    /* Initialize parameters */
    element_init_G1(Qid, sys_param_pub->pairing);
    element_init_G1(U, sys_param_pub->pairing);
    element_init_GT(gid, sys_param_pub->pairing);
    element_init_Zr(r, sys_param_pub->pairing);

    element_random(r);

    /* Hash the receiver ID */
    hash_ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), rec_id, 20, hash);
    if(hash_ret != 0) {
        Serial.println("failed to hash ID!");
    }

    element_from_hash(Qid, hash, strlen(hash));
    
    element_mul_zn(U, sys_param_pub->P, r);
    element_pairing(gid, Qid, sys_param_pub->Kpub);
    element_pow_zn(gid, gid, r);
    gs = (unsigned char *)malloc(element_length_in_bytes(gid));
    element_to_bytes(gs, gid);

    hash_ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), (unsigned char *)gs, 20, (unsigned char *)hash);
    if (hash_ret != 0) {
        Serial.println("failed to hash gs!");
    }

    j = 0;
    for (i = 0; i < MSG_SIZE; i++) {
        if (j >= 20) {
            j = 0;
        }
        c[i] = m[i] ^ hash[j];
        j++;
    }

    free(gs);
    element_clear(r);
    element_clear(U);
    element_clear(gid);
    element_clear(Qid);

}

// after start the program, ibe should first communicate with PKG to get its own private key Did
// U is from c space

/**
 * \brief                   This function will be called by the receiver, which receives
 *                          encrypted message using his ID
 *
 * \param sys_param_pub     Public system parameters given by PKG
 * \param ciphertext        Encrypted message
 * \param ciphertext_len    Length of encrypted message
 * \param U                 Parameter given by sender, which will be used do the pairing with Did
 * \param Did               Private key of corresponding ID. Given by PKG
 * \param plaintext         Decrypted message
 */
void ibe_decryption(ibe_sys_param_public_t *sys_param_pub, unsigned char *ciphertext, size_t ciphertext_len \
                         element_t U, element_t Did, unsigned char *plaintext) {

    element_t xt, U;

    /* Initialize the GT element */
    element_init_GT(xt);

    element_pairing(xt, Did, U);
    gs = (unsigned char *)malloc(element_length_in_bytes(xt));
    element_to_bytes(gs, xt);

    hash_ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), (unsigned char *)gs, 20, (unsigned char *)hash);
    if(hash_ret != 0) {
        Serial.println("failed to hash gs in decryption");
    }

    j = 0;
    for (i = 0; i < MSG_SIZE; i++) {
        if (j >= 20) {
            j = 0;
        }
        mv[i] = c[i] ^ hash[j];
        j++;
    }
}

void PrintHEX(unsigned char* str, int len) {

    for (int i = 0; i < len; ++i) {
        printf("%.2X ", str[i]);
    }

    printf("\n");
}





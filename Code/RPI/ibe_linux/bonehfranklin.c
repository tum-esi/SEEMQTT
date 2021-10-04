/*
 * Boneh & Franklin encryption scheme as presented in the article "Id-based encryption from the Weil pairing"
 *
 * @download: sudo apt install libgmp3-dev
 *            sudo apt install libssl-dev
 *            pbc library: https://crypto.stanford.edu/pbc/
 *
 * @compile: gcc bonehfranklin.c -o bfibe -lpbc -lgmp -lssl -lcrypto -Wl,-rpath=/usr/local/lib
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
    printf("starting ibe program ... \n");

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

    printf("finished initialization!\n");
	
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
	
	printf("\nComparison result: %d\n",strncmp(m,mv,80));

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

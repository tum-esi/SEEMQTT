
#include "bssl.h"




  /*
 * Encrypt message pt (plain text) using the defined key and store the encrypted inplace
 */
 int  aes_encryption(uint8_t * pt, uint8_t  * k , int len, uint8_t * iv)
{
  
    
      struct timeval start, end;

      br_aes_big_cbcenc_keys    ctx ;
      #ifdef CRY_DEBUG_AES
      printf(" <<<<<<<<<<<<<<<start AES Encryption>>>>>>>>>>>>>>>>>\n");
      printf(" Plain Text for AES \n");
      
 
      if (len >32)
      Print(pt,len,len-32);
      else
      Print(pt,len,0);
      #endif 
      br_aes_big_cbcenc_init(&ctx, k,16 );

      gettimeofday(&start, NULL);

      br_aes_big_cbcenc_run(&ctx,iv, pt, 16);
 

 
     gettimeofday(&end, NULL);
     #ifdef TIME_DEBUG
      //printf("Time to Encrypt the Data using AES is: %d us \n", (end.tv_usec - start.tv_usec));
      printf("\t%d\t", (end.tv_usec - start.tv_usec));
     #endif

     #ifdef CRY_DEBUG_AES
      printf(" Encryptted text by AES: \n");
       if (len >32)
      Print(pt,len,len-32);
      else
      Print(pt,len,0);
      printf("<<<<<<<<<<<<<<<End AES Encryption>>>>>>>>>>>>>>>>>\n");
      #endif
        printf(" aes_encryption << \n"); 
      return len;
  }


int  aes_decryption(uint8_t * cipher, uint8_t * key , int len, uint8_t  * iv )
{
  
  br_aes_big_cbcdec_keys    ctx ;
  br_aes_big_cbcdec_init(&ctx, key,BLOCK_SIZE );
  br_aes_big_cbcdec_run(&ctx,iv,  cipher, len);

  printf(" Plain text: \n");
  Print(cipher,len,0);
  printf("<<<<<<<<<<<<<<<End Decryption>>>>>>>>>>>>>>>>> \n");
  
   return len; 
  }



/*
RSA
*/


static const unsigned char SHA1_OID[] = {
  0x05, 0x2B, 0x0E, 0x03, 0x02, 0x1A
};



/*
 *  function for RSA encryption
 */
typedef struct {
  const br_prng_class *vtable;
  unsigned char buf[128];
  size_t ptr, len;
} rng_fake_ctx;

 void rng_fake_init(rng_fake_ctx *cc, const void *params, const void *seed, size_t len);
 void rng_fake_generate(rng_fake_ctx *cc, void *dst, size_t len);
 void rng_fake_update(rng_fake_ctx *cc, const void *src, size_t len);

 const br_prng_class rng_fake_vtable = {
  sizeof(rng_fake_ctx),
  (void (*)(const br_prng_class **,
    const void *, const void *, size_t))&rng_fake_init,
  (void (*)(const br_prng_class **,
    void *, size_t))&rng_fake_generate,
  (void (*)(const br_prng_class **,
    const void *, size_t))&rng_fake_update
};

 void rng_fake_init(rng_fake_ctx *cc, const void *params, const void *seed, size_t len)
{
  (void)params;
  if (len > sizeof cc->buf) {
    fprintf(stderr,"seed is too large ( bytes)\n");
    //Serial.println ((unsigned long)len);
    exit(EXIT_FAILURE);
  }
  cc->vtable = &rng_fake_vtable;
  memcpy(cc->buf, seed, len);
  cc->ptr = 0;
  cc->len = len;
}

 void rng_fake_generate(rng_fake_ctx *cc, void *dst, size_t len)
{
  if (len > (cc->len - cc->ptr)) {
  	fprintf(stderr, "asking for more data than expected\n");
    	exit(EXIT_FAILURE);
  }
  memcpy(dst, cc->buf + cc->ptr, len);
  cc->ptr += len;
}

 void rng_fake_update(rng_fake_ctx *cc, const void *src, size_t len)
{

}
int
check_equals(const char *banner, const void *v1, const void *v2, size_t len)
{

  unsigned char *b;

  if (memcmp(v1, v2, len) == 0) {
    return 0;
  }
  else
  return -1;
}

 size_t hextobin(unsigned char *dst, const char *src)
{
  size_t num;
  unsigned acc;
  int z;

  num = 0;
  z = 0;
  acc = 0;
  while (*src != 0) {
    int c = *src ++;
    if (c >= '0' && c <= '9') {
      c -= '0';
    } else if (c >= 'A' && c <= 'F') {
      c -= ('A' - 10);
    } else if (c >= 'a' && c <= 'f') {
      c -= ('a' - 10);
    } else {
      continue;
    }
    if (z) {
      *dst ++ = (acc << 4) + c;
      num ++;
    } else {
      acc = c;
    }
    z = !z;
  }
  return num;
}

/*
 * RSA decrypt 
 * modula 1024, 2048
 */ 
int rsa_decrypt(unsigned char * cipher, int modula, char * parr , char * qarr , char * dparr, char * dqarr, char * iqarr)
{
       br_rsa_oaep_decrypt mdec = br_rsa_i15_oaep_decrypt;
	//printf("<<<<<<<<<<<<<Start RSA Decryption>>>>>>>>>>>>>>>>>>\n");

	char * seedar  = "AAFD12F659CAE63489B479E5076DDEC2F06CB58F" ;

	unsigned char  seed[128];


	unsigned char p[256];
	unsigned char q[256];
	unsigned char dp[256];
	unsigned char dq[256];
	unsigned char iq[256];
	br_rsa_private_key sk;

	sk.n_bitlen = modula;
	sk.p = p;
	sk.plen = hextobin(p, parr);
	sk.q = q;
	sk.qlen = hextobin(q, qarr);
	sk.dp = dp;
	sk.dplen = hextobin(dp, dparr);
	sk.dq = dq;
	sk.dqlen = hextobin(dq, dqarr);
	sk.iq = iq;
	sk.iqlen = hextobin(iq, iqarr);

	
	size_t len = 128;
	if (mdec(&br_sha1_vtable, NULL, 0,  &sk, cipher, &len) != 1)
	  {
	    printf("decryption failed");
	    return  -1;
	  }
	
        return len; 
     
     //printf("<<<<<<<<<<<<<END RSA DEcryption >>>>>>>>>>>>>>>>>>\n");
  }

/*
 * RSA Encryption
 */

int  rsa_encrypt(unsigned char * plaintext, int plain_len, unsigned char * output, unsigned char * arrn,int ln,  unsigned char * expo, int le)
  {
       br_rsa_oaep_encrypt  menc= br_rsa_i15_oaep_encrypt;
	struct timeval start, end;
	char * seedar  = "AAFD12F659CAE63489B479E5076DDEC2F06CB58F" ;
	unsigned char  seed[128];
	size_t seed_len ;
	
        #ifdef CRY_DEBUG
	   printf("<<<<<<<<<<<<<Start RSA Encryption>>>>>>>>>>>>>>>>>>\n");
        #endif
	
	unsigned char n[512];
	unsigned char e[8];

	br_rsa_public_key pk;

	pk.n = n;
	pk.nlen = hextobin(n, arrn);
	pk.e = e;
	pk.elen = hextobin(e, expo);
	
	rng_fake_ctx rng;

	unsigned char tmp[513];
	size_t len;

	//#ifdef CRY_DEBUG
	 printf("Plain Key: ");
	 Print( plaintext, plain_len, 0 );
	//#endif

	 seed_len = hextobin(seed, seedar);

	 rng_fake_init(&rng, NULL, seed, seed_len);


	gettimeofday(&start, NULL);
	len = menc(&rng.vtable, &br_sha1_vtable, NULL, 0, &pk, tmp, sizeof tmp, plaintext, plain_len);
	gettimeofday(&end, NULL);



	 if (len == 0 )
	 {
	    #ifdef CRY_DEBUG
	    printf("Encryption failed ! \n");
	    #endif
	    return -1;
	 }

	#ifdef CRY_DEBUG
	printf("Encryption succeeded ! \n ");
	printf ("Encryptted key: \n");
	Print(tmp, len, 0);
	#endif


	for (int i = 0 ; i< (int)len; i++)
	    output[i] = tmp[i];

	#ifdef TIME_DEBUG
	    //printf (" Time to Encrypt the  session key using RSA: %d us \n", (end.tv_usec - start.tv_usec));
	    printf("%d\t",(end.tv_usec - start.tv_usec));
	#endif
	#ifdef CRY_DEBUG
	printf("<<<<<<<<<<<<<END RSA Encryption>>>>>>>>>>>>>>>>>>\n");
	#endif

	return len;
  }


  /*
 * RSA SIGNING
 * signedhash (128)
 */
void rsa_sign( char * msg ,  unsigned char * signedhash,  int modula, char * parr , char * qarr , char * dparr, char * dqarr, char * iqarr)
    {
      br_rsa_pkcs1_sign fsign = br_rsa_i15_pkcs1_sign;
      struct timeval start, end;
      #ifdef CRY_DEBUG_RSA_SIGN
       printf("<<<<<<<<<<<<<Start Sign>>>>>>>>>>>>>>>>>>>\n");
      #endif
      
      unsigned char p[256];
      unsigned char q[256];
      unsigned char dp[256];
      unsigned char dq[256];
      unsigned char iq[256];
      br_rsa_private_key sk;

      sk.n_bitlen = modula;
      sk.p = p;
      sk.plen = hextobin(p, parr);
      sk.q = q;
      sk.qlen = hextobin(q, qarr);
      sk.dp = dp;
      sk.dplen = hextobin(dp, dparr);
      sk.dq = dq;
      sk.dqlen = hextobin(dq, dqarr);
      sk.iq = iq;
      sk.iqlen = hextobin(iq, iqarr);
      
      
      unsigned char hv[20];
      br_sha1_context hc;
      size_t u;

      br_sha1_init(&hc);
      br_sha1_update(&hc, msg, strlen(msg));
      br_sha1_out(&hc, hv);

      #ifdef CRY_DEBUG_RSA_SIGN
      printf("Hash Value :");
      Print(hv, 20, 0 );
      #endif
      gettimeofday(&start, NULL);
      if (!fsign(SHA1_OID, hv, 20, &sk, signedhash)) {
	printf("Signature generation failed\n");
      }
     gettimeofday(&end, NULL);

      #ifdef TIME_DEBUG
	//printf(" Time to sign: %d us \n", (end.tv_usec - start.tv_usec));
	 printf("\t%d\t",(end.tv_usec - start.tv_usec));
      #endif

      #ifdef CRY_DEBUG_RSA_SIGN
      printf(" signature :");
      Print(signedhash, 128, 0);
      printf("<<<<<<<<<<<<<End Sign>>>>>>>>>>>>>>>>>>\n");
     #endif
  }


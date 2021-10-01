#include <stdio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

//#include "assert.h"

/*
 * Print the specified number of spaces.
 */
void print_space(FILE *fp, int n)
{
    while (n--)
      fprintf(fp, " ");
}



void print_key(FILE *fp, char *algname ,  char *key)
{
	
    int length  = 50 ;
    int i, k;
    int start = 12; 

    print_space(fp, start);
    fprintf(fp, "\"%s", algname);

    for (i = 0, k = strlen(algname) + 2; i < strlen(key); i++, k++)
    {
	if (k == length)
	{
	    if (i == strlen(key))
	    {
		fprintf(fp, "\"\n");
		return;
	    }

	    fprintf(fp, "\\\n");
	    print_space(fp, start);
	    i--;
	    k = 0;
	}
	else
	  fprintf(fp, "%c", key[i]);
    }

    fprintf(fp, "\"\n");
}


char *  encodesign( char *algname ,  char *key)
{

    int  index = 0 ;

    char * result = malloc (364);  
    memset(result, '\0',364);
	
    int length  = 50 ;
    int i, k;
    int start = 12; 

    for (int j = 0 ; j< start; j++)
	result[index +j]= ' '; 
    
    index += start;
    // add "\""; 
    result[index++] = '\"'; 
    int alglen= strlen(algname); 
    
   strncpy(result+index,algname, alglen);  
   index += alglen;

    for (i = 0, k = strlen(algname) + 2; i < strlen(key); i++, k++)
    {
	if (k == length)
	{
	    if (i == strlen(key))
	    {
                char * t = "\"\n"; 
                strncpy (result+index,t, strlen(t)); 
                index +=strlen(t); 
		return  result;
	    }
	     char * tt = "\\\n";
	     strncpy(result+index,tt, strlen(tt)); 
             index +=strlen(tt);
	      for (int j = 0 ; j< start; j++)
		result[index +j]= ' '; 
    
   	    index += start;
	    i--;
	    k = 0;
	}
	else
	{
	  result[index++] = key[i] ; 
	}

    }
   
    char * l = "\"\n" ; 
    strncpy (result+index , l , strlen(l)); 
    return result; 
}

/*
 * ASCII-encode a key.
 * keytype  1 for public , 2 private 
 * encoding 1 for Hex , 2 for BASE64
 */
char * kn_encode_key(RSA *rsa, int encoding, int keytype)
{

   	 char *foo, *ptr;
    	int i;

    	char *s;

         
	if (keytype == 1)
	  i = i2d_RSAPublicKey(rsa, NULL);
	else
	  i = i2d_RSAPrivateKey(rsa, NULL);

	if (i <= 0)
	{
	    printf(" Error: i2d_RSAPublicKey \n"); 
	    return (char *) NULL;
	}
        
	ptr = foo = (char *) calloc(i, sizeof(char));
	if (foo == (char *) NULL)
	{
	    printf(" Error: calloc foo \n"); 
	    return (char *) NULL;
	}
        
	if (keytype == 1)
	  i = i2d_RSAPublicKey(rsa, (unsigned char **) &foo);
	else
	  i = i2d_RSAPrivateKey(rsa, (unsigned char **) &foo);
        if (i <= 0)
	{
	    printf(" Error: i2d_RSAPublicKey \n"); 
	    return (char *) NULL;
	}
        
       
	if (encoding == 1)
	{
	    if (kn_encode_hex(ptr, &s, i) != 0)
	    {
		printf(" Error: kn_encode_hex\n"); 
		free(ptr);
		return (char *) NULL;
	    }

	    free(ptr);	

	printf("  end kn_encode_key \n"); 
	    return s;
	}
	else
	  if (encoding == 2)
	  {
	      s = (char *) calloc(2 * i, sizeof(char));
	      if (s == (char *) NULL)
	      {
		  printf(" Error: calloc BASE64\n"); 
		  free(ptr);
		  return (char *) NULL;
	      }
		/*

	      if (kn_encode_base64(ptr, i, s, 2 * i) == -1)
	      {
		  printf(" Error: kn_encode_base64\n"); 
		  free(s);
		  free(ptr);
		  return (char *) NULL;
	      }
	     */

	      free(ptr);
	      return s;
	  }
    
    printf(" Error: end\n"); 
    return (char *) NULL;
}


static const char hextab[] = {
     '0', '1', '2', '3', '4', '5', '6', '7',
     '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
}; 

/*
 * Actual conversion to hex.
 */   
static void
bin2hex(unsigned char *data, unsigned char *buffer, int len)
{

    int off = 0;
     
    while(len > 0) 
    {
	buffer[off++] = hextab[*data >> 4];
	buffer[off++] = hextab[*data & 0xF];
	data++;
	len--;

    }

}



/*
 * Encode a binary string with hex encoding. Return 0 on success.
 */
int
kn_encode_hex(unsigned char *buf, char **dest, int len)
{
    
    if (dest == (char **) NULL)
    {
	printf(" Error: dest  is NULL \n"); 
	return -1;
    }

    *dest = (char *) calloc(2 * len + 1, sizeof(char));
    if (*dest == (char *) NULL)
    {
	printf(" Error: kn_encode_hex: calloc\n"); 
	return -1;
    }

    bin2hex(buf, *dest, len);

    return 0;
}

/*
 * Decode a hex encoding. Return 0 on success. The second argument
 * will be half as large as the first.
 */
int
kn_decode_hex(char *hex, char **dest)
{
    int i, decodedlen;
    char ptr[3];

  
    if (dest == (char **) NULL)
    {
	printf(" Error: dest NULL\n"); 
	return -1;
    }

    if (strlen(hex) % 2)			/* Should be even */
    {
	printf(" Error: hex Should be even\n");
	return -1;
    }

    decodedlen = strlen(hex) / 2;
    *dest = (char *) calloc(decodedlen, sizeof(char));
    if (*dest == (char *) NULL)
    {
	printf(" Error: dest is NULL\n");
	return -1;
    }

    ptr[2] = '\0';
    for (i = 0; i < decodedlen; i++)
    {
	ptr[0] = hex[2 * i];
	ptr[1] = hex[(2 * i) + 1];
      	(*dest)[i] = (unsigned char) strtoul(ptr, (char **) NULL, 16);
    }

    return 0;
}

 /*
  * generate_key: Create public andd private key compatible with KeyNote 
  * pkname: name of public key 
  * prname: name of private key
  * int	bits = 1024, 2048, 
  */ 
int  generate_key( char * pkname,  char * prname, int bits)
{
	int	ret = 0;
	RSA	*r = NULL;
	BIGNUM	*bne = NULL;
	BIO	*bp_public = NULL, *bp_private = NULL;

	
	unsigned long	e = RSA_F4;


       char PEMPK[20]; 
       char PEMPR[20]; 
       sprintf(PEMPK, "%s%s", pkname, ".PEM");  
       sprintf(PEMPR, "%s%s", prname, ".PEM");

	printf(" PEMPK = %s , PEMPK =%s", PEMPK ,PEMPK);

	// 1. generate rsa key
	bne = BN_new();
	ret = BN_set_word(bne,e);
	if(ret != 1){
		goto free_all;
	}

	r = RSA_new();
	ret = RSA_generate_key_ex(r, bits, bne, NULL);
	if(ret != 1){
		goto free_all;
	}
     
	// 2. save public key
	bp_public = BIO_new_file(PEMPK, "w+");
	ret = PEM_write_bio_RSAPublicKey(bp_public, r);
	if(ret != 1){
		goto free_all;
	}

	// 3. save private key
	bp_private = BIO_new_file(PEMPR, "w+");
	ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);

	
	// extra 
         char *fpk  = kn_encode_key (r, 1,1); 

         if (fpk == NULL)
		printf("Error HERE\n"); 
         
         FILE *fppk = fopen(pkname,"w+"); 
         print_key(fppk, "rsa-hex:", fpk);
         fclose(fppk); 
          free (fpk);

 	char *prf  = kn_encode_key (r, 1,2); 

         if (prf == NULL)
		printf("Error HERE\n"); 
         
         FILE *fppr = fopen(prname,"w+"); 
         print_key(fppr,"private-rsa-hex:" ,prf);
         fclose(fppr); 
          free (prf); 

	// 4. free
	free_all:

	    BIO_free_all(bp_public);
	    BIO_free_all(bp_private);
	    RSA_free(r);
	    BN_free(bne);

	return (ret == 1);
}



/*
 * openssl_createRSA: create RSA data type by reading public or private key
 * key: PEM file that contain the key 
 * keytypr: 1 for  public key , 0 for private key
 */ 
RSA* openssl_createRSA(unsigned char* key, int keytype) {

    RSA* rsa = NULL;
    BIO* keybio = NULL;
    keybio = BIO_new_mem_buf(key, -1);

    if (keybio == NULL) {
        printf("Failed to create key BIO \n");
    } else if (keytype) {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
    } else {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    }

    return rsa;
}



/* 
 * printhex: print in HEX
 * input: data to be printed 
 * len: length
 */ 
void printhex(char * input, int len){

for (int i =0;i<len ; i++)
    printf("%x ",input[i]);
printf("\n"); 
    
} 

/* 
 * openssl_rsa_sign: Sign a message using using private key
 * m: the message 
 * prikey: the private key
 */ 
char * openssl_rsa_sign(const unsigned char * m , unsigned int len,  unsigned char * prikey)
{
      char * sigalg  = "sig-rsa-sha1-hex:"; 
      struct timeval start, end;
      SHA_CTX shscontext;
      RSA* priv_rsa ;  
      unsigned char res2[20]; 
      char *finalbuf = (char *) NULL;


      int slen;

    SHA1_Init(&shscontext);
    printf(" as->as_startofsignature = %s\n as->as_allbutsignature - as->as_startofsignature = %d \n", m , len); 
    SHA1_Update(&shscontext,m, len);
    printf("sigalg  = %s , len = %d\n", sigalg, strlen(sigalg));
    SHA1_Update(&shscontext, sigalg, strlen(sigalg));  
    SHA1_Final(res2, &shscontext);
    printf("SHA result:\n" );
    printhex(res2,20);
    
    priv_rsa = openssl_createRSA(prikey, 0);
    
    unsigned char  * sbuf = (unsigned char *) calloc(RSA_size(priv_rsa),sizeof(unsigned char));
    if (sbuf == (unsigned char *) NULL)
    {
	printf(" ERROR_MEMORY\n");
	return (char *) NULL;
    }

    
    if (RSA_sign_ASN1_OCTET_STRING(RSA_PKCS1_PADDING, res2, 20,sbuf, &slen, priv_rsa) <= 0)
          {
              free(sbuf);
              printf(" ERROR_SYNTAX\n");
              return (char *) NULL;
          }
     printf(" slen = %d \n", slen);
     printhex(sbuf, slen); 


     memset(sbuf, '0',slen);
     RSA_sign(NID_sha1, res2, 20, sbuf, &slen, priv_rsa);

     printf(" slen = %d \n", slen);
     printhex(sbuf, slen); 


     int  i = kn_encode_hex(sbuf, (char **) &finalbuf, slen);
     free(sbuf);
     if (i != 0)
       return (char *) NULL;

   /* Concatenate algorithm name and signature value */
    char * rebuf = (char *) calloc(strlen(sigalg) + strlen(finalbuf) + 1, sizeof(char));
    sprintf(rebuf, "%s%s", sigalg, finalbuf);
    free(finalbuf);

	return rebuf;
}


/* 
 * Create a credentil 
 */ 
char* getCredentials(int * len)
{
    FILE *fp;
    long cre_Size;
    char *result;
    fp= fopen("cred","r");
    if (fp == NULL)
    {
        fprintf(stderr, "Credential file is missing ");
        exit(1);
    }
    fseek(fp,0,SEEK_END);
    cre_Size = ftell(fp);
    printf(" Credentials Size %d\n",cre_Size);
    rewind(fp);
    result = malloc(cre_Size+1);
    fread(result,cre_Size,1,fp);
    fclose(fp);
    result[cre_Size]='\0';
    *len =cre_Size;
    return result;
}

char* getpk(int * len)
{
    FILE *fp;
    long pol_Size;
    char *result;
    fp= fopen("iotpr.PEM","r");
    if (fp == NULL)
    {
        fprintf(stderr, "Policy file is missing ");
        exit(1);
    }
    fseek(fp,0,SEEK_END);
    pol_Size = ftell(fp);
    rewind(fp);
    result = malloc(pol_Size+1);
    fread(result,pol_Size,1,fp);
    fclose(fp);
    result[pol_Size]='\0';
    *len =pol_Size;
    return result;
}

int main(int argc, char* argv[])

{    
    generate_key(argv[1], argv[2],atoi(argv[3])); 
    }

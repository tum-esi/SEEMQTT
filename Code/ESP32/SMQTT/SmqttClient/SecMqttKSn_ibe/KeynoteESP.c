//#include <iostream>
#include "mbedtls/pk.h"
#include "mbedtls/rsa.h"
#include "mbedtls/sha1.h"
#include "mbedtls/md5.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/entropy_poll.h"

#include "KeynoteESP.h"

static const char hextab[] = {
     '0', '1', '2', '3', '4', '5', '6', '7',
     '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};
/*
* Actual conversion to hex.
*/
void bin2hex(unsigned char *data, unsigned char *buffer, int len)
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


static int kn_myrand(void *rng_state, unsigned char *output, size_t len)
{
  size_t olen;
  return mbedtls_hardware_poll(rng_state, output, len, &olen);
}

/*
* Encode a binary string with hex encoding. Return 0 on success.
*/
int kn_encode_hex(unsigned char *buf, char **dest, int len)
{

  if (dest == (char **) NULL)
  {
    //.println(" Error: dest  is NULL \n");
    return -1;
  }

  *dest = (char *) calloc(2 * len + 1, sizeof(char));
  if (*dest == (char *) NULL)
  {
    //Serial.println(" Error: kn_encode_hex: calloc\n");
    return -1;
  }

  bin2hex((unsigned char *)buf, (unsigned char *) *dest, len);

  return 0;
}

char * kn_encode_signture( char *algname ,  char *key)
{
  int  index = 0 ;
  char * result = (char *)malloc (364);
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

char *  kn_rsa_sign_md5(const unsigned char * input, int len, const unsigned char * iot_pr_key, int keylen,   unsigned char * sign)
{
  mbedtls_pk_context  pr;
  mbedtls_pk_init(&pr);
  mbedtls_md5_context shscontext;
  int error_code_pr ;
  int hashlen =  16 ;
  unsigned char hashval[hashlen];

  #ifdef DBG_PRINT
  //.println("Input Text");
  Print( (uint8_t *)input, 0 ,P_SIZE);
  #endif

  error_code_pr =  mbedtls_pk_parse_key(&pr, iot_pr_key, keylen,NULL,NULL);
  if( error_code_pr != 0)
  {
    #ifdef DBG_PRINT
    //Serial.println( "READING PRIVATE KEY NOT SUCCESSFULL");
    #endif
    mbedtls_pk_free(&pr);
    return NULL ;
  }
  #ifdef DBG_PRINT
  //Serial.println( "READING PRIVATE KEY SUCCESSFULL");
  #endif

  /* compute the SHA1 hash for the input*/
  mbedtls_md5_init(&shscontext);
  mbedtls_md5_starts(&shscontext);
  mbedtls_md5_update(&shscontext,input, len);
  const unsigned char sigalg[]  = "sig-rsa-md5-hex:";
  int sigalglen = strlen((char *)sigalg);
  mbedtls_md5_update(&shscontext, sigalg, sigalglen);
  mbedtls_md5_finish(&shscontext, hashval);
  #ifdef DBG_PRINT
  //Serial.println("MD5 result:");
  Print(hashval, 0 ,hashlen );
  #endif
  int sign_ret;
  size_t olen= 0;
  sign_ret = mbedtls_pk_sign(&pr, MBEDTLS_MD_MD5, hashval, hashlen, sign, &olen, kn_myrand, NULL);
  if (sign_ret != 0)
  {
    #ifdef DBG_PRINT
    //Serial.println( " Sign not SUCCESSFULL");
    #endif
    mbedtls_pk_free(&pr);
    return NULL;
  }
  else
  {
    #ifdef DBG_PRINT
    //Serial.println( " Sign  SUCCESSFULL");
    //Serial.println( " Sign :");
    #endif
  }
  char * finalbuf = (char *)NULL;
  int  i = kn_encode_hex(sign, (char **) &finalbuf, olen);

  if (i != 0)
  return NULL;

  /* Concatenate algorithm name and signature value */
  char * rebuf = (char *) calloc(sigalglen + strlen(finalbuf) + 1, sizeof(char));
  sprintf(rebuf, "%s%s", sigalg, finalbuf);
  #ifdef DBG_PRINT
  //Serial.printf(rebuf);
  #endif

  return rebuf;
  mbedtls_pk_free(&pr);
}

#include <ArduinoBearSSL.h>


//#include <bearssl_rsa.h>
#define  CRY_DEBUG
char * arrn = "BBF82F090682CE9C2338AC2B9DA871F7368D07EED41043A440D6B6F07454F51FB8DFBAAF035C02AB61EA48CEEB6FCD4876ED520D60E1EC4619719D8A5B8B807FAFB8E0A3DFC737723EE6B4B7D93A2584EE6A649D060953748834B2454598394EE0AAB12D7B61A51F527A9A41F6C1687FE2537298CA2A8F5946F8E5FD091DBDCB";
char * expo = "11";
//char * plaintxt= "D436E99569FD32A7C8A05BBC90D32C49D436E99569FD32A7C8A05BBC90D32C49D436E99569FD32A7C8A05BBC90D32C49" ; 
char * plaintxt= "4436E99569FD" ; 
uint8_t key[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
char * seedar  = "AAFD12F659CAE63489B479E5076DDEC2F06CB58F" ; 

char * parr  = "EECFAE81B1B9B3C908810B10A1B5600199EB9F44AEF4FDA493B81A9E3D84F632124EF0236E5D1E3B7E28FAE7AA040A2D5B252176459D1F397541BA2A58FB6599";
char * qarr  = "C97FB1F027F453F6341233EAAAD1D9353F6C42D08866B1D05A0F2035028B9D869840B41666B42E92EA0DA3B43204B5CFCE3352524D0416A5A441E700AF461503";
char * dparr = "54494CA63EBA0337E4E24023FCD69A5AEB07DDDC0183A4D0AC9B54B051F2B13ED9490975EAB77414FF59C1F7692E9A2E202B38FC910A474174ADC93C1F67C981";
char * dqarr = "471E0290FF0AF0750351B7F878864CA961ADBD3A8A7E991C5C0556A94C3146A7F9803F8F6F8AE342E931FD8AE47A220D1B99A495849807FE39F9245A9836DA3D";
char * iqarr=   "B06C4FDABB6301198D265BDBAE9423B380F271F73453885093077FCD39E2119FC98632154F5883B167A967BF402B4E9E2E0F9656E698EA3666EDFB25798039F7";



unsigned char RSA_N[] = {
    0x00,0xba,0xb8,0x44,0xe5,0xf9,0xbb,0x95,0x8e,0xad,0xc2,0xb9,0x67,0xca,0x8f,
    0xaf,0x4e,0x1c,0xd1,0x10,0xa0,0x5a,0x8c,0xf1,0x92,0xe1,0x10,0x07,0x43,0x3a,
    0xf5,0x3a,0xdf,0xad,0x7a,0xac,0x94,0x9e,0xeb,0x97,0xc4,0xb8,0x0f,0x5c,0xd6,
    0x03,0xe9,0x50,0xfb,0xf4,0x82,0x89,0xbc,0xf6,0x63,0xbd,0x38,0xda,0xf2,0x14,
    0x51,0x1d,0x99,0x7c,0x41,0xbb,0xd3,0x61,0x11,0x8e,0x47,0x56,0x0c,0x80,0x47,
    0x01,0x3c,0xa3,0xab,0x55,0xce,0x68,0xc0,0x4f,0x7c,0xee,0x7b,0x21,0xd0,0xa6,
    0x6d,0x1a,0xa9,0x37,0x93,0x5d,0xbe,0xed,0x64,0x92,0xce,0x35,0x5e,0x1e,0x9f,
    0x79,0x43,0xb8,0xd9,0x59,0xb5,0xd3,0x94,0x31,0xae,0x82,0xe2,0xbf,0x0e,0x0f,
    0x1a,0x45,0x0e,0xda,0x82,0xff,0x08,0x73,0x2f
};
unsigned char RSA_E[] = {
  0x01, 0x00, 0x01
};
  unsigned char RSA_P[] = {
  0x00,0xeb,0xc4,0xcf,0x5a,0xa1,0x72,0xa9,0x50,
  0x6c,0x3e,0xad,0x7c,0xb8,0xab,0x62,0x40,0x9b,
  0x8f,0xb7,0x6a,0x70,0xb1,0x96,0x4d,0x4e,0x7f,
  0x7a,0xa7,0x20,0x4c,0x99,0x55,0x1b,0xa9,0x8f,0x62,0x0e,0x7d,0x0a,0xae,
  0xf2,0x97,0xcf,0xb7,0x34,0xfc,0xf6,0x75,0x11,0x52,0x2d,0xfd,0x99,0x97,0xf4,0x80,0x8b,0x4a,0xab,0x20,0x75,0xa5,0xcc,0x59 
};
  unsigned char RSA_Q[] = {
    0x00,0xca,0xbd,0xfc,0xf9,0x58,0xe9,0xd7,0xe2,0x02,0x10,0xc2,0xc5,0x89,0x8e,
    0x32,0xde,0x96,0x90,0xf8,0xcc,0x5b,0x96,0x52,0x1b,0x8b,0x45,0xda,0x36,0xcc,
    0xae,0x93,0x21,0xcd,0x61,0x7b,0x84,0x65,0x5e,0x43,0x06,0x05,0x87,0x00,0x1b,
    0x99,0x21,0x99,0x11,0xf7,0xef,0xba,0xfe,0xf7,0x73,0x08,0xb0,0xdb,0x5e,0xa6,
    0x5b,0xa0,0xa0,0x2a,0xc7
};
 unsigned char RSA_DP[] = {0x00,0x96,0xd6,0xd6,0xb6,0xac,0x3e,0x18,0x97,0x0c,0x79,
 0xd9,0x32,0xc7,0x4f,0x56,0xcf,0xf0,0xf6,0xb4,0xa9,0x13,0xaa,0x7a,0x77,0xc1,0x36,
 0x0b,0xf9,0xaa,0xcf,0x76,0xf6,0x1d,0x79,0x90,0xf3,0x72,0x43,0x8b,0xb7,0x3c,0x1b,
 0xe6,0x57,0xd7,0x3d,0xd4,0x4c,0x37,0xa5,0xb5,0x28,0xdf,0xc0,0x79,0x3c,0x58,0x61,
 0x90,0x34,0x77,0x30,0x79,0x19
};
 unsigned char RSA_DQ[] = {
  0x5d,0x3e,0xf0,0x60,0xc6,0x52,0x16,0xe6,0x6d,0xc8,0xc3,
  0xac,0x12,0xce,0x01,0xfb,0x7a,0x08,0x87,0x45,0x86,0x42,
  0x26,0xd5,0x60,0x42,0xea,0x50,0x15,0xe7,0x9c,0xe8,0x09,
  0x0b,0xf2,0xc1,0x3b,0xd6,0x14,0xc1,0x6a,0x3d,0x63,0x01,
  0x4e,0x76,0xb3,0x6e,0x1e,0x38,0x76,0x2a,0xda,0x28,0x60,
  0x30,0xe7,0x54,0x2a,0xf3,0x2d,0x77,0x3c,0xf5
};
 unsigned char RSA_IQ[] = {
  0x00,0xc5,0xd7,0x18,0x69,0x9b,0xb9,0x48,0xa6,0xde,0x5c,0x19,0xc6,0xb0,0xc6,
  0xf3,0x0f,0xb7,0x53,0xdd,0xc1,0x09,0x08,0xf1,0x77,0xce,0xa4,0x63,0xdb,0x63,
  0x7d,0xe5,0x14,0xc2,0x47,0x56,0x8c,0xe8,0x39,0xe9,0xb1,0x10,0x6d,0x76,0x57,
  0xe8,0x9f,0x57,0xf1,0xa8,0x77,0xcb,0xc1,0xee,0x29,0xd3,0x10,0x23,0xf3,0xd9,
  0x44,0x79,0xd9,0xe2,0x78
};

static const br_rsa_public_key RSA_PK = {
  RSA_N, sizeof RSA_N,
  RSA_E, sizeof RSA_E
};

static const br_rsa_private_key RSA_SK = {
  1024,
  RSA_P, sizeof RSA_P,
  RSA_Q, sizeof RSA_Q,
  RSA_DP, sizeof RSA_DP,
  RSA_DQ, sizeof RSA_DQ,
  RSA_IQ, sizeof RSA_IQ
};


  
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
    Serial.print("seed is too large ( bytes)\n");
    Serial.println ((unsigned long)len);
    //exit(EXIT_FAILURE);
  }
  cc->vtable = &rng_fake_vtable;
  memcpy(cc->buf, seed, len);
  cc->ptr = 0;
  cc->len = len;
}

 void rng_fake_generate(rng_fake_ctx *cc, void *dst, size_t len)
{
  if (len > (cc->len - cc->ptr)) {
   // fprintf(stderr, "asking for more data than expected\n");
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
  /*
  Serial.print( "v1: ");
  for (int u = 0, b = ( unsigned char *)v1; u < len; u ++) {
    Serial.print(b[u], HEX);
    Serial.print(" ");
  }
  Serial.println(); 
  Serial.print("v2: ");
  for (int u = 0, b = ( unsigned char *)v2; u < len; u ++) {
     Serial.print(b[u], HEX);
    Serial.print(" ");
  }
  Serial.println(); 
  return 1; 

   */
}

void hw_wdt_disable(){
  *((volatile uint32_t*) 0x60000900) &= ~(1); // Hardware WDT OFF
}

void hw_wdt_enable(){
  *((volatile uint32_t*) 0x60000900) |= 1; // Hardware WDT ON
}

void setup() {
   Serial.begin(115200);
  while (!Serial) {
    ; //wait
  }
  //ESP.wdtDisable();
 // hw_wdt_disable();
  
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


unsigned char tmp[128];
void rsa_decrypt( br_rsa_oaep_decrypt mdec)
{

  Serial.println("<<<<<<<<<<<<<Start dec>>>>>>>>>>>>>>>>>>");
    unsigned char p[256];
    unsigned char q[256];
    unsigned char dp[256];
    unsigned char dq[256];
    unsigned char iq[256];
    unsigned char  seed[128], cipher[512];
    br_rsa_private_key sk;

    sk.n_bitlen = 1024;
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

    //size_t len = hextobin(cipher, tmp);
    size_t len = sizeof(tmp); 
    Serial.print("len before: ");
Serial.println(len); 
    if (mdec(&br_sha1_vtable, NULL, 0,  &sk, tmp, &len) != 1)
      {
        Serial.println("decryption failed");
      }
      /*
       Serial.print("len after: ");
       Serial.println(len);
     for (int i=0 ; i<len; i++)
      {
        Serial.print(tmp[i],HEX);
         Serial.print(" "); 
      }
      Serial.println(); 
      */
     
  Serial.println("<<<<<<<<<<<<<END>>>>>>>>>>>>>>>>>>");  
  }

void rsa_encrypt( br_rsa_oaep_encrypt menc)
{
  Serial.println("<<<<<<<<<<<<<Start>>>>>>>>>>>>>>>>>>"); 
    unsigned char n[512];
    unsigned char e[8];
    
    br_rsa_public_key pk;

    pk.n = n;
    pk.nlen = hextobin(n, arrn);
    pk.e = e;
    pk.elen = hextobin(e, expo);

     Serial.print("PK. Module:");
    for (int i =0 ; i<  pk.nlen; i++)
     {
      Serial.print(pk.n[i], HEX); 
      Serial.print(" "); 
     }
     Serial.println(); 
    Serial.print("PK. E:"); 
    for (int i =0 ; i<  pk.elen; i++)
     {
      Serial.print(pk.e[i], HEX);
      Serial.print(" "); 
     }
     Serial.println(); 

     unsigned char plain[512], seed[128], cipher[512];
     size_t plain_len, seed_len, cipher_len;
     rng_fake_ctx rng;
     
     size_t len;

     plain_len = hextobin(plain,plaintxt);
      Serial.print(" Plain Text Lenght");
      Serial.println( plain_len); 
      Serial.println("Text: ");
      for (int i=0 ; i<plain_len; i++)
      {
        Serial.print(plain[i],HEX);
         Serial.print(" "); 
      }
      Serial.println(); 
      
      seed_len = hextobin(seed, seedar);
       Serial.println("Seed"); 
      for (int i=0 ; i<seed_len; i++)
      {
        Serial.print(seed[i],HEX);
         Serial.print(" "); 
      }
      Serial.println();
      
     
      rng_fake_init(&rng, NULL, seed, seed_len);
      
      if (br_sha1_vtable.context_size== 0)
        Serial.println("br_sha1_vtable is NULL"); 
       else
       {
       Serial.print("br_sha1_vtable context_size :"); 
       Serial.println(br_sha1_vtable.context_size); 
       }

        unsigned long start = micros();
       len = menc(&rng.vtable, &br_sha1_vtable, NULL, 0, &pk, tmp, sizeof tmp, plain, plain_len);
      unsigned long end = micros();

      Serial.print( "Time to Encrypt (us): ");
      Serial.println(end-start);  
      Serial.print("len out from encryption is :"); 
      Serial.println(len); 
/*
    for (int i=0 ; i<len; i++)
      {
        Serial.print(tmp[i],HEX);
         Serial.print(" "); 
      }
      Serial.println(); 
      
     */
  Serial.println("<<<<<<<<<<<<<END>>>>>>>>>>>>>>>>>>");  
  }


  /*
 * Print array
 * size: the size of the array 
 * beg: index to start printing 
 */
  void Print(uint8_t * arr, int size, int beg)
{
  Serial.println();
  int start = beg; //defualt = 0
  int row =0 ;
  Serial.print(row); 
  Serial.print(" >> ");  
  for (int i =start; i<size; i++)
  {
     if(i> 0 && i % 16 ==0)
     {
      row++;
      Serial.println(); 
      Serial.print(row); 
      Serial.print(" >> ");
      
     }
    Serial.print(arr[i],HEX); 
    Serial.print(" "); 
   
  }
  Serial.println();
    
  }
  
static const unsigned char SHA1_OID[] = {
  0x05, 0x2B, 0x0E, 0x03, 0x02, 0x1A
};


  void sign_rsa(br_rsa_private fpriv, br_rsa_pkcs1_sign fsign, br_rsa_pkcs1_vrfy fvrfy)
{

Serial.println("<<<<<<<<<<<<<Start Sign>>>>>>>>>>>>>>>>>>"); 
  unsigned char t1[128], t2[128];
  unsigned char hv[20], tmp[20];
  unsigned char hv2[64], tmp2[64], sig[128];
  br_sha1_context hc;
  size_t u;


  /*
   * Verify the KAT test (computed with OpenSSL).
   */
   char * text ="Hello world"; 
  br_sha1_init(&hc);
  br_sha1_update(&hc, text, strlen(text));
  br_sha1_out(&hc, hv);

  Serial.println(" HASH :"); 
  for (int i=0 ; i<20; i++){
    Serial.print(hv[i], HEX);
    Serial.print(" "); 
  }
  Serial.println(); 
 
   unsigned long start = micros(); 
  if (!fsign(SHA1_OID, hv, 20, &RSA_SK, t2)) {
    Serial.println("Signature generation failed");
  }
  unsigned long end = micros();
  Serial.print(" Time to sign (us)"); 
  Serial.println(end-start); 

  Serial.println(" signed HASH :"); 
  for (int i=0 ; i<128; i++){
    Serial.print(t2[i], HEX);
    Serial.print(" "); 
  }
Serial.println("<<<<<<<<<<<<<End Sign>>>>>>>>>>>>>>>>>>"); 
Serial.println("<<<<<<<<<<<<<Start Verfiy>>>>>>>>>>>>>>>>>>"); 
  
  
  


    if (!fvrfy(t2, sizeof t2, SHA1_OID, sizeof tmp, &RSA_PK, tmp)) {
    Serial.println("Signature verification failed");
  }
   int i =  check_equals("Extracted hash value", hv, tmp, sizeof tmp);

  if (i ==0)
  Serial.println("sucess"); 
  else 
  Serial.println("not pass"); 

  Serial.println("<<<<<<<<<<<<<END Verfiy>>>>>>>>>>>>>>>>>>"); 
    
  }

void test_RSA_i15_enc(void)
{
 
  rsa_encrypt(&br_rsa_i15_oaep_encrypt);

 //sign_rsa(&br_rsa_i15_private,  &br_rsa_i15_pkcs1_sign, &br_rsa_i15_pkcs1_vrfy);
}

void test_RSA_i15_dec(void)
{
rsa_decrypt(&br_rsa_i15_oaep_decrypt); 

}
void loop() {


   unsigned long senc= micros();
  test_RSA_i15_enc();
   unsigned long end= micros();
   Serial.print(" Time to encrypt (us): "); 
   Serial.println(end-senc);
   
   test_RSA_i15_dec();
   unsigned long enddec= micros();
   Serial.print(" Time to decrypt (us): "); 
   Serial.println(enddec-end);
 
delay(10000);


}

#include <math.h>
#include <string.h>
#include <pbc.h>
#include <mbedtls/ssl.h>
#include <mbedtls/md.h>
#include <mbedtls/error.h>
#include <mbedtls/entropy_poll.h>

/* public key 1024 bit */
const unsigned char iot_pk_key[]= \
"-----BEGIN PUBLIC KEY-----\n" \
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQC007GfgttefDhYI5Mcv7IrPlWp\n" \
"62tCsqBdcfP/UqneljzLl0uk+1sSMCvR7oCtL+kZcCDbkbv4CjFCKkuhwebZ7YNM\n" \
"TRJ6ws37emQfS8IM7XRlYaIQmmOacDvFFjrWBd0pYKXnP8ZnBSPVmKHu8as+mz0i\n" \
"7BnKdUGhRn9atFzZDQIDAQAB\n" \
"-----END PUBLIC KEY-----\n";

const unsigned char iot_pr_key[]= \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIICXAIBAAKBgQC007GfgttefDhYI5Mcv7IrPlWp62tCsqBdcfP/UqneljzLl0uk\n" \
"+1sSMCvR7oCtL+kZcCDbkbv4CjFCKkuhwebZ7YNMTRJ6ws37emQfS8IM7XRlYaIQ\n" \
"mmOacDvFFjrWBd0pYKXnP8ZnBSPVmKHu8as+mz0i7BnKdUGhRn9atFzZDQIDAQAB\n" \
"AoGAU75CYXwJug1PTspS5BqHGe3JYGMNjpsJF52hgVo4H0R2rVbJCoP53kd+079f\n" \
"ylUI3+YE4YrxgWK/A0RxOF2DWhusL0MAUdg2sSDf9htfiCbflBD0scYFXCLQOr+9\n" \
"3GNoNIv5nPIzuqDCTbr7leAQTJfYGoz5rlcfd7s99astOkECQQDsn8fyi12I2sHc\n" \
"LkA5DngTq/z3M0cTRxhU0Bh1Hge2H2eJTZRjaD5GXnEjZvFmDU/7luUJYUzxyTYY\n" \
"IgH68Hy9AkEAw6JEYjlFqFaXPOHcyNEoRsk8J5cXGofwY4ABaifYg+EU9o9vs0+I\n" \
"8DCzyrRZbU0jt/GX8zvXJEuhxck5+pcakQJBAOv5aEBUhcHuTvhSU4/TAyKzGQI5\n" \
"a/8onnYuVMWvXgddCDbgXERKeBhbJL8mcUTRr9r6H40cMMzLWZv1hj4HyI0CQD50\n" \
"PRSGaCB3lEyRZmSNsSf38kZJS8zifPGm2czD77EaWBDmdahuya60PZGGxc3JBJAi\n" \
"hnnWLradloWH2gSP3KECQB2GGMO2Lpjvn1d2Zcpf/r4TpYYa5LbQ9sBIUyj+Obvs\n" \
"H3JAp8zyNQe3mFnp64DI3y0wB95+lU0EULtL95DhYvk=\n" \
"-----END RSA PRIVATE KEY-----\n";


#define MSG_SIZE 117
#define C_SIZE 128
//#define DBG


/*
 * call plattform-specific  TRNG driver
 */
static int myrand(void *rng_state, unsigned char *output, size_t len)
{
    size_t olen;
    return mbedtls_hardware_poll(rng_state, output, len, &olen);
}


/* mbedtls: pk.h */
/* Note: 1. For RSA keys, the default padding type is PKCS#1 v1.5., which requires at least 11 bytes for PADDING */
/*       2. Plaintext length should be max. 117 bytes and the cipher text is a fixed 128 bytes */


void setup() {

    Serial.begin(115200);
    size_t heapsize;
    heapsize = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    Serial.printf("heap free size before: %d\n",heapsize);
    
    unsigned char m[MSG_SIZE];

    unsigned char *cipher = (unsigned char *) malloc(2*MSG_SIZE);
    unsigned char *plaintext = (unsigned char *) malloc(MSG_SIZE);

    heapsize = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    Serial.printf("heap free size after: %d\n",heapsize);
    int maxBlock = 0;

    size_t oolen;
    int error_code;
    unsigned long stime, etime, enctime, dectime;
    
    esp_fill_random(m, MSG_SIZE);


    /* RSA encryption: maxBlock = keysize(1024)/8 - 11 */
    maxBlock = C_SIZE - 11;

    #ifdef DBG
    Serial.println("finish m block");
    #endif
    
    stime = micros();

    mbedtls_pk_context pk; 
    mbedtls_pk_init(&pk);

    error_code =  mbedtls_pk_parse_public_key(&pk, iot_pk_key, sizeof(iot_pk_key));
    if(error_code != 0) {
        Serial.println("READING KEY STORE PUBLIC KEY NOT SUCCESSFULL");
        mbedtls_pk_free(&pk);
    } else {

        int offset = 0;
        int i = 0;

        while(offset < MSG_SIZE) {
            if (MSG_SIZE - offset >= maxBlock) {
                int result = mbedtls_pk_encrypt( &pk, m + offset, maxBlock, cipher + i*C_SIZE, &oolen, 2*MSG_SIZE, myrand, NULL );
                if (result == 0) {
                } else {
                    Serial.println("RSA Encryption NOT SUCCESSFULL");
                }
            } else {
                int result = mbedtls_pk_encrypt( &pk, m + offset, MSG_SIZE - offset, cipher + i*C_SIZE, &oolen, 2*MSG_SIZE, myrand, NULL );
                if (result == 0) {
                } else {
                    Serial.println("RSA Encryption NOT SUCCESSFULL");
                }
            }
            
            i++;
            offset = i * maxBlock;
        }
    }
    mbedtls_pk_free(&pk);
    
    enctime = micros() - stime;

    #ifdef DBG
    Serial.println("enc done!");
    #endif

    /* rsa decryption: maxBlock = keysize(1024)/8 */
    maxBlock = C_SIZE;

    stime = micros();
    
    mbedtls_pk_context pr;
    mbedtls_pk_init(&pr);

    error_code = mbedtls_pk_parse_key(&pr, iot_pr_key, sizeof(iot_pr_key), NULL, NULL);
    if(error_code != 0) {
        Serial.println("READING KEY STORE PRIVATE KEY NOT SUCCESSFULL");
        mbedtls_pk_free(&pr);
    } else {
        size_t oplen;
    
        #ifdef DBG
        Serial.println("read private key done");  
        #endif
    
        int offset = 0;
        int offsetp = 0;
        int i = 0;

        while(offsetp < MSG_SIZE) {
            int result = mbedtls_pk_decrypt(&pr, cipher + offset, C_SIZE, plaintext + offsetp, &oplen, MSG_SIZE, myrand, NULL);
            if (result == 0) {
            } else {
                Serial.println("RSA Decryption NOT SUCCESSFULL");
                mbedtls_pk_free(&pr);
            }

            #ifdef DBG
            Serial.printf("dec: %d\n",i);
            #endif
            
            i++;
            offset = i * maxBlock;
            offsetp = offsetp + oplen;
        }
        
        mbedtls_pk_free(&pr);
    }

    dectime = micros() - stime;

    // check m and plaintext, whether they are the same
    if (!memcmp(m, plaintext, MSG_SIZE)) {
        Serial.println("m is equal to plaintext!");
    }

    Serial.printf("time to enc: \t%lu (us)\n", enctime);
    Serial.printf("time to dec: \t%lu (us)\n", dectime);

    free(plaintext);
    free(cipher);
}



void loop() {
}

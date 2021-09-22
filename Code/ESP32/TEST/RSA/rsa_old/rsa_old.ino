
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


#define MSG_SIZE 120
#define C_SIZE 128

unsigned long stime, etime, enctime, dectime;


/*
 * call plattform-specific  TRNG driver
 */
static int myrand(void *rng_state, unsigned char *output, size_t len)
{
    size_t olen;
    return mbedtls_hardware_poll(rng_state, output, len, &olen);
}

void setup() {

    Serial.begin(115200);

    unsigned char m[MSG_SIZE];
    unsigned char plaintext[128];
    unsigned char cipher[128];
    size_t oolen;
    
    esp_fill_random(m, MSG_SIZE);
    
    int error_code;

    stime = micros();
    mbedtls_pk_context pk; 
    mbedtls_pk_init(&pk);
    error_code =  mbedtls_pk_parse_public_key(&pk, iot_pk_key, sizeof(iot_pk_key));

    if(error_code != 0) {
        Serial.println("READING KEY STORE PUBLIC KEY NOT SUCCESSFULL");
        mbedtls_pk_free(&pk);
    } else {
        int result = mbedtls_pk_encrypt( &pk, m, MSG_SIZE, cipher, &oolen, C_SIZE, myrand, NULL );
        if (result == 0) {
        } else {
            Serial.println("RSA Encryption NOT SUCCESSFULL");
            mbedtls_pk_free(&pk);
        }
    }

    enctime = micros() - stime;

    stime = micros();

    mbedtls_pk_context pr;
    mbedtls_pk_init(&pr);

    error_code = mbedtls_pk_parse_key(&pr, iot_pr_key, sizeof(iot_pr_key), NULL, NULL);
    if(error_code != 0) {
        Serial.println("READING KEY STORE PRIVATE KEY NOT SUCCESSFULL");
        mbedtls_pk_free(&pr);
    } else {
        size_t oplen;
        int result = mbedtls_pk_decrypt(&pr, cipher, C_SIZE, plaintext, &oplen, C_SIZE, myrand, NULL);
        if (result == 0) {
            mbedtls_pk_free(&pr);
        } else {
            Serial.println("RSA Decryption NOT SUCCESSFULL");
            mbedtls_pk_free(&pr);
        }
    }

    dectime = micros() - stime;

    if(!memcmp(m, plaintext, MSG_SIZE)) {
        Serial.println("m equal to plaintext");
    }

    Serial.printf("time to enc: \t%lu (us)\n", enctime);
    Serial.printf("time to dec: \t%lu (us)\n", dectime);
}
void Compare(const unsigned char * t1, const unsigned char t2 , int len)
{
  int i = 0 ; 
  for (;i<len; i++){
    if (t1[i]!=t2[i]){
      return -1; 
    }
  }
  return 0; 
}

void PrintHEX(const unsigned char* str, int start_byte, int end_byte) {

    for (int i = start_byte; i < end_byte; ++i) {
        printf("%.2X ", str[i]);
    }

    printf("\n");
}
void loop() {

}

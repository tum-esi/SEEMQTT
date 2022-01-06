/*
  This program measures the time consumption of AES-128 encryption and decryption.
  The mbedtls library will use AES hardware accelerator for AES encryption and decryption
  if AES hardware accelerator is enabled on ESP32 board. By default
  AES hardware accelerator is enabled.
  
  Comment out the line #define CONFIG_MBEDTLS_HARDWARE_AES 1 in sdkconfig.h file to 
  disable the AES hardware accelerator. sdkconfig.h header file locates in
    "~/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/include/config"
  
  However, disabling AES hardware accelerator leads to function undefined error. Because 
  Arduino IDE (Version 1.8.13) uses pre-compiled static libraries to improve the project compile 
  speed. With AES hardware accelerator enabled the original aes functions in mbedtls library will 
  be overwritten by corresponding aes functions in esp32/hwcrypto/aes.h in esp32 sdk building
  procedure.

  See header files for more details:
  "~/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/include/esp32/hwcrypto/aes.h"
  "~/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/include/mbedtls/aes_alt.h"
  "~/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/include/mbedtls/mbedtls/aes.h"

  aes_sw library is a home-made aes-128 implementation with two modes available (ecb, gcm)
*/

#include "mbedtls/aes.h"
#include "aes_sw.h"

//#define HW_CRYPTO

/* key size 128 bits */
#define BLOCK_SIZE 16

uint8_t key[BLOCK_SIZE] = {0x1C,0x3E,0x4B,0xAF,0x13,0x4A,0x89,0xC3,0xF3,0x87,0x4F,0xBC,0xD7,0xF3, 0x31, 0x31};

/* size of the message */
#define Size_Byte 1024

/* global time variable */
unsigned long senc, eenc, sdec, edec;

/* 
  function to generate a random string of a given size,
  which will be encrypted and then decrypted to test the
  functionality of encryption and decryption functions
 */
static uint8_t  *rand_string(uint8_t  *str, size_t size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXZY";
    if (size) {
        for (size_t n = 0; n < size; n++) {
            int key = esp_random() % (int) (sizeof charset - 1); // exclude '\0' in the end
            str[n] = charset[key];
        }
    }
    return str;
}

uint8_t * rand_string_alloc(size_t size) {
     uint8_t  *s =(uint8_t  *) malloc(size);
     if (s) {
         rand_string(s, size);
     }
     return s;
}

void Print(uint8_t * arr, int start, int size) {
  int row = 0;
  Serial.print(row); 
  Serial.print(" >> ");  
  for (int i = start; i < size; i++) {
    if(i > 0 && i % BLOCK_SIZE == 0) {
      row++;
      Serial.println(); 
      Serial.print(row); 
      Serial.print(" >> ");
    }
    Serial.print(arr[i], HEX);
    Serial.print(" "); 
  }
  Serial.println();
}

void encryption(uint8_t * input, uint8_t * output) {
  uint8_t iv[BLOCK_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  //#ifdef HW_CRYPTO
  Serial.println("<<<<<<<<<<<<<< Start Encryption HW >>>>>>>>>>>>>>>>");
  Serial.println("Plain Text:");
  Print(input, Size_Byte - 32, Size_Byte);

  senc = micros();
  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, (const unsigned char*) key, BLOCK_SIZE * 8);
  mbedtls_aes_crypt_cbc(&aes,MBEDTLS_AES_ENCRYPT,Size_Byte,iv, input, output);
  mbedtls_aes_free(&aes);
  eenc = micros();
  
  Serial.println("Encryptted text:");
  Print(output, Size_Byte - 32, Size_Byte);
  Serial.println("<<<<<<<<<<<<<<< End Encryption HW >>>>>>>>>>>>>>>>>");

  //#else
  Serial.println("<<<<<<<<<<<<<< Start Encryption SW >>>>>>>>>>>>>>>>");
  Serial.println("Plain Text:");
  Print(input, Size_Byte - 32, Size_Byte);
  uint8_t iv2[BLOCK_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  senc = micros();
  aes128_cbc_encrypt(iv2, Size_Byte, key, input, output);
  eenc = micros();
  
  Serial.println("Encryptted text:");
  Print(output, Size_Byte - 32, Size_Byte);
  Serial.println("<<<<<<<<<<<<<<< End Encryption SW >>>>>>>>>>>>>>>>>");
  
  //#endif
 }

void decryption(uint8_t * ciphertext, uint8_t * plaintext) {
  uint8_t iv[BLOCK_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  //#ifdef HW_CRYPTO
  Serial.println("<<<<<<<<<<<<<< Start Decryption HW >>>>>>>>>>>>>>>>");
  Serial.println("Cipher Text:");
  Print(ciphertext, Size_Byte - 32, Size_Byte);

  sdec = micros();
  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_dec(&aes, (const unsigned char*) key, BLOCK_SIZE * 8);
  mbedtls_aes_crypt_cbc(&aes,MBEDTLS_AES_DECRYPT,Size_Byte,iv, ciphertext, plaintext);
  mbedtls_aes_free(&aes);
  edec = micros();
  
  Serial.println("Plain text:");
  Print(plaintext, Size_Byte - 32, Size_Byte);
  Serial.println("<<<<<<<<<<<<<<< End Decryption HW >>>>>>>>>>>>>>>>>");
  
  //#else
  Serial.println("<<<<<<<<<<<<<< Start Decryption SW >>>>>>>>>>>>>>>>");
  Serial.println("Cipher Text:");
  Print(ciphertext, Size_Byte - 32, Size_Byte);

  uint8_t iv2[BLOCK_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  sdec = micros();
  aes128_cbc_encrypt(iv2, Size_Byte, key, ciphertext, plaintext);
  edec = micros();
   
  Serial.println("Plain text:");
  Print(plaintext, Size_Byte - 32, Size_Byte);
  Serial.println("<<<<<<<<<<<<<<< End Encryption SW >>>>>>>>>>>>>>>>>");

  //#endif
}


void setup() {
  Serial.begin(115200);
 
  uint8_t * input =rand_string_alloc(Size_Byte);
  unsigned char output[Size_Byte];
  unsigned char plaintext[Size_Byte];

  encryption(input, output);
  decryption(output,plaintext);

  Serial.print(" Time to encryption (us):"); 
  Serial.println( eenc - senc); 
  Serial.print(" Time to decryption (us):"); 
  Serial.println( edec - sdec);
}
 
void loop() {}

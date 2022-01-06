/*
  Test program for aes software implementation
  aes ecb mode: basic aes enc/decryption block. It takes a 16 bytes array as input and output
  aes cbc mode: size_block should be a multiple of 16 bytes
*/

#include <stdio.h>
#include "aes_sw.h"

void main() {
  #define size_block 64
  uint8_t key[16] = {0x06, 0xa9, 0x21, 0x40, 0x36, 0xb8, 0xa1, 0x5b, 0x51, 0x2e, 0x03, 0xd5, 0x34, 0x12, 0x00, 0x06};
  uint8_t iv[16] = {0x3d, 0xaf, 0xba, 0x42, 0x9d, 0x9e, 0xb4, 0x30, 0xb4, 0x22, 0xda, 0x80, 0x2c, 0x9f, 0xac, 0x41};
  uint8_t input[size_block] = "Single block msgSingle block msgSingle block msgaaaaaaaaaaaaaaa";
  uint8_t output1[16];
  uint8_t output2[size_block];
  uint8_t dec[16];
  uint8_t dec2[size_block];

//  aes128_encrypt(input, key, output1);
//  printf("%s\n", output1);
//  aes128_decrypt(output1, key, dec);
//  printf("%s\n", dec);


  aes128_cbc_encrypt(iv, size_block, key, input, output2);
  printf("%s\n", output2);
  aes128_cbc_decrypt(iv, size_block, key, output2, dec2);
  printf("%s\n", dec2);
}

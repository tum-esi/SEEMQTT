#ifndef AES_SW_H_
#define AES_SW_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define POLY 0x1b
#define SUCC_AES_CBC 0
#define ERR_INVALID_INPUT_LENGTH 1

//macro define function of xtime()
#define xtime(x) ((x << 1) ^ (POLY & (0 - (x >> 7))))



/**
 * @brief  AES-128 decryption
 * @param  *in_ciphertext - input ciphertext to be decrypted
 * 		   *key - key used for decryption
 * 		   *out_plaintext - output plaintext
 * @return void
 */
void aes128_decrypt(uint8_t *in_ciphertext, uint8_t *key, uint8_t *out_plaintext);


/**
 * @brief  AES-128 encryption
 * @param  *in_plaintext - input plaintext to be encrypted
 * 		   *key - key used for encryption
 * 		   *out_ciphertext - output ciphertext
 * @return void
*/
void aes128_encrypt(uint8_t *in_plaintext, uint8_t *key, uint8_t *out_ciphertext);


int aes128_cbc_encrypt(uint8_t *iv, uint32_t length, uint8_t *key, uint8_t *input, uint8_t *output);
int aes128_cbc_decrypt(uint8_t *iv, uint32_t length, uint8_t *key, uint8_t *input, uint8_t *output);


void aes128_gcm_encrypt(uint8_t *in_plaintext, uint32_t in_plaintext_len, uint8_t *key, uint8_t *iv, uint8_t *aad, uint32_t aad_len, uint8_t *out_ciphertext, uint8_t *out_tag);

int aes128_gcm_decrypt(uint8_t *in_ciphertext, uint32_t in_ciphertext_len, uint8_t *key, uint8_t *iv, uint8_t *aad, uint32_t aad_len, uint8_t *in_tag, uint8_t *out_plaintext);

void ghash_fnc(uint8_t *X, uint8_t len, uint8_t *H, uint8_t *Y);

void bit_shift_right(uint8_t *V);

void gcm_mult(uint8_t *X, uint8_t *Y,  uint8_t *Z);


void aes128_hw_acce_encrypt(uint8_t *in_plaintext, uint8_t *key, uint8_t *out_ciphertext);


#ifdef __cplusplus
}
#endif

#endif /* AES_SW_H_ */

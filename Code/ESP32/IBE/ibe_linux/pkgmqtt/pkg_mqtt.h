#ifndef PKG_MQTT_H_
#define PKG_MQTT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <assert.h> 
#include <fcntl.h>
#include <math.h>   
#include <string.h>
#include <openssl/sha.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <pbc/pbc.h>
#include <pbc/pbc_test.h>
#include <mosquitto.h>


/* definition of curve parameters */
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

struct ibe_sys_param_public {
    element_t Kpub;
    element_t P;
    pairing_t pairing;
};

struct ibe_sys_param_private {
    element_t Km;
};

typedef struct ibe_sys_param_public ibe_sys_param_public_t;
typedef struct ibe_sys_param_private ibe_sys_param_private_t;

/**
 * \brief                This function will be called once by PKG to setup system parameters.
 * 
 * \param sys_param_pub  A struct contains system parameters, which will be made public.
 *                       Parameters: pairing, PKG public key Kpub and random P.
 * \param_sys_param_pri  A struct contains system parameters, which should be kept secret.
 *                       Parameters: Km (PKG private key)
 */
void ibe_setup(ibe_sys_param_public_t *sys_param_pub, ibe_sys_param_private_t *sys_param_pri);

/**
 * \brief                   This function will be called by PKG, when a user requests the private key
 *                          of corresponding ID.
 * 
 * \param sys_param_pub     A struct contains IBE public system parameters given by PKG
 * \param Km master         key of PKG (should be kept secret)
 * \param id                ID given by user
 * \param key_id            Extracted private key of given ID, which will be sent to user.
 */
void ibe_extract(ibe_sys_param_public_t *sys_param_pub, ibe_sys_param_private_t *sys_param_pri, unsigned char *id, unsigned char *Didc);

/**
 * \brief                   This function will be called by user to encrypt message using receiver ID
 *
 * \param sys_param_pub     Public system parameters given by PKG
 * \param rec_id            Receiver ID, which is used as key to encrypt the message
 * \param plaintext         Message in plaintext
 * \param cipher            Encrypted message
 */
void ibe_encryption(ibe_sys_param_public_t *sys_param_pub, unsigned char* rec_id, unsigned char *plaintext, \
                    size_t plaintext_len , unsigned char *cipher, unsigned char *Uc);

/**
 * \brief                   This function will be called by the receiver, which receives
 *                          encrypted message using his ID
 *
 * \param sys_param_pub     Public system parameters given by PKG
 * \param ciphertext        Encrypted message
 * \param ciphertext_len    Length of encrypted message
 * \param U                 Parameter given by sender, which will be used do the pairing with Did
 * \param Did               Private key of corresponding ID. Given by PKG
 * \param plaintext         Decrypted message
 */
void ibe_decryption(ibe_sys_param_public_t *sys_param_pub, unsigned char *ciphertext, size_t ciphertext_len, \
                         unsigned char *Uc, unsigned char *Didc, unsigned char *plaintext);

void PKGCallback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg);

void PrintHEX(unsigned char* str, int start_byte, int end_byte);

#endif /* end of PKG_MQTT_H_ */

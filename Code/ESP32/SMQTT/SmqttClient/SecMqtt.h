/*
  This library is a wrapper from PubSubClient library
  mbedtls library is used to do aes encryption and rsa signature
*/

#ifndef _SECMQTT_H
#define _SECMQTT_H

#include <iostream>
#include <functional>
//#include <string.h>
#include "Arduino.h"
#include "PubSubClient.h"
#include "mbedtls/pk.h"
#include "mbedtls/rsa.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/entropy.h"
#include "mbedtls/entropy_poll.h"
#include "mbedtls/aes.h"

using namespace std;

// enable to print debug message
#define DBG_MSG

// SMQTT_TIMEOUT: smqtt timeout in milliseconds. Override with setSmqttTimeout()
#ifndef SECMQTT_TIMEOUT
#define SECMQTT_TIMEOUT 90000
#endif

// Possible values for _smqtt_state
#define SECMQTT_CONNECTION_TIMEOUT -2
#define SECMQTT_KS_DISCONNECTED -1
#define SECMQTT_KS_CONNECTED 0
#define SECMQTT_CONNECT_GOOD_NONCE 1
#define SECMQTT_CONNECT_BAD_NONCE 2

#define BLOCK_SIZE 16 // aes-128 block size
#define C_SIZE 128 // size of the encrypted key
#define MAX_SK_USAGE 20 // a session key can only be used in MAX_SK_USAGE times communication, then needs to be updated
#define TOPIC_SIZE 50 // maximum size of a topic
#define NONCE_SIZE 16 // size of nonce
#define MAX_MSG_SIZE 1024 // maximum size of a message

/*
  1. encrypt key K1 using public key from key store
  2. send Epk(K1) and nonce. Wait for ACK
  3. check nonce. If ok, generate session key (Sk), encrypts Sk using K1 and send Ek(Sk) to Key Store.
  4. encrypts message using Sk and publish message
*/


/*
  SECMQTT TOPIC TREE
  MK/
     PK1/value
        /ack
        /sk

     PK2/value
        /ack
        /sk
  
     PK3/value
        /ack
        /sk
     .
     .
     .
*/


class SecMqtt: public PubSubClient {
  protected:
    const unsigned char* _iot_pk_key;
    const unsigned char* _iot_pr_key;
    const unsigned char* _ks_pk_key; // ks key should be an array
    unsigned int _iot_pk_key_size;
    unsigned int _iot_pr_key_size;
    unsigned int _ks_pk_key_size;
    const unsigned char* _ca_cert;
    const unsigned char* _cred;
    unsigned char _ks_sym_key[BLOCK_SIZE];
    unsigned char _session_key[BLOCK_SIZE];
    unsigned char _nonce[NONCE_SIZE];
    char _conn_topic[TOPIC_SIZE];
    char _ack_topic[TOPIC_SIZE];
    char _sk_topic[TOPIC_SIZE];
    char _sk_ack_topic[TOPIC_SIZE];
    int _sk_counter; // count how many messages encrypted using this same session key
    int _secmqtt_state;
    String _base_topic;
    
  public:
    SecMqtt();
    SecMqtt(Client& client);
    int aes_encryption(const unsigned char* input, size_t input_len, const unsigned char* key, unsigned char* iv, unsigned char* output);
    int aes_decryption(const unsigned char* input, size_t input_len, const unsigned char* key, unsigned char* iv, unsigned char* output);
    static int myrand(void *rng_state, unsigned char *output, size_t len);
    size_t rsa_encryption(unsigned char * plainText, size_t P_SIZE, unsigned char * cipher);
    void rsa_sign(const unsigned char * input, size_t P_SIZE, unsigned char * sign);
    void sym_key_generator(unsigned char* sym_key);
    void SecConnect();
    void SecPublish(const char* topic, const unsigned char* msg, size_t msg_len);
    void SecCallback(char* topic, uint8_t* payload, unsigned int payload_length);
    void SecSessionKeyUpdate();
    void secmqtt_set_cred(const unsigned char* cred);
    void secmqtt_set_ca_cert(const unsigned char* ca_cert);
    void secmqtt_set_iot_pk_key(const unsigned char* key, unsigned int key_size);
    void secmqtt_set_iot_pr_key(const unsigned char* key, unsigned int key_size);
    void secmqtt_set_ks_pk_key(const unsigned char* key, unsigned int key_size);
    int get_state();
    void PrintHEX(unsigned char* arr, int arr_size);
};

#endif /* End of _SECMQTT_H */

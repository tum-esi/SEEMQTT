/*
  This library is a wrapper from PubSubClient library
  mbedtls library is used to do aes encryption and rsa signature
*/

#ifndef _SECMQTTKSN_H
#define _SECMQTTKSN_H

#include <iostream>
#include <functional>
#include "Arduino.h"
#include "PubSubClient.h"
#include "mbedtls/pk.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/entropy.h"
#include "mbedtls/entropy_poll.h"
#include "mbedtls/aes.h"
#include "mbedtls/gcm.h"
#include "pbc.h"
#include "secconfig.h"
#include "shamirsecretshare.h"
#include "KeynoteESP.h"

using namespace std;


#define DATA    "data"
#define CR      "CR"

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


extern unsigned char kpub_t[ELEMENT_LEN], p_t[ELEMENT_LEN], km_t[20];

typedef struct ibe_sys_param_public {
    element_t Kpub;
    element_t P;
    pairing_t pairing;
} ibe_sys_param_pub_t;

typedef struct ibe_sys_param_private {
    element_t Km;
} ibe_sys_param_private_t;

typedef struct tuples {
    bool verified = false;
    unsigned char masterkey[BLOCK_SIZE];
    unsigned char nonce[BLOCK_SIZE];
    char ibe_id[50];
    unsigned int ibe_id_len;
    char value_topic[50];
    char ack_topic[50];
    char sk_topic[50];
    element_t Qid;
} ks_tuples_t;

typedef struct time_message {
    unsigned long t_s;
    unsigned long t_enc;
    unsigned long t_dec;
    unsigned long t_connect;
    unsigned long t_send_pk;
    unsigned long t_recv[5];
    unsigned long t_recvs[5];
    unsigned long t_keysplit;
    unsigned long t_cred;
    unsigned long t_cred_sign;
    unsigned long t_p11; // time to generate and send all keys and start waiting for ack
    unsigned long t_p11s;
    unsigned long t_p2;
    unsigned long t_p3;
    unsigned long t_p3enc;
    unsigned long t_p3send;
} time_msg_t;

/*
  1. encrypt key K1 using public key from key store
  2. send Epk(K1) and nonce. Wait for ACK
  3. check nonce. If ok, generate session key (Sk), encrypts Sk using K1 and send Ek(Sk) to Key Store.
  4. encrypts message using Sk and publish message
*/

class SecMqtt: public PubSubClient {
  protected:
    const unsigned char* _iot_pk_key;
    const unsigned char* _iot_pr_key;
    int _iot_pk_key_size;
    int _iot_pr_key_size;
    const unsigned char* _cred;
    unsigned char _ks_sym_key[BLOCK_SIZE];
    unsigned char _session_key[BLOCK_SIZE];
    unsigned char _nonce[NONCE_SIZE];
    unsigned char _iot_pk_key_hash[HASH_LEN];
    unsigned char _iot_data_hash[HASH_LEN];
    unsigned char _shares[KSN_NUM][BLOCK_SIZE];
    char _shares_sss[KSN_NUM][SSS_SIZE];
    char _conn_topic[TOPIC_SIZE];
    char _ack_topic[TOPIC_SIZE];
    char _sk_topic[TOPIC_SIZE];
    char _sk_ack_topic[TOPIC_SIZE];
    char _ibe_ack_topic[TOPIC_SIZE];
    int _sk_counter;
    int _secmqtt_state;
    bool _ibe_mode;
    bool _sss_mode;
    bool _ksn_mode;
    String _base_topic;
    ks_tuples_t ksn_list[KSN_NUM];
    ibe_sys_param_pub_t ibe_param_pub;
    ibe_sys_param_private_t ibe_param_pri;

  public:
    SecMqtt();
    SecMqtt(Client& client);
    int aes_encryption(const unsigned char* input, size_t input_len, const unsigned char* key, unsigned char* iv, unsigned char* output);
    int aes_decryption(const unsigned char* input, size_t input_len, const unsigned char* key, unsigned char* iv, unsigned char* output);
    int aes_gcm_encryption(const unsigned char* input, size_t input_len, const unsigned char* key, unsigned char* iv, unsigned char* output, const unsigned char* add, size_t add_len, unsigned char* tag, size_t tag_len);
    int aes_gcm_decryption(const unsigned char* input, size_t input_len, const unsigned char* key, unsigned char* iv, unsigned char* output, const unsigned char* add, size_t add_len, unsigned char* tag, size_t tag_len);
    static int myrand(void *rng_state, unsigned char *output, size_t len);
    int ibe_encryption(int ks_id, unsigned char *plaintext, size_t plaintext_len , unsigned char *cipher, unsigned char *Uc);
    void rsa_sign(const unsigned char * input, size_t P_SIZE, unsigned char * sign);
    void sym_key_generator(unsigned char* sym_key);
    void SecConnect(const char *client_id);
    void SecPublish(const char* topic, const unsigned char* msg, size_t msg_len);
    void SecCallback(char* topic, uint8_t* payload, unsigned int payload_length);
    void SecSessionKeyUpdate();
    void secmqtt_set_cred(const unsigned char* cred);
    void secmqtt_set_iot_pk_key(const unsigned char* key, int key_size);
    void secmqtt_set_iot_pr_key(const unsigned char* key, int key_size);
    void secmqtt_key_split();
    void secmqtt_sss_split();
    void secmqtt_set_enc_mode(char *mode);
    void secmqtt_set_secret_share_mode(char *mode);
    void secmqtt_set_ibe_id(const char*id, int idlen, int ksid);
    bool secmqtt_check_all_ksn_nonce_stat();
    int get_state();
    void PrintHEX(unsigned char* arr, int arr_size);
    time_msg_t time_info;
};

#endif /* End of _SECMQTTKSN_H */

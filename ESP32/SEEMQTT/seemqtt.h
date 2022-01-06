
/*
 * Copyright (c) 2021 Hangmao Liu <liuhangmao@hotmail.com>
 *                    Mohammad Hamad <mohammad.hamad@tum.de>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */
/*
  This library is a wrapper from PubSubClient library
  mbedtls library is used to do aes encryption and rsa signature
*/

#ifndef _SEEMQTT_H
#define _SEEMQTT_H

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


//#define DATA    "data"
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
    element_t Gid ;
} ks_tuples_t;

typedef struct time_message {
  unsigned long t_connect; // time to connectt_send_pk
  unsigned long t_s; //  the start of certain action.t_send_pk
  unsigned long t_p1s; // the starting time of Phase I-1
  unsigned long t_p11s; // the starting time of Phase I-1
  unsigned long t_p11_start[KSN_NUM]; // starting time to setup connection with ks_i
  unsigned long t_p12_end[KSN_NUM]; // time after recieving the ack from ks_i
  unsigned long t_ibe_enc; //time to encrypt the  Symmetric master key using IBE
  unsigned long t_ibe_oto; //time to calculate the ID that are used in IBE encryption. This is one time overhead
  unsigned long t_p11_publish; // time to Publish the master symmetric key
  unsigned long t_recvs[KSN_NUM]; // time after publishing the Symmetric master to the Br
  unsigned long t_p11;   // Phase I-1  timw (T_{PhaseI-1})
  unsigned long t_recv[KSN_NUM]; //  the time between Publish the symertic master key and the receiving the acknowledgement of ks_i
  unsigned long t_p12_dec; // time to decrypt the acknowledgment
  unsigned long t_p12;   // time of phase I-2
  unsigned long t_p1 ; //time of Phase I
  unsigned long t_p2_s ; //  Sarting time of Phase II
  unsigned long t_keysplit; // time to split the topic key
  unsigned long t_p2_share_enc; // time to encrypt share
  unsigned long t_p2_share_publsih; // time to encrypt share
  unsigned long t_cred; // time to  Publish credential
  unsigned long t_cred_sign; // time to sign the credential
  unsigned long t_p2; //totoal time of Phase II
  unsigned long t_p3_enc;  // time to encrypt the messgae
  unsigned long t_p3_pub; //time to publish the encrypted message
  unsigned long t_p3_all; // toatal time of Phase III
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
    unsigned char * _iot_credntial;
    char _data_topic[TOPIC_SIZE];
    int _iot_pk_key_size;
    int _iot_pr_key_size;
    int _iot_credntial_size;
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
    void SetDataTopic(const char* topic);
    void SecDisconnect();
    void SecPublish(const char* topic, const unsigned char* msg, size_t msg_len);
    void SecCallback(char* topic, uint8_t* payload, unsigned int payload_length);
    void SecSessionKeyUpdate();
    void secmqtt_set_cred(const unsigned char* cred);
    void secmqtt_set_iot_pk_key(const unsigned char* key, int key_size);
    void secmqtt_set_iot_pr_key(const unsigned char* key, int key_size);
    void secmqtt_set_iot_credential(const unsigned char* cre, int cre_size);
    void secmqtt_key_split();
    void secmqtt_sss_split();
    char * secmqtt_sss_split(unsigned char * key, int n , int t);
    char * secmqtt_sss_combine(const char * shares, int t);
    void secmqtt_set_enc_mode(char *mode);
    void secmqtt_set_secret_share_mode(char *mode);
    void secmqtt_set_ibe_id(const char*id, int idlen, int ksid);
    bool secmqtt_check_all_ksn_nonce_stat();
    int get_state();
    int set_state(int i);
    void PrintHEX(unsigned char* arr, int arr_size);
    unsigned long  Median(unsigned long arr[], int len);
    unsigned long  Max(unsigned long arr[], int len);
    time_msg_t time_info;
};

#endif /* End of _SEEMQTT_H*/

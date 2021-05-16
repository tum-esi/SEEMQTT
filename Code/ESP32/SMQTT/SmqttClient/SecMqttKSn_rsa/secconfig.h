#ifndef _SECCONFIG_H
#define _SECCONFIG_H

/* DEBUG FLAGS  */
//#define DBG_MSG
#define TIME_MSG

/* KeyStore */
#define KSN_NUM 5
#define MAX_SK_USAGE 5 // a session key can only be used in MAX_SK_USAGE times communication, then needs to be updated

/* Shamir Secret Sharing Params */
#define PRIME 257
#define SSS_T 2 // SSS threshold (<= KSN_NUM)
#define SSS_SIZE 39 // 6 + 2*BLOCK_SIZE + 1

/* Crypto Params */
#define BLOCK_SIZE 16
#define C_SIZE 128 // rsa key size (1024 bits)
#define PK_LEN 273
#define HASH_LEN 32
#define ELEMENT_LEN 128

#define TOPIC_SIZE 50 // maximum size of a topic
#define NONCE_SIZE 16 // size of nonce

#endif /* End of _SECCONFIG_H */


#ifndef CONFIG_H_
#define CONFIG_H_


//#define CRYP_DBG
//#define DBG
#define SELF_TEST
#define TOPIC_SIZE 50
#define BLOCK_SIZE 16
#define NONCE_SIZE 16
#define MAX_TEXT_SIZE 1024
#define PKLEN 273
#define HASH_LEN 32




#define KSN_NUM 3
#define MAX_SK_USAGE 5 // a session key can only be used in MAX_SK_USAGE times communication, then needs to be updated

/* Shamir Secret Sharing Params */
#define PRIME 257
#define SSS_T 2 // SSS threshold (<= KSN_NUM)
#define SSS_SIZE 39 // 6 + 2*BLOCK_SIZE + 1


#endif

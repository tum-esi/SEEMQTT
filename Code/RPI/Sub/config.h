
#ifndef CONFIG_H_
#define CONFIG_H_


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>


//#define CRYP_DBG
#define DBG
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



/* Paths  and the name of the credentials, pk, asserts */ 
#define CR2  "cred1"  // CR^(CA)_SUB
#define CR1	 "cred2"  // CR(Pub)_(CA)
#define SUBPK "sub_pk" //pkK_Sub

#endif

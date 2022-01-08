#ifndef _CONFIG_H_
#define _CONFIG_H_

#define DBG
#define SHARPATH "../../SHARDB"

#define KS_NUM 3

struct KS_INFO {
int id ;
char ip [16];
int port};

struct KS_INFO  kslist[KS_NUM];

#endif

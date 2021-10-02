#ifndef _CONFIG_H_
#define _CONFIG_H_

/* update the list of the port based on the number of the ks you have */ 
#define KS_NUM 3
#define DBG 1 
struct KS_INFO {
int id ; 
char ip [16]; 
int port}; 

struct KS_INFO  kslist[KS_NUM]; 




#endif

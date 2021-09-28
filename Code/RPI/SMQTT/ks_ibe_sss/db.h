
#ifndef DB_H_
#define DB_H_

#include <stdio.h>
#include "typ.h"

#define MAXDBROW 10 

typedef struct list {
unsigned char hiot[HASH_LEN];
unsigned char htpk[HASH_LEN]; 
unsigned char msymk[BLOCK_SIZE] ; 
unsigned char shar[SHAR_LEN];  
int isuded ; 
	} LIST; 
	

LIST db[MAXDBROW]; 


void SaveShar(int id, LIST lst); 
void Initlst (LIST l); 
void Initdb(); 
void Printlst(LIST l ) ; 
void Printdb(); 
int GetShare(const unsigned  char * iotid, const unsigned char * tpkid, unsigned char * share );
int GetSymK(const unsigned  char * iotid,  unsigned char * symk ); 
int Getindex (const unsigned  char * iotid); 
int AddsymK(const unsigned  char * iotid, const unsigned char * symk); 
int Addshare (const unsigned  char * iotid, const unsigned char * tpkid, const unsigned char * share );
int AddRow(const unsigned  char * iotid,  const unsigned char * symk, const unsigned char * tpkid, const unsigned char * share); 

#endif


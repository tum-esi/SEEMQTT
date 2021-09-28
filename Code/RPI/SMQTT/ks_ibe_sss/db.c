#include "db.h"
#include "typ.h"




/* 
 * Initdb:  Initialize the db which  will contains the information related to every topic and iot device 
 */ 
void Initdb () 
	{
		int i = 0 ; 
		for (i = 0 ; i< MAXDBROW;  i++)
			Initlst(db[i]); 
	}

/*
 * Initlst: intalize a row in the db 
 */ 
void Initlst (LIST lst) 
{
	memset(lst.htpk,  '\0', HASH_LEN); 
	memset(lst.hiot,  '\0', HASH_LEN); 
	memset(lst.msymk, '\0', BLOCK_SIZE); 
	memset(lst.shar,  '\0', SHAR_LEN); 
	lst.isuded = 0 ; 
	
	}

void Printdb()
{
	int i = 0;
	for (; i<MAXDBROW; i++)
	{
		if (db[i].isuded ==1)
			Printlst(db[i]); 
	}
}
/*
 * Printlst: print a row in db
 */ 
void  Printlst(LIST lst)
{
	int i ; 
	printf("***************************\n");
	printf("iotpk: "); 
	for (i = 0; i< HASH_LEN; i++)
		printf("%x", lst.hiot[i]); 
	printf("\n"); 
	
		printf("topic: "); 
	for (i = 0; i< HASH_LEN; i++)
		printf("%x", lst.htpk[i]); 
	printf("\n"); 
	
	
		printf("master symetric key : "); 
	for (i = 0; i< BLOCK_SIZE; i++)
		printf("%x", lst.msymk[i]); 
	printf("\n"); 
	
		printf("share: "); 
	for (i = 0; i< SHAR_LEN; i++)
		printf("%x", lst.shar[i]); 
	printf("\n"); 
	
	printf("***************************\n"); 
	
	}
	
	void SaveShar(int id, LIST lst)
{
	
	/*
	 *************************************
	 * H(pkK_iot)| H(tp_i)| symK | share *
	 *************************************
	 */
    char name[5]; 
    
    sprintf(name, "%d.txt", id);
    char path [124] ="/home/pi/Desktop/SSSS/" ; 
    strcat(path,name); 
    FILE *fptr;
   fptr = fopen(path,"w");

   if(fptr == NULL)
   {
      printf("Error!");   
      exit(1);             
   }
	int i = 0 ; 

	for (i = 0; i< HASH_LEN; i++)
		fprintf(fptr, "%x", lst.hiot[i]); 
	fprintf(fptr, "%s", "|"); 
	
		for (i = 0; i< HASH_LEN; i++)
		fprintf(fptr, "%x", lst.htpk[i]); 
	fprintf(fptr, "%s", "|"); 
	
	

	for (i = 0; i< BLOCK_SIZE; i++)
		fprintf(fptr, "%x", lst.msymk[i]); 
	fprintf(fptr, "%s", "|"); 
	
	for (i = 0; i< SHAR_LEN; i++)
		fprintf(fptr, "%x", lst.shar[i]); 

	fprintf(fptr, "%s", "|\n"); 

   fclose(fptr);

   return 0;
    
}
/*
 * GetShare: extract the secret share  based on the topic and iot id 
 */ 
int GetShare(const unsigned  char * iotid, const unsigned char * tpkid, unsigned char * share )
{
	int i = 0; 
	for (; i<MAXDBROW; i++)
	{
		if (memcmp(iotid, db[i].hiot,HASH_LEN)==0 && memcmp(tpkid, db[i].htpk,HASH_LEN)==0)
		{
				memcpy(share, db[i].shar,SHAR_LEN); 
				return 0 ; 
			}
			
	}
	return -1; // not exist 
}

int Addshare (const unsigned  char * iotid, const unsigned char * tpkid, const unsigned char * share )
{
	int i = 0; 
	int index = -1; 
	for (; i<MAXDBROW; i++)
	{
		if (db[i].isuded ==1)
		{
			if (memcmp(iotid, db[i].hiot,HASH_LEN)== 0 )
			{
				index = i ;
				
				/* udate the tpk and share*/
				if (Hastopic(index) == 0)
				{
					memcpy(db[i].htpk, tpkid, HASH_LEN) ; 
					memcpy(db[i].shar, share, SHAR_LEN) ; 
					return i ; 
				}
				/* update share */	
				if (memcmp(tpkid, db[i].htpk,HASH_LEN)==0)
				{
					memcpy(db[i].shar, share, SHAR_LEN) ; 
					return i ; 
				} 
			}	
		}
	}
	/* not exist  */ 
	if (index == -1)
	{
		printf(" Error: adding secret share for non registered iot device\n") ; 
		return -1 ; 
	}
	else 
	{
	/* add new row  with the new topic and share*/
		return AddRow(iotid, db[index].msymk, tpkid,share) ; 
	}
	
}

/*
 * GetSymK: get the symmetric master key of certin iot  id (iotid) and store it in symk 
 */ 
int GetSymK(const unsigned  char * iotid,  unsigned char * symk )
{
	int i = 0; 
	for (; i<MAXDBROW; i++)
	{
		if (memcmp(iotid, db[i].hiot,HASH_LEN)==0)
		{
				memcpy(symk, db[i].msymk, BLOCK_SIZE); 
				return i ; 
			}
			
	}
	return -1; // not exist 
}

int Getindex (const unsigned  char * iotid)
{
	int i = 0; 
	for (; i<MAXDBROW; i++)
	{
		if (memcmp(iotid, db[i].hiot,HASH_LEN)==0)
			return i; 
			
	}
	return -1 ; 
}

/* 
 * AddsymK: add master symetric key to db. 
 * if there is an exiting row, update the  symetric key 
 * other wise  create a new row and return the id of that row. 
 */
int AddsymK(const unsigned  char * iotid, const unsigned char * symk)
{
	int i = 0; 
	// search if it is already exisiting 
	int exist = 0 ; 
		
	for (; i<MAXDBROW; i++)
	{
		if (db[i].isuded == 1  && memcmp(iotid, db[i].hiot,HASH_LEN)==0 )
		{
			memcpy	(db[i].msymk, symk,BLOCK_SIZE) ; 
			exist = 1; 
		}
	}
	if (exist ==0)
	{
		for (i = 0; i<MAXDBROW; i++)
		{
			if (db[i].isuded == 0 )
			{
				memcpy	(db[i].hiot, iotid,HASH_LEN) ; 
				memcpy	(db[i].msymk, symk,BLOCK_SIZE) ; 
				db[i].isuded = 1; 
				return i; 
			}
		}
	}
	return -1 ; 
}
/*
 * Add one row to the db 
 */
int AddRow(const unsigned  char * iotid,  const unsigned char * symk, const unsigned char * tpkid, const unsigned char * share){
		int i = 0; 
	for (; i<MAXDBROW; i++)
	{
		if (db[i].isuded == 0 )
		{
			memcpy	(db[i].hiot, iotid,HASH_LEN) ; 
			memcpy	(db[i].msymk, symk,BLOCK_SIZE) ;
			memcpy	(db[i].htpk, tpkid,HASH_LEN) ; 
			memcpy	(db[i].shar, share,SHAR_LEN) ;
			db[i].isuded = 1; 
			return i; 
		}
	}
	return -1 ; 
}
/*
 * Hastopic: check if the row has already a topic 
 */ 
int Hastopic(int id )
{
  unsigned char empty[HASH_LEN];  
  memset(empty, '\0', HASH_LEN) ; 
  if (memcmp(empty, db[id].htpk, HASH_LEN) == 0)
	return 0 ;	
 return 1 ; 
}

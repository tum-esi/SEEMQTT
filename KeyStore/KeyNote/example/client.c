#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vp_msg.h"
#include <regex.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>


#include "../keynote.h"
#include "../header.h"

/*
 * This program connects to host whose name is given as argument 1 over port SRV_PORT
 * and sends the message given to it as argument 2.
 */
 char * getcre(char * path, int *len);
char* getQuery();
char* SignedQuery(char* cond_buf);
char* getCredentials( char * path, int * len);
char * getpk(int * len);
void print_key(FILE *fp, char *algname, char *key, int start, int length);
char*  SetSigKey(char *key, int start, int length);
//int  SetSigKey(char*[] result,char *algname, char *key, int start, int length)
int main(int argc, char **argv)
{
    char * topic= "t1"; 
    char * lication = "l1"; 
	int sock;
	struct sockaddr_in server;
	struct hostent *hp;
	char *srv_hostname;
	int plen;
	char pbuf[100] ={'0'};

	if (argc != 2) {
		fprintf(stderr, "Usage: %s hostname \n", argv[0]);
		exit(1);
	}
	srv_hostname = argv[1];
	/* Create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
	      perror("opening stream socket\n");
	      exit(1);
	}

	/* Connect socket using name specified by command line. */
	server.sin_family = AF_INET;
	hp = gethostbyname(srv_hostname);
	if (hp == 0) {
	      fprintf(stderr, "%s: unknown host\n", srv_hostname);
	      exit(2);
	}

	memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
	server.sin_port = htons(SRV_PORT);
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
	      perror("connecting stream socket");
	      exit(1);
	}
    
    
    
/*
 * *************************************************************
 *  Totla_len |c|cr_len|cr|.......|k|pk_l|PK|r|R_l|R | s_l | S *
 * *************************************************************
 */
 
        
 
     //    int cr_l ;
       // char * cr = getCredentials(&cr_l); 
       
       
       int cr1_len ; 
       char * cr1= getCredentials("cred1",&cr1_len);
       
       int cr2_len ; 
       char*  cr2= getCredentials("cred2",&cr2_len); 
       
       

        printf(" credential 1 length = %d \n", cr1_len);
        mhh_printchar(cr1, cr1_len); 
        
         printf(" credential 2 length = %d \n", cr2_len);
        mhh_printchar(cr2, cr2_len); 
        
        
         int pk_l ;
        char * pk = getpk(&pk_l);
       
        
        printf(" pk length = %d \n", pk_l);
        mhh_printchar(pk, pk_l);  
          
          
        char * asrt = "topic=\"t1\"\n"\
        "location=\"l1\"\n";  
        int asrt_l = strlen(asrt); 
        
         mhh_printchar(asrt, asrt_l); 
        
    
        
        
        int cr_l = cr1_len + cr2_len; 
        
        
        int msg_l = sizeof (int)  + cr_l  +  sizeof(int) *2 + asrt_l+ sizeof (int)+  pk_l +  +sizeof (int); 
        
        

        
        char *  msg= (char *)malloc(msg_l); 
        
        memset(msg,0,msg_l);
        
        memcpy(msg, &msg_l, sizeof(int));  // total length
        
        //memcpy(msg+sizeof(int), &cr_l, sizeof(int));  //cr lnght
        //memcpy(msg+2*sizeof(int), cr, cr_l); //cr
        
        memcpy(msg+sizeof(int), &cr1_len, sizeof(int));  
        memcpy(msg+2*sizeof(int), cr1, cr1_len); 
        
        memcpy(msg+2*sizeof(int)+ cr1_len, &cr2_len, sizeof (int) ); 
        memcpy(msg+3*sizeof(int)+ cr1_len, cr2, cr2_len); 
        
        memcpy(msg+3*sizeof(int)+ cr_l, &pk_l, sizeof (int) ); 
  
        memcpy(msg+3*sizeof(int)+ cr_l+ sizeof(int), pk, pk_l ); 
        
        memcpy(msg+3*sizeof(int)+ cr_l+ sizeof(int) + pk_l , &asrt_l, sizeof (int) ); 
        
        memcpy(msg+3*sizeof(int)+ cr_l+ sizeof(int) + pk_l + sizeof(int) , asrt, asrt_l); 
          
        
      printf(" Totla Message size = %d \n" , msg_l);
      int  sent =  0 ;
       int remin = msg_l ; 
     while (remin >0 )
     {

            int s = write( sock, msg+sent , remin); 
            printf(" sent %d \n" , s );
            sent += s; 
            remin -=s ; 
     }
     
     int re = 0; 
	
	  int result = read(sock, &re, sizeof(re));
      printf(" result = %d" , result ); 

	printf("decision: <<%s>>\n", pbuf);
	
	//fclose(sock);
    return 0;

}


char* SignedQuery(char* cond_buf)
{
    FILE *fp;
    long lSize;
    long pr_lSize;
    long assertion_Size;
    long query_Size;
    char *Query;
    char *assertion_buffer;// Query
    char *buffer;// public key
    char *pr_buffer;// private key
    char *sig; // signature
    char *algname="sig-rsa-sha1-hex:";
    int flg=0;
    
    
    fp= fopen("public_key","r");
    if (fp == NULL)
    {
        fprintf(stderr, "public_key file is missing ");
        return 0;
    }
    fseek(fp,0,SEEK_END);
    lSize = ftell(fp);
    rewind(fp);
    buffer = malloc(lSize+1);
    fread(buffer,lSize,1,fp);
    buffer[lSize]='\0';
    fclose(fp);

    char *au = "Authorizer: ";
    char *lic = "licensees: \"REQUEST\"";
    char *con=  "\nconditions: \"";
    char *si=   "\nsignature: ";
    
    long kk = strlen(con)+strlen(cond_buf);
    char *condition;
    condition = malloc(kk+1);
    sprintf(condition,"%s%s\"",con,cond_buf);
    
    
    assertion_Size = strlen(au)+strlen(buffer)+strlen(lic)+strlen(condition)+strlen(si)+1;
    
    assertion_buffer = malloc(assertion_Size);
    
    //snprintf(assertion_buffer,strlen(au),"%s",au);
    strcat(assertion_buffer,au);
    strcat(assertion_buffer,buffer);
    strcat(assertion_buffer,lic);
    strcat(assertion_buffer,condition);
    strcat(assertion_buffer,si);
    
    
    // bring the privite key
    fp= fopen("private_key","r");
    if (fp == NULL)
    {
        fprintf(stderr, " couldn't open private_key file ");
        return 0;
    }
    fseek(fp,0,SEEK_END);
    pr_lSize = ftell(fp);
    rewind(fp);
    pr_buffer = malloc(pr_lSize+1);
    fread(pr_buffer,pr_lSize,1,fp);
    pr_buffer[pr_lSize]='\0';
    fclose(fp);
    // sign the  query
    sig = kn_sign_assertion(assertion_buffer, assertion_Size, pr_buffer, algname, flg);
   
   
    char *key;
    key=SetSigKey(sig, SIG_PRINT_OFFSET, SIG_PRINT_LENGTH);

    query_Size= assertion_Size + strlen(key)+1;
    Query= malloc(query_Size);
    
    snprintf(Query,query_Size,"%s%s",assertion_buffer,key);
    //strcat(Query,assertion_buffer);
    //strcat(Query,key);

    free(assertion_buffer);
    
    free(buffer);
    free(pr_buffer);
    
    return Query;
}

char * getcre(char * path, int *len)
{
    char * buf;
    int fd,i =0 ;
    int cl = 8192;
    struct stat sb; 
    
    buf = (char *) malloc(cl * sizeof(char)); 
    if (buf ==NULL)
    {
        perror("calloc()");
		exit(-1);
    }
    if ((fd = open(path, O_RDONLY, 0)) < 0)
	{
	    perror(path);
	    exit(-1);
	}

	if (fstat(fd, &sb) < 0)
	{
	    perror("fstat()");
	    exit(-1);
	}

	if (sb.st_size > cl - 1)
	{
	    free(buf);
	    cl = sb.st_size + 1;
	    buf = (char *) calloc(cl, sizeof(char));
	    if (buf == (char *) NULL)
	    {
		perror("calloc()");
		exit(-1);
	    }
	}
    
    i = read(fd, buf, sb.st_size);
	if (i < 0)
	{
	    perror("read()");
	    exit(-1);
	}

	close(fd);
    *len = cl ;
    return buf; 
    }




/*
 * Read Credentials 
 * 
 */ 
char* getCredentials( char * path, int * len)
{
    FILE *fp;
    long cre_Size;
    char *result;
    fp= fopen(path,"r");
    if (fp == NULL)
    {
        fprintf(stderr, "Credential file is missing ");
        exit(1);
    }
    fseek(fp,0,SEEK_END);
    cre_Size = ftell(fp);
    printf(" Credentials Size %d\n",cre_Size);
    rewind(fp);
    result = malloc(cre_Size+1);
    fread(result,cre_Size,1,fp);
    fclose(fp);
    result[cre_Size]='\0';
    *len =cre_Size;
    return result;
}
/* 
 * Get publick key 
 */ 

char* getpk(int * len)
{
    FILE *fp;
    long pol_Size;
    char *result;
    fp= fopen("sub_pub","r");
    if (fp == NULL)
    {
        fprintf(stderr, "Policy file is missing ");
        exit(1);
    }
    fseek(fp,0,SEEK_END);
    pol_Size = ftell(fp);
    rewind(fp);
    result = malloc(pol_Size+1);
    fread(result,pol_Size,1,fp);
    fclose(fp);
    //result[pol_Size]='\0';
    *len =pol_Size;
    return result;
}

char* getQuery()
{
    FILE *fp;
    long query_Size;
    char *Query_buf;
    
    fp= fopen("Client_files/Query","r");
    if (fp == NULL)
    {
        fprintf(stderr, " couldn't open Query file. ");
        exit(1);
    }
    fseek(fp,0,SEEK_END);
    query_Size = ftell(fp);
    rewind(fp);
    Query_buf = malloc(query_Size+1);
    fread(Query_buf,query_Size,1,fp);
    fclose(fp);
    //Query_buf[query_Size ]='\0';
    return Query_buf;
}
void print_space(FILE *fp, int n)
{
    while (n--)
        fprintf(fp, " ");
}
void print_key(FILE *fp, char *algname, char *key, int start, int length)
{
    int i, k;
    
    print_space(fp, start);
    fprintf(fp, "\"%s", algname);
    
    for (i = 0, k = strlen(algname) + 2; i < strlen(key); i++, k++)
        
    {
        if (k == length)
        {
            if (i == strlen(key))
            {
                fprintf(fp, "\"\n");
                return;
            }
            
            fprintf(fp, "\\\n");
            print_space(fp, start);
            i--;
            k = 0;
        }
        else
            fprintf(fp, "%c", key[i]);
    }
    
    fprintf(fp, "\"\n");
}

void put_space(char *buf,int start, int n)
{
    
    while (n--)
    {
        buf[start]=' ';
        start++;
    }
}
char*  SetSigKey(char *key, int start, int length)
{
    char *rkey;
    char result[3000];
    int i, k;
    
    put_space(result, 0,start);
    
    int index = start;
    result[index] ='"';
    index+=1;
    
    for (i = 0, k =2; i < strlen(key); i++, k++)
        
    {
        
        if (k == length)
        {
            
            if (i == strlen(key))
            {
                result[index++] ='"';
                result[index++] ='\n';
                return result;
            }
            // end of line
            result[index] ='\\';
            result[index+1] ='\n';
            index=index+2;
            put_space(result,index ,start);
            index+=start;
             i--;
             k = 0;
        }
         else
         {
             
             result[index]=key[i];
             index+=1;
         }
        
     }
     result[index] ='"';
     //result[index++] ='\n';
     index+=1;
     result[index] ='\0';
    
     rkey = malloc(strlen(result)+1);
     strcat (rkey,result);
    return rkey;
}



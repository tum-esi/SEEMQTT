#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <time.h> 


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <regex.h>

#include "config.h"



//#define SRV_PORT 3333
#define TRUE 1
#define QLEN 20
#define MAXVAR  10  



int varpair = 0; 
char * varlist[MAXVAR] ;
 
char* getPolicy();

char* ReadShare();


int ksid; 


void creatfile(char * filename , char * buf, int len)
{        
	
        FILE * fptr;
        fptr = fopen(filename, "w"); // "w" defines "writing mode"
        for (int i = 0; i<len; i++) {
            /* write to file using fputc() function */
            fputc(buf[i], fptr);
        }
        fclose(fptr);
        return 0;

}


     
void Evaluate(int msgsock)
{
 int msgsize =0; 
      int rs = read ( msgsock, &msgsize,sizeof(int));

      printf(" received Messages length = %d \n" ,msgsize); 
      
      char  * puf = (char *) malloc(msgsize); 
      memset(puf, '0', msgsize);
      if(puf == NULL)
      {
        printf(stderr, " can not create a buf for recv the messages\n"); 
        return ; 
    }
    
    int left =msgsize - sizeof(int); 
    int rcv = 0; 
    while (left >0)
    {
              
      int rs = read (msgsock, puf+rcv,left);
      left -=rs;
      rcv +=rs;
    }
    
      int cr1_len ;
      memcpy (&cr1_len, puf, sizeof(int)); 
      char * cr1  = (char *) malloc (cr1_len); 
      memcpy (cr1, puf + sizeof(int), cr1_len); 

     creatfile("cred1", cr1,cr1_len);
      
      int cr2_len ;
     memcpy (&cr2_len, puf + sizeof(int)+ cr1_len, sizeof(int)); 
      char * cr2 = (char *) malloc (cr2_len); 
       memcpy (cr2, puf + sizeof(int)+ cr1_len + sizeof(int), cr2_len);
       

      creatfile("cred2", cr2,cr2_len);
      int cr_l  = cr1_len+cr2_len;
      
      int pk_l ; 
      memcpy (&pk_l, puf + 2* sizeof(int)+ cr_l, sizeof(int)); 
       
     
      
      char * PK_authorizer = (char *) malloc (pk_l); 
      memcpy (PK_authorizer,  puf + 2* sizeof(int)+ cr_l + sizeof(int), pk_l); 
      
       creatfile("sub", PK_authorizer,pk_l);
      
      int asrt_l ; 
      memcpy (&asrt_l,  puf + 2* sizeof(int)+ cr_l + sizeof(int)+ pk_l, sizeof(int)); 
      


      
      char * asrt = (char *) malloc(asrt_l);
      memcpy (asrt,  puf + 2* sizeof(int)+ cr_l + sizeof(int)+ pk_l +sizeof(int), asrt_l); 
        
      creatfile("assrt", asrt,asrt_l);


 
  	/* 
  	 * Execute by passing the name directly 
  	 */

	char *  output [12] = {'\0'}; 
    sprintf(output, "output%d.txt",msgsock); 
    // char * output = "tmpoutput.txt";  
	char * cmdkn = "../KeyNote/keynote verify  -r unauthorixed,authorized -e assrt -l Policy -k sub cred1 cred2 >>";
	char* CMD = (char *)malloc(strlen(cmdkn)+strlen(output)+1);
    memset(CMD,'\0', strlen(cmdkn)+strlen(output)+1); 
	strncpy(CMD, cmdkn,  strlen(cmdkn)); 
	strncpy(CMD+ strlen(cmdkn), output, strlen(output)); 

  	system (CMD);
	int result = IsAuthorized(output); 
    #ifdef DBG
        printf(" result = %d \n", result ); 
    #endif

	if (result == 0)
	{
        char * share = ReadShare(); 
     
        int w = write(msgsock, share, strlen(share));
        #ifdef DBG
            printf("DEBUG: share %s\n",share); 
            printf("DEBUG:  %d sent to the requester\n", w); 
        #endif
        //int w = write(msgsock, &result, sizeof(result));
	}
	else 
	{
         int sharlen = 39 ; 
         char share[sharlen]; 
         memset(share, '0', sharlen); 
         int w = write(msgsock, share, strlen(share));
        #ifdef DBG
            printf("DEBUG: share %s\n",share); 
            printf("DEBUG:  %d sent to the requester\n", w); 
        #endif
	}
	
	//clean 
	remove(output); 
 
  	return 0; 
  
}

char* ReadShare()
{
     char name[5];
    sprintf(name, "/%d.txt", ksid);
    char path [124]={'\0'}; 
    strcpy(path, SHARPATH);
    strcat(path,name); 
    FILE *fptr;
    fptr = fopen(path,"r");
    if (fptr == NULL)
    {
        #ifdef DBG
        printf(" can not find file: %s \n", path); 
        #endif
    }

    char * line ;
    int len = 0 ;
    int read = getline(&line, &len, fptr);
    if (read != -1)
    {
        //printf("Retrieved line of length %zu :\n", read);
        //printf("%s", line);
    }

    char * token = strtok(line, "|");

    int index = 0 ;
    for (; index<3;index++)
        token = strtok(NULL, "|");

    char *  strshares = (char *) malloc (strlen(token)+1);
    memset(strshares, '\0', strlen(token)+1) ;
    sprintf(strshares , "%s\n", token);
    // printf  ("%s", strshares);
    free(line);
    fclose(fptr);
    
    return strshares;

}



/*
Check the  result of credentials evaluation. 
The result will be either 1 ( aothorized) and (0) for non authorized. 
*/

int IsAuthorized(char *output)
{
    FILE *fp;
    char *result;
    fp= fopen(output,"r");
    if (fp == NULL)
    {
        fprintf(stderr, "output file is missing ");
        exit(1);
    }
    fseek(fp,0,SEEK_END);
    int pol_Size = ftell(fp);
    rewind(fp);
    result = malloc(pol_Size+1);
    fread(result,pol_Size,1,fp);
    fclose(fp);
    result[pol_Size]='\0'; 
    char * token = strtok(result, "=");
    token = strtok(NULL, "=");
    #ifdef DBG
        printf("<token==%s>\n", token);
    #endif
    
    //if (strncmp(token, "authorized",strlen("authorized"))==0)
    if (memcmp(" authorized\n",token, strlen(" authorized\n"))==0)  
        return 0; 
    else 
        return -1; 
}
/* 
* getPolicy: Read the local policy" 
*/ 
char* getPolicy()
{
    FILE *fp;
    long pol_Size;
    char *result;
    fp= fopen("Policy","r");
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
    result[pol_Size]='\0';
    return result;
}


int
main(int argc, char **argv)
{
      int sock, length;
      struct sockaddr_in server;
      int msgsock;
      int i;

	  /* Get KS number */
	  ksid = atoi(argv[1]);
      
      /* Get the port number */
      int SRV_PORT = atoi(argv[2]); 
 
      /* Create socket */
      sock = socket(AF_INET, SOCK_STREAM, 0);
      if (sock < 0) {
            perror("opening stream socket");
            exit(1);
      }

      /* Name socket using wildcards */
      server.sin_family = AF_INET;
      server.sin_addr.s_addr = INADDR_ANY;
      server.sin_port = htons(SRV_PORT);
      if (bind(sock, (struct sockaddr *)&server, sizeof(server))) {
            perror("binding stream socket");
            exit(1);
      }

      /* Find out assigned port number and print it out */
      length = sizeof(server);
      if (getsockname(sock, (struct sockaddr *)&server, &length)) {
            perror("getting socket name");
            exit(1);
      }

      printf("Socket has port #%d\n", ntohs(server.sin_port));

      /* Start accepting connections */
      listen(sock, 5);
      do {
		msgsock = accept(sock, 0, 0);
		if (msgsock == -1)
			perror("accept");
		Evaluate(msgsock);
	        close(msgsock);
      } while (TRUE);
/*
 * Since this program has an infinite loop, the socket "sock" is
 * never explicitly closed.  However, all sockets will be closed
 * automatically when a process is killed or terminates normally.
 */
}


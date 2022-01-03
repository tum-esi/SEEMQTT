#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 



#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vp_msg.h"

#include <regex.h>






#define TRUE 1
#define QLEN 20
#define MAXVAR  10  


int varpair = 0; 
char * varlist[MAXVAR] ; 

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

char* getPolicy();



void sigverusage(void);
void print_region(FILE *fd_out, char *vp_msg, char *vp_s, char *vp_e);
char* getAuthKey(char *vp_s, char *vp_e);


     
void process(int msgsock)
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
  system ("../keynote verify  -r unauthorixed,authorized -e assrt -l Policy -k sub_pub cred1 cred2");
   
  return 0; 
  
}



int
main(int argc, char **argv)
{
      int sock, length;
      struct sockaddr_in server;
      int msgsock;
      int i;

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
		process(msgsock);
	        close(msgsock);
      } while (TRUE);
/*
 * Since this program has an infinite loop, the socket "sock" is
 * never explicitly closed.  However, all sockets will be closed
 * automatically when a process is killed or terminates normally.
 */
}
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



void sigverusage(void)
{
    fprintf(stderr, "Arguments:\n");
    fprintf(stderr, "\t<AssertionFile>\n");
}





/*
 * print_region -- print a range of characters located inside a string
 *	Routine receives the file descriptor that it should print its
 *	output, an informative message to print before the data, and
 *	beginning and end pointers for the data to be printed.
 */

void print_region(FILE *fd_out, char *vp_msg, char *vp_s, char *vp_e)
{
    char c;
    
    if ((vp_s == NULL) || (vp_e ==NULL) || (vp_s == vp_e))
    {
        printf("%s = <<NULL>>\n", vp_msg);
        return;
    }
    c = *vp_e;	// save char at end of string
    *vp_e = '\0';	// zero terminate string
    fprintf(fd_out, "%s = <<%s>>\n", vp_msg, vp_s);
    *vp_e = c;	// restore value that was zeroed
}
char* getAuthKey( char *vp_s, char *vp_e)
{
    char c;
    char result[30000];
    if ((vp_s == NULL) || (vp_e ==NULL) || (vp_s == vp_e))
    {
        printf("auth key = <<NULL>>\n");
        return NULL;
    }
    int i=0;
    while (vp_s != vp_e)
    {
        c = *vp_s;
        
        if(c=='\\' ||c=='\n' || c=='"' || c==' ')
        {
            
        }
        else
        {
            result[i]=c;
            i++;
        }
        vp_s++;
        
    }
    return result;
}


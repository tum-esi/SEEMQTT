#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vp_msg.h"

#include <regex.h>
#include "../keynote.h"
#include "../assertion.h"





#define TRUE 1
#define QLEN 20
#define MAXVAR  10  


int varpair = 0; 
char * varlist[MAXVAR] ; 
void parsevars(char * buf , int len)
{
    
   #ifdef DBUG
        mhh_printchar(buf,len);
   #endif
 
    int index = 0;
    int str = index;  
    while ( index <len)
    {
        if (buf[index] =='\n'|| buf[index]== '=')
        {
        int end  =  index;
          #ifdef DBUG 
            printf(" end = %d, str = %d \n", end, str); 
            mhh_printchar (buf+str, end-str);
           #endif
        int l = end-str;
        varlist[varpair]  =  (char *) malloc (l+1);
        memset( varlist[varpair] ,'\0',l+1);
        strncpy (  varlist[varpair],buf+str,  l); 
        varpair++; 
        index++; 
        str = index; 
         }
        else 
            index++; 
     }
}

char* getPolicy();



void sigverusage(void);
void print_region(FILE *fd_out, char *vp_msg, char *vp_s, char *vp_e);
char* getAuthKey(char *vp_s, char *vp_e);

void
process_message(int msgsock)
{
	FILE * fd;
   

	int plen;
	int rval;
	char c;
   
      
    char **decomposed;
    struct assertion *qass;
    int n,assert_index , assert_r, sessionid;
         
    // init a new KeyNote session here.
    
    sessionid = kn_init();
    if (sessionid == -1)
    {
        fprintf(stderr, "Failed to create a new session.\n");
        exit(-1);
    }  
    
    
    // Read and process the policy
    char *policyBuffer;
    policyBuffer = getPolicy();
    decomposed = kn_read_asserts(policyBuffer, strlen(policyBuffer),
                                 &n);
    if (decomposed == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for policy assertions.\n");
        exit(-1);
    }
    if (n == 0)
    {
        free(decomposed);
        fprintf(stderr, "No policy assertions provided.\n");
        exit(-1);
    }
    
    for (assert_index = 0; assert_index< n; assert_index++)
    {
        printf("********** kn_add_assertion( \n %s, %d \n)\n", decomposed[assert_index], strlen(decomposed[assert_index]));
        
        assert_r = kn_add_assertion(sessionid, decomposed[assert_index],
                                    strlen(decomposed[assert_index]), ASSERT_FLAG_LOCAL);
        if (assert_r == -1)
        {
            switch (keynote_errno)
            {
                case ERROR_MEMORY:
                    fprintf(stderr, "Out of memory, trying to add policy "
                            "assertion %d.\n", assert_r);
                    break;
                    
                case ERROR_SYNTAX:
                    fprintf(stderr, "Syntax error parsing policy "
                            "assertion %d.\n", assert_r);
                    break;
                    
                case ERROR_NOTFOUND:
                    fprintf(stderr, "Session %d not found while adding "
                            "policy assertion %d.\n", sessionid, assert_r);
                default:
                    fprintf(stderr, "Unspecified error %d (shouldn't happen) "
                            "while adding policy assertion %d.\n",
                            keynote_errno, assert_r);
                    break;
            }
            /* We don't need the assertion any more. */
            free(decomposed[assert_index]);
        }
    }
    free(decomposed);
    memset(policyBuffer,0,strlen(policyBuffer));
    
    

     /* 
      * Recive the msg 
      */
      
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
    
    /*
      
      int cr_l ; 
      memcpy (&cr_l, puf, sizeof(int)); 
      
      char* cre  = (char *) malloc (cr_l); 
      memcpy (cre, puf + sizeof(int), cr_l); 
      */
      
      int cr1_len ;
      memcpy (&cr1_len, puf, sizeof(int)); 
      char * cr1  = (char *) malloc (cr1_len); 
      memcpy (cr1, puf + sizeof(int), cr1_len); 
      
      int cr2_len ;
     memcpy (&cr2_len, puf + sizeof(int)+ cr1_len, sizeof(int)); 
      char * cr2 = (char *) malloc (cr2_len); 
       memcpy (cr2, puf + sizeof(int)+ cr1_len + sizeof(int), cr2_len);
       
      int cr_l  = cr1_len+cr2_len;
      
      int pk_l ; 
      memcpy (&pk_l, puf + 2* sizeof(int)+ cr_l, sizeof(int)); 
       
     
      
      char * PK_authorizer = (char *) malloc (pk_l); 
      memcpy (PK_authorizer,  puf + 2* sizeof(int)+ cr_l + sizeof(int), pk_l); 
      
      
      int asrt_l ; 
      memcpy (&asrt_l,  puf + 2* sizeof(int)+ cr_l + sizeof(int)+ pk_l, sizeof(int)); 
      
      
      char * asrt = (char *) malloc(asrt_l);
      memcpy (asrt,  puf + 2* sizeof(int)+ cr_l + sizeof(int)+ pk_l +sizeof(int), asrt_l); 
        
        
    mhh_printchar(cr1,cr1_len);
    int p = kn_add_assertion(sessionid, cr1, cr1_len, 0);
	if (p == -1)
	{
	    fprintf(stderr, "Error for assertion in file , errno %d.\n",
		    keynote_errno);
	    keynote_errno = 0;
	}
   
     mhh_printchar(cr2,cr2_len);
     int p1 = kn_add_assertion(sessionid, cr2, cr2_len, 0);
	if (p1 == -1)
	{
	    fprintf(stderr, "Error for assertion in file , errno %d.\n",
		    keynote_errno);
	    keynote_errno = 0;
	}
   
    
    
    
     /*
     * Now add the action authorizer. If we have more than one, just
     * repeat. Note that the value returned here is just a success or
     * failure indicator. If we want to later on delete an authorizer from
     * the session (which we MUST do if this is a persistent session),
     * we must keep a copy of the key.
     */
     
    printf(" authoriztion key : \n" ); 
    mhh_printchar(  PK_authorizer,pk_l);
     
    if (kn_add_authorizer(sessionid, PK_authorizer) == -1)
    {
	switch (keynote_errno)
	{
	    case ERROR_MEMORY:
		fprintf(stderr, "Out of memory while adding action "
			"authorizer.\n");
		break;

	    case ERROR_SYNTAX:
		fprintf(stderr, "Malformed action authorizer.\n");
		break;

	    case ERROR_NOTFOUND:
		fprintf(stderr, "Session %d not found while adding action "
			"authorizer.\n", sessionid);
		break;

	    default:
		fprintf(stderr, "Unspecified error while adding action "
			"authorizer.\n");
		break;
	}
    }



    /*
     * Add the relevant action attributes. Flags is zero, since we are not
     * using any callback functions (ENVIRONMENT_FLAG_FUNC) or a regular
     * expression for action attribute names (ENVIRONMENT_FLAG_REGEX).
     */
     
     
    char * fasst = "ass.txt"; 
    if (read_environment(fasst) == -1)
	 	  exit(-1);
     /*
     if (kn_add_action(sessionid, "topic", "t1", 0) == -1)
        fprintf(stderr, "Unspecified error %d (shouldn't happen) "
			"while adding action attribute \n", keynote_errno);
      
      if (kn_add_action(sessionid, "location", "l1", 0) == -1)
        fprintf(stderr, "Unspecified error %d (shouldn't happen) "
			"while adding action attribute \n", keynote_errno);
      
   parsevars(asrt, asrt_l);
   printf("varpair = %d \n", varpair);
   
   for (int  index = 0 ; index <varpair ; index+=2)
   {
       
       printf   ( "%d = <<%s>>        %d =<<%s>>\n",  index,varlist[index], index+1, varlist[index+1]);
       if (kn_add_action(sessionid, varlist[index], varlist[index+1], 0) == -1)
        {
            switch (keynote_errno)
            {
                case ERROR_SYNTAX:
                fprintf(stderr, "Invalid name action attribute name "
			"[app_domain]\n");
                break;

                case ERROR_MEMORY:
                fprintf(stderr, "Out of memory adding action attribute "
			"[app_domain = \"test application\"]\n");
                break;

                case ERROR_NOTFOUND:
                fprintf(stderr, "Session %d not found while addiing action "
			"attribute [app_domain = \"test application\"]\n",
			sessionid);
                break;

                default:
                fprintf(stderr, "Unspecified error %d (shouldn't happen) "
			"while adding action attribute [app_domain = "
			"\"test application\"]\n", keynote_errno);
                break;
            }
        }
       
    }
    
    
    */
    
    
     /*
    if (kn_add_action(sessionid, "app_domain", "CocaCola", 0) == -1)
    {
	switch (keynote_errno)
	{
	    case ERROR_SYNTAX:
		fprintf(stderr, "Invalid name action attribute name "
			"[app_domain]\n");
		break;

	    case ERROR_MEMORY:
		fprintf(stderr, "Out of memory adding action attribute "
			"[app_domain = \"test application\"]\n");
		break;

	    case ERROR_NOTFOUND:
		fprintf(stderr, "Session %d not found while addiing action "
			"attribute [app_domain = \"test application\"]\n",
			sessionid);
		break;

	    default:
		fprintf(stderr, "Unspecified error %d (shouldn't happen) "
			"while adding action attribute [app_domain = "
			"\"test application\"]\n", keynote_errno);
		break;
	}
    }

    if (kn_add_action(sessionid, "count", "1", 0) == -1)
    {
        switch (keynote_errno)
        {
            case ERROR_SYNTAX:
                fprintf(stderr, "Invalid name action attribute name "
			"[some_num]\n");
                break;

            case ERROR_MEMORY:
                fprintf(stderr, "Out of memory adding action attribute "
			"[some_num = \"1\"]\n");
                break;

            case ERROR_NOTFOUND:
                fprintf(stderr, "Session %d not found while addiing action "
			"attribute [some_num = \"1\"]\n", sessionid);
                break;

            default:
                fprintf(stderr, "Unspecified error %d (shouldn't happen) "
			"while adding action attribute [some_num = \"1\"]",
			keynote_errno);
            break;
        }
    }

    if (kn_add_action(sessionid, "price", "4", 0) == -1)
    {
        switch (keynote_errno)
        {
            case ERROR_SYNTAX:
                fprintf(stderr, "Invalid name action attribute name "
			"[some_num]\n");
                break;

            case ERROR_MEMORY:
                fprintf(stderr, "Out of memory adding action attribute "
			"[some_num = \"1\"]\n");
                break;

            case ERROR_NOTFOUND:
                fprintf(stderr, "Session %d not found while addiing action "
			"attribute [some_num = \"1\"]\n", sessionid);
                break;

            default:
                fprintf(stderr, "Unspecified error %d (shouldn't happen) "
			"while adding action attribute [some_num = \"1\"]",
			keynote_errno);
            break;
        }
    } 
    
    */    
        
    // make decision
   printf(" Here before"); 
    char *returnval[2]= {"unauthorized","authorized"};
    //returnvalues[0] = "unauthorized";
    //returnvalues[1] = "authorized";
     printf(" Here before"); 
    
    assert_r = kn_do_query(sessionid, returnval, 2);
    
    printf(" Here after"); 
    if (assert_r == -1)
    {
        switch (assert_r)
        {
            case ERROR_MEMORY:
                fprintf(stderr, "Out of memory while performing authorization "
                        "query.\n");
                break;
                
            case ERROR_NOTFOUND:
                fprintf(stderr, "Session %d not found while performing "
                        "authorization query.\n", sessionid);
                break;
                
            default:
                fprintf(stderr, "Unspecified error %d (shouldn't happen) "
                        "while performing authorization query.\n",
                        keynote_errno);
                break;
        }
    }
    else
    {
        // send decision packet
       
        char DECISION_STR[2000]="Return value is ";
        strcat(DECISION_STR,returnval[assert_r]);
        int result = write(msgsock, DECISION_STR, strlen(DECISION_STR));
        printf( "D%06d:%s\n", strlen(DECISION_STR), DECISION_STR);
        //fflush(fd);
       
    }    
    kn_close(sessionid);
	return;
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
		process_message(msgsock);
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


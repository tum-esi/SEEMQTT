#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vp_msg.h"


void mhh_printchar(char * buf, int len )

{
	
	printf(" --------------------------------------------\n");
	for (int i = 0; i<len ; i++)
		printf("%c", buf[i]); 
	printf("\n--------------------------------------------\n");
}
/*
 * get_buf -- read exactly len bytes into buffer buf
 *	from file descriptor fd
 */
static int
get_buf(FILE *fd, char *buf, int len)
{
	int rem;
	int rval = 0;
	int max_read = 10;

	for (rem = len; rem > 0; )
	{
		if ((rval = fread(buf, 1, rem, fd)) < 0)
			return(rval);
fprintf(stderr, "asked for %d, got %d bytes\n", rem, rval);
		buf += rval;
		rem -= rval;
		if (max_read-- <= 0) {
			fprintf(stderr, "get_buf: too many reads\n");
			return(len-rem);
		}
	}
	return(len);
}


/*
 * read_message -- read message sent over file descriptor fd
 *	The message has the format outlined in vp_hdr.h
 *	The caller specifies which message types she wants
 *	by placing the relevant ids in msg_ids (e.g. "HDO"
 *	means that we are interested in HELLO, DECISION and
 *	OFFER messages only.
 *	If the message being received is not of the ids
 *	specidied, reception is aborted and read_message
 *	returns an error.
 *
 *	If the message is read in its entirety, pbuf
 *	contains the payload, plen the length, and the
 *	return value is the actuall message if received.
 *
 *	Since pbuf is allocated here, it must be freed by
 *	the caller.
 */
int
read_message(FILE *fd, char *msg_ids, char **pbuf_p, int *plen_p)
{
	char c;
	char cmd;
	int rval;
	char *pbuf;
	int plen;

	if ((cmd=getc(fd)) < 0) {
		perror("read_message: getc");
		return(MSG_CMD_ERROR);
	}

	// here we only look at messages that have their ids in msg_ids
	if (strchr(msg_ids, cmd) == NULL) {
		fprintf(stderr, "process_message: invalid message id '%c'\n", cmd);
		return(MSG_CMD_ERROR);
	}
	// read message length
	for (plen = 0; isdigit((c=getc(fd))); )
		plen = plen *10 + (c - '0');
	if (c != ':') {
		fprintf(stderr, "process_message: sync error, expected ':', got '%c'\n", c);
		return 1;
	}
    printf(" ************ here ************"); 
	// get message
	if ((pbuf = malloc(plen+1)) == NULL) // need one extra byte for the EOS
		perror("calloc failed");
	if ((rval = get_buf(fd, pbuf, plen)) < 0)
		perror("reading stream message");

	pbuf[plen] = '\0';
	*pbuf_p = pbuf;
	*plen_p = plen;
	return(cmd);
}

	


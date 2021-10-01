
#define SRV_PORT	3333

/*
 * read in message: each message is of the form Cnnn:mmm
 *      where C is a one character message indentifier, nnnnn is
 *      an unsigned integer denoting payload lenght, and mmm
 *      is the payload.
 *      Current message indentifiers are:
 *              R       request: request, followed by
 *                              at least one credentials
 *              C(c)    credentials: credentials authorising the
 *                              sale. The last credential uses 'C'
 *                              while the one in the middle use 'c'.
 *              K       publick key of the issuer
 *              D       decision: outcome (essentially yes/no)
 */

#define MSG_CMD_ERROR   -1
#define MSG_CMD_REQUEST 'R'
#define MSG_CMD_CRED    'c'
#define MSG_CMD_CRED_L  'C'
#define MSG_CMD_PK      'K'
#define MSG_CMD_DECIS   'D'

#define DBUG 1

int read_message(FILE *fd, char *msg_ids, char **pbuf_p, int *plen_p);

void mhh_printchar(char * buf, int len);

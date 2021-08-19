#ifndef _KEYNOTE_ESP_H
#define _KEYNOTE_ESP_H

#ifdef __cplusplus
extern "C" {
#endif
#define  KN_SIGN_SZ 364

int kn_encode_hex(unsigned char *buf, char **dest, int len);
char *   kn_encode_signture( char *algname ,  char *key) ;
char *  kn_rsa_sign_md5(const unsigned char * input, int len, const unsigned char * iot_pr_key, int keylen,   unsigned char * sign);

#ifdef __cplusplus
}
#endif
#endif /*_KEYNOTE_ESP_H*/

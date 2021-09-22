#ifndef _SHAMIRSECRETSHARE_H
#define _SHAMIRSECRETSHARE_H

#include "stddef.h"
#include "secconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

int modular_exponentiation(int base, int exp, int mod);
int *split_number(int number, int n, int t);
int *gcdD(int a, int b);
int modInverse(int k);
int join_shares(int *xy_pairs, int n);
char **split_string(char *secret, int n, int t);
void free_string_shares(char ** shares, int n);
char *join_strings(char ** shares, int n);
char *generate_share_strings(char * secret, int n, int t);
void trim_trailing_whitespace(char * str);
char * strtok_rr( char * str, const char * delim, char ** nextp);
char * extract_secret_from_share_strings(const char * string);

#ifdef __cplusplus
}
#endif

#endif /* End of _SHAMIRSECRETSHARE_H */

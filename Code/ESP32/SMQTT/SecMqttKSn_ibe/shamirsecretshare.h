/*
     shamirsecretshare.c -- Shamir's Secret Sharing
     Adopted from the implementaion of shamir's secret sharing of Fletcher T. Penney:
     http://fletcher.github.io/c-sss/index.html
     ## The MIT License ##
     Permission is hereby granted, free of charge, to any person obtaining a copy
     of this software and associated documentation files (the "Software"), to deal
     in the Software without restriction, including without limitation the rights
     to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
     copies of the Software, and to permit persons to whom the Software is
     furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
    */

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

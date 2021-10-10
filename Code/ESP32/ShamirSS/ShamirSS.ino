#include <pbc.h>


unsigned int endt;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

 //int n = 5; /* Maximum n = 255 */
 // int t = 3; 
 

  
  char  Key [17]; 
  esp_fill_random(Key, 16);
  Key[16] ='\0'; 
  Serial.print(" Secret is : "); 
  //Serial.println(Key);
  dbgPrint ((unsigned char *)Key, 0 , 16);
  

  Serial.println("id\tn\tt\tSplit\t\Combine"); 

// for (int n = 2; n<16; n++)
 //{
    /* n, MAx n = 255 */
    int n = 14; 
    /* t */
  for (int t= 2; t<=n; t++)
   { 
      //int  t= n; 
      for (int id = 0 ; id <100 ; id++)
       {
        unsigned long start_s = micros();
        char * shares = generate_share_strings(Key, n, t);
        unsigned long end_s = micros();
        

        
        int lenshares = strlen(shares); 
        //Serial.printf("length is \n", lenshares);  
        //Serial.printf("<<\n%s>>\n", shares);  
        
        int len  = lenshares /n ; 
        char * colectedshare = (char *)malloc (t* len +1);
        memset(colectedshare, '\0', t* len +1); 
        memcpy(colectedshare, shares, t*len); 
       // colectedshare[t* len +1] ='\0'; 
       // Serial.printf("<<\n%s>>\n", colectedshare); 


        unsigned long start_j = micros();
        char * extracted = extract_secret_from_share_strings (colectedshare, t);
        unsigned long end_j = micros();

        unsigned long start_j1 = micros();
        char * extracted2 = extract_secret_from_share_strings (shares, t);
        unsigned long end_j1 = micros();
        
        if (strncmp(extracted, Key, 16)==0)
          Serial.printf("%d\t%d\t%d\t%d\t\%d\t\%d\t\%d\n", id, n , t, end_s - start_s, end_j -start_j,end_j1 -start_j1,  endt); 
       
       
       free(shares);
       free (extracted);
       free (extracted2);
       free (colectedshare);
       
 //}
 } 
  }
}

void loop() {

}
static int prime = 257;

/*
 * Print array
 * size: the size of the array
 * beg: index to start printing
 */
void dbgPrint(uint8_t * arr, int beg ,int size) {
  Serial.println();
  int start = beg; //defualt = 0
  int row = 0 ;
  Serial.print(row);
  Serial.print(" >> ");
  for (int i = start; i < size; i++)
  {
     if(i > 0 && i % 16 == 0)
     {
      row++;
      Serial.println();
      Serial.print(row);
      Serial.print(" >> ");
     }
    Serial.print(arr[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}
unsigned long mix(unsigned long a, unsigned long b, unsigned long c) {
  a = a - b;
  a = a - c;
  a = a ^ (c >> 13);
  b = b - c;
  b = b - a;
  b = b ^ (a << 8);
  c = c - a;
  c = c - b;
  c = c ^ (b >> 13);
  a = a - b;
  a = a - c;
  a = a ^ (c >> 12);
  b = b - c;
  b = b - a;
  b = b ^ (a << 16);
  c = c - a;
  c = c - b;
  c = c ^ (b >> 5);
  a = a - b;
  a = a - c;
  a = a ^ (c >> 3);
  b = b - c;
  b = b - a;
  b = b ^ (a << 10);
  c = c - a;
  c = c - b;
  c = c ^ (b >> 15);
  return c;
}

void seed_random(void) {
  unsigned long seed = mix(clock(), time(NULL), getpid());
  srand(seed);
}
int modular_exponentiation(int base, int exp, int mod) {
  if (exp == 0) {
    return 1;
  } else if (exp % 2 == 0) {
    int mysqrt = modular_exponentiation(base, exp / 2, mod);
    return (mysqrt * mysqrt) % mod;
  } else {
    return (base * modular_exponentiation(base, exp - 1, mod)) % mod;
  }
}

/*
  split_number() -- Split a number into shares
  n = the number of shares
  t = threshold shares to recreate the number
*/

int * split_number(int number, int n, int t) {
  int * shares = (int *) malloc(sizeof(int) * n);

  int * coef =  (int *)malloc(sizeof(int) * t);
  int x;
  int i;

  coef[0] = number;

  for (i = 1; i < t; ++i) {
    /* Generate random coefficients -- use arc4random if available */
#ifdef HAVE_ARC4RANDOM
    coef[i] = arc4random_uniform(prime);
#else
    coef[i] = rand() % (prime);
#endif
  }

  for (x = 0; x < n; ++x) {
    int y = coef[0];

    /* Calculate the shares */
    for (i = 1; i < t; ++i) {
      int temp = modular_exponentiation(x + 1, i, prime);

      y = (y + (coef[i] * temp % prime)) % prime;
    }

    /* Sometimes we're getting negative numbers, and need to fix that */
    y = (y + prime) % prime;

    shares[x] = y;
  }

  free(coef);

  return shares;
}
int * gcdD(int a, int b) {
  int * xyz = (int *)malloc(sizeof(int) * 3);

  if (b == 0) {
    xyz[0] = a;
    xyz[1] = 1;
    xyz[2] = 0;
  } else {
    int n = floor(a / b);
    int c = a % b;
    int * r = gcdD(b, c);

    xyz[0] = r[0];
    xyz[1] = r[2];
    xyz[2] = r[1] - r[2] * n;

    free(r);
  }

  return xyz;
}

int modInverse(int k) {
  k = k % prime;

  int r;
  int * xyz;

  if (k < 0) {
    xyz = gcdD(prime, -k);
    r = -xyz[2];
  } else {
    xyz = gcdD(prime, k);
    r = xyz[2];
  }

  free(xyz);

  return (prime + r) % prime;
}

/*
  join_shares() -- join some shares to retrieve the secret
  xy_pairs is array of int pairs, first is x, second is y
  n is number of pairs submitted
*/

int join_shares(int * xy_pairs, int n) {

  unsigned long  start = micros(); 
  int secret = 0;
  long numerator;
  long denominator;
  long startposition;
  long nextposition;
  long value;
  int i;
  int j;

  // Pairwise calculations between all shares
  for (i = 0; i < n; ++i) {
    numerator = 1;
    denominator = 1;

    for (j = 0; j < n; ++j) {
      if (i != j) {
        startposition = xy_pairs[i * 2];    // x for share i
        nextposition = xy_pairs[j * 2];   // x for share j
        numerator = (numerator * -nextposition) % prime;
        denominator = (denominator * (startposition - nextposition)) % prime;
      }
    }

    value = xy_pairs[i * 2 + 1];
    secret = (secret + (value * numerator * modInverse(denominator))) % prime;

  }

  /* Sometimes we're getting negative numbers, and need to fix that */
  secret = (secret + prime) % prime;

  endt =  micros() - start; 
  return secret;
}

/*
  split_string() -- Divide a string into shares
  return an array of pointers to strings;
*/

char ** split_string(char * secret, int n, int t) {
  int len = strlen(secret);

  char ** shares = (char **) malloc (sizeof(char *) * n);
  int i;

  for (i = 0; i < n; ++i) {
    /* need two characters to encode each character */
    /* Need 4 character overhead for share # and quorum # */
    /* Additional 2 characters are for compatibility with:
      http://www.christophedavid.org/w/c/w.php/Calculators/ShamirSecretSharing
    */
    shares[i] = (char *) malloc(2 * len + 6 + 1);

    sprintf(shares[i], "%02X%02XAA", (i + 1), t);
  }

  /* Now, handle the secret */

  for (i = 0; i < len; ++i) {
    int letter = secret[i]; // - '0';

    if (letter < 0) {
      letter = 256 + letter;
    }

    int * chunks = split_number(letter, n, t);
    int j;

    for (j = 0; j < n; ++j) {
      if (chunks[j] == 256) {
        sprintf(shares[j] + 6 + i * 2, "G0"); /* Fake code */
      } else {
        sprintf(shares[j] + 6 + i * 2, "%02X", chunks[j]);
      }
    }

    free(chunks);
  }

  return shares;
}


void free_string_shares(char ** shares, int n) {
  int i;

  for (i = 0; i < n; ++i) {
    free(shares[i]);
  }

  free(shares);
}




char * join_strings(char ** shares, int n) {

  if ((n == 0) || (shares == NULL) || (shares[0] == NULL)) {
    return NULL;
  }

  // `len` = number of hex pair values in shares
  int len = (strlen(shares[0]) - 6) / 2;

  char * result = (char *)malloc(len + 1);
  char codon[3];
  codon[2] = '\0';  // Must terminate the string!

  int x[n];   // Integer value array
  int i;      // Counter
  int j;      // Counter

  // Determine x value for each share
  for (i = 0; i < n; ++i) {
    if (shares[i] == NULL) {
      free(result);
      return NULL;
    }

    codon[0] = shares[i][0];
    codon[1] = shares[i][1];

    x[i] = strtol(codon, NULL, 16);
  }

  // Iterate through characters and calculate original secret
  for (i = 0; i < len; ++i) {
    int * chunks = (int *) malloc(sizeof(int) * n  * 2);

    // Collect all shares for character i
    for (j = 0; j < n; ++j) {
      // Store x value for share
      chunks[j * 2] = x[j];

      codon[0] = shares[j][6 + i * 2];
      codon[1] = shares[j][6 + i * 2 + 1];

      // Store y value for share
      if (memcmp(codon, "G0", 2) == 0) {
        chunks[j * 2 + 1] = 256;
      } else {
        chunks[j * 2 + 1] = strtol(codon, NULL, 16);
      }
    }

    //unsigned char letter = join_shares(chunks, n);
    char letter = join_shares(chunks, n);

    free(chunks);

    sprintf(result + i, "%c", letter);
  }

  return result;
}
char * generate_share_strings(char * secret, int n, int t) {
  char ** result = (char **)split_string(secret, n, t);

  int len = strlen(secret);
  int key_len = 6 + 2 * len + 1;
  int i;

  char * shares = (char *)malloc(key_len * n + 1);

  for (i = 0; i < n; ++i) {
    sprintf(shares + i * key_len, "%s\n", result[i]);
  }
  //Serial.println(shares);
  free_string_shares(result, n);

  return shares;
}

/* Trim spaces at end of string */
void trim_trailing_whitespace(char * str) {
  unsigned long l;

  if (str == NULL) {
    return;
  }

  l = strlen(str);

  if (l < 1) {
    return;
  }

  while ( (l > 0) && (( str[l - 1] == ' ' ) ||
            ( str[l - 1] == '\n' ) ||
            ( str[l - 1] == '\r' ) ||
            ( str[l - 1] == '\t' )) ) {
    str[l - 1] = '\0';
    l = strlen(str);
  }
}
char * strtok_rr(
  char * str,
  const char * delim,
  char ** nextp) {
  char * ret;

  if (str == NULL) {
    str = *nextp;
  }

  if (str == NULL) {
    return NULL;
  }

  str += strspn(str, delim);

  if (*str == '\0') {
    return NULL;
  }

  ret = str;

  str += strcspn(str, delim);

  if (*str) {
    *str++ = '\0';
  }

  *nextp = str;

  return ret;
}
/*
  extract_secret_from_share_strings() -- get a raw string, tidy it up
    into individual shares, and then extract secret
*/

char * extract_secret_from_share_strings(const char * string, int t) {
  char ** shares = (char **) malloc(sizeof(char *) * 255);

  char * share;
  char * saveptr = NULL;
  int i = 0;

  /* strtok_rr modifies the string we are looking at, so make a temp copy */
  char * temp_string = (char *)strdup(string);

  /* Parse the string by line, remove trailing whitespace */
  share = strtok_rr(temp_string, "\n", &saveptr);

  shares[i] = strdup(share);
  trim_trailing_whitespace(shares[i]);

  while ( (share = strtok_rr(NULL, "\n", &saveptr))) {
    i++;

    shares[i] = strdup(share);

    trim_trailing_whitespace(shares[i]);

    if ((shares[i] != NULL) && (strlen(shares[i]) == 0)) {
   
      free(shares[i]);
      i--;
    }
  }

  i++;
  if (i<t)
  {
    Serial.printf("Error: the %d provided shares is leass than t (%d)\n" , i, t); 
    return NULL; 
  }

 // Serial.printf( " join_strings %d\n", i); 
  char * secret =(char *) join_strings(shares, t);

  free_string_shares(shares, i);
  free(temp_string);

  return secret;
}

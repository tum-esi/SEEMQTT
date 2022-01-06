#include "mbedtls/aes.h"



 


#define BLOCK_SIZE 16


uint8_t key[BLOCK_SIZE] = {0x1C,0x3E,0x4B,0xAF,0x13,0x4A,0x89,0xC3,0xF3,0x87,0x4F,0xBC,0xD7,0xF3, 0x31, 0x31 };


/*size of the message */
#define Size_Byte 1024


static uint8_t  *rand_string(uint8_t  *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXZY";
    if (size) {
        //--size;
        for (size_t n = 0; n < size; n++) {
            int key = esp_random() % 16;
            str[n] = key;
        }
    }
    return str;
}

uint8_t * rand_string_alloc(size_t size)
{
     uint8_t  *s =(uint8_t  *) malloc(size);
     if (s) {
         rand_string(s, size);
     }
     return s;
}

void bufferSize(char* text, int &length)
{
  int i = strlen(text);
  int buf = round(i / BLOCK_SIZE) * BLOCK_SIZE;
  length = (buf <= i) ? buf + BLOCK_SIZE : length = buf;
}
    
void Print(uint8_t * arr, int start, int size)
{
  Serial.println();
 // int start = size -32; //defualt = 0
  int row =0 ;
  Serial.print(row); 
  Serial.print(" >> ");  
  for (int i =start; i<size; i++)
  {
     if(i> 0 && i % BLOCK_SIZE ==0)
     {
      row++;
      Serial.println(); 
      Serial.print(row); 
      Serial.print(" >> ");
      
     }
    Serial.print(arr[i],HEX); 
    Serial.print(" "); 
   
  }
  Serial.println();
    
  }

void encryption(uint8_t * input, uint8_t * output)
{
  mbedtls_aes_context aes;
  uint8_t iv[BLOCK_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  

  Serial.println("<<<<<<<<<<<<<<<start Encryption>>>>>>>>>>>>>>>>>");
  Serial.println("Plain Text:");
  Print(input,Size_Byte -32, Size_Byte);

  //char *input1 = "Tech tutorials x";
  char * key1 = "abcdefghijklmnop";
  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_enc( &aes, (const unsigned char*) key, BLOCK_SIZE * 8 );
  mbedtls_aes_crypt_cbc(&aes,MBEDTLS_AES_ENCRYPT,Size_Byte,iv, input, output);
  Serial.println(" Encryptted text:");
  Print(output,Size_Byte -32,Size_Byte);

  Serial.println("<<<<<<<<<<<<<<<End Encryption>>>>>>>>>>>>>>>>>");
  mbedtls_aes_free( &aes );
 }

 void decryption(uint8_t * cipher, uint8_t * plaintext)
 {
  
   mbedtls_aes_context aes;
  uint8_t iv[BLOCK_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  

  Serial.println("<<<<<<<<<<<<<<<start Decryption>>>>>>>>>>>>>>>>>");
  Serial.println("Cipher Text:");
  Print(cipher,Size_Byte -32, Size_Byte);

  char * key1 = "abcdefghijklmnop";
  mbedtls_aes_init(&aes );
  mbedtls_aes_setkey_dec(&aes, (const unsigned char*) key, BLOCK_SIZE * 8 );
  mbedtls_aes_crypt_cbc(&aes,MBEDTLS_AES_DECRYPT,Size_Byte,iv, cipher, plaintext);
  Serial.println("Plain text:");
  Print(plaintext,Size_Byte -32,Size_Byte);

  Serial.println("<<<<<<<<<<<<<<<End Encryption>>>>>>>>>>>>>>>>>");
  mbedtls_aes_free( &aes );
  
  
  }


void setup() {
 
  Serial.begin(115200);
 
  uint8_t * input =rand_string_alloc(Size_Byte);
  unsigned char output[Size_Byte];
  unsigned char plaintext[Size_Byte];
  unsigned long senc= micros(); 
  encryption(input, output); 
  unsigned long eenc= micros(); 
  decryption(output,plaintext); 
  unsigned long edec= micros();
  Serial.print(" Time to encryption (us):"); 
  Serial.println( eenc -senc); 
  Serial.print(" Time to decryption (us):"); 
  Serial.println( edec -eenc); 

}
 
void loop() {}

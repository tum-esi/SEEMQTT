#include <ArduinoBearSSL.h>

/*
 * Test AES encrption and decryption 
 */



#define BLOCK_SIZE 16

char *plain_text;
uint8_t key[BLOCK_SIZE] = { 0x1C,0x3E,0x4B,0xAF,0x13,0x4A,0x89,0xC3,0xF3,0x87,0x4F,0xBC,0xD7,0xF3, 0x31, 0x31 };


/*size of the message */
#define Size_Byte 1024


static uint8_t  *rand_string(uint8_t  *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXZY";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % 16;
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
    


void encryption(uint8_t * pt)
{
  uint8_t iv[BLOCK_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  
 
  br_aes_big_cbcenc_keys    ctx ; 
  Serial.println("<<<<<<<<<<<<<<<start Encryption>>>>>>>>>>>>>>>>>");
  Serial.println(" Plain Text");
  Print(pt,Size_Byte);

  br_aes_big_cbcenc_init(&ctx, key,BLOCK_SIZE ); 

  br_aes_big_cbcenc_run(&ctx,iv,  pt, Size_Byte);

  Serial.println(" Encryptted text:");
  Print(pt,Size_Byte);


  Serial.println("<<<<<<<<<<<<<<<End Encryption>>>>>>>>>>>>>>>>>");
  }


void decryption(uint8_t * cipher)
{
  uint8_t iv[BLOCK_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
 /*
  Serial.println("IV:");
  Print(iv,BLOCK_SIZE);
  Serial.println("KEY:");
  Print(key,BLOCK_SIZE);
  */
  br_aes_big_cbcdec_keys    ctx ; 
  Serial.println("<<<<<<<<<<<<<<<start Decryption>>>>>>>>>>>>>>>>>");
  Serial.println(" Cipher Text");
  Print(cipher,Size_Byte);

  br_aes_big_cbcdec_init(&ctx, key,BLOCK_SIZE ); 

  br_aes_big_cbcdec_run(&ctx,iv,  cipher, Size_Byte);

  Serial.println(" Plain text:");
  Print(cipher,Size_Byte);


  Serial.println("<<<<<<<<<<<<<<<End Decryption>>>>>>>>>>>>>>>>>");
  }


void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; //wait
  }
  //  delay(15000);
  
 
   

}

void Print(uint8_t * arr, int size)
{
  Serial.println();
  int start = size -32; //defualt = 0
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

void loop() {


  uint8_t * pt =rand_string_alloc(Size_Byte); 
  Print(pt,Size_Byte);
  unsigned long senc= micros(); 
  encryption(pt);
  unsigned long eenc= micros(); 
  decryption(pt);
  unsigned long edec= micros();
  Serial.print(" Time to encryption (us):"); 
  Serial.println( eenc -senc); 
 Serial.print(" Time to decryption (us):"); 
  Serial.println( edec -eenc); 
  delay(5000);
  
}

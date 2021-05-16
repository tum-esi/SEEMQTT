
#include "mbedtls/sha256.h"
#define DBG
#define HTLEN 100
#define HLEN 32


uint8_t  HashTable[HTLEN][32];
const  char * SecSeed = "keepitsecret"; 
void Fill_HT()
{
  mbedtls_sha256((const unsigned char *)SecSeed, strlen(SecSeed), HashTable[0], 0);

  for (int i = 1; i< HTLEN; i++)
  {
    mbedtls_sha256((const unsigned char *)HashTable[i-1], sizeof(uint8_t)*32, HashTable[i], 0);
  } 
  #ifdef DBG 
 for (int i = 0; i< HTLEN; i++)
  {
   Print(HashTable[i], 16, 32); 
  }
  #endif
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
     if(i> 0 && i % 16 ==0)
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
void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
  Fill_HT();

}

void loop() {
  // put your main code here, to run repeatedly:
  
}

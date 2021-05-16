/*
* Secret Splitting: we divide the secret into n parts. To reconstrcut the secret we need the n parts  (n,n)
*/


#define SLenght 16 
#define DBG
# define N 6
uint8_t ** table; 
void setup()
{
Serial.begin(115200);
}
 
void loop()
{
 uint8_t  secret[SLenght];

 table = (uint8_t **)malloc(N * sizeof(uint8_t *));
 esp_fill_random(secret, sizeof(uint8_t)*SLenght);

  #ifdef DBG
  Serial. print(" Initial Secret: "); 
  Print (secret, 0, SLenght); 
  #endif
 splitting(secret, N); 
 
   uint8_t * rsecret =extract (table, N); 
   #ifdef DBG
    Serial. print(" Recovered Secret: "); 
    Print (rsecret, 0, SLenght); 
  #endif
  delay(1000);
}
/*
 * creating t secret form the initil secret. 
 */
uint8_t ** splitting( uint8_t * secret, int t )
{

 /* 
  *Creat t-1 random  numbers 
  */
for ( int i =0;  i < t-1 ; i++)
{
  table[i] = (uint8_t *)malloc(SLenght * sizeof(uint8_t));
  esp_fill_random(table[i],  sizeof(uint8_t)*SLenght);
}

/*
 * Calulate the Nth secret
 */
table[t-1] = (uint8_t *)malloc(SLenght * sizeof(uint8_t));
for (int j=0 ; j< SLenght; j++)
{
  table[t-1] [j] = secret[j]; 
  for ( int i = 0;  i <  t-1 ; i++)
     table[t-1][j] = table[t-1][j] ^table[i][j];
}
 

 #ifdef DBG

 for ( int i = 0;  i < t ; i++)
 {
  Serial. print(" Secret #");
  Serial. print(i);
  Serial. print(": ");    
  Print (table[i], 0, SLenght); 
 }
 #endif

 return table; 
}

uint8_t *  extract(uint8_t  ** arr, int t )
{
  uint8_t * result = (uint8_t *) malloc (sizeof(uint8_t) * SLenght); 
  for ( int j = 0 ; j< SLenght; j++)
  {
     result [j] =  arr[0][j]; 
     for (int i = 1; i<t; i++)
        result [j] = result [j] ^ arr[i][j]; 
  }
  return result ; 
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

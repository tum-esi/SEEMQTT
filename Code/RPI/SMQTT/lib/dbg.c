#include "dbg.h"



void Print(uint8_t * arr, int size, int beg)
{

  int start = beg; //defualt = 0
  int row =0 ;
  printf("%d >>",row);
  for (int i =start; i<size; i++)
  {
     if(i> 0 && i % 16 ==0)
     {
     	row++;
     	printf("\n");
	printf("%d >>",row);

     }
    printf("%X ", arr[i]);

  }
  printf("\n");
}

void PrintHEX(unsigned char* str, int len) {

    for (int i = 0; i < len; ++i) {
        printf("%.2X ", str[i]);
    }

    printf("\n");
}

#include "main.h"
#include "Number.h"

int main(int argc, char ** argv)
{
  Number a;
  Number b;
  Number c;

  c = 0x03;

  //printf("%x %x %x\n", 290, 190, 290 + 190);
  
  a = 0x01;
  b = 0x02;
  a += b;
  printf("Hello world.\n");
  c = a + b;
  
  printf("--- Drawing result for 'a':\n");
  a.PrintHex();
  b.PrintHex();
  c.PrintHex();
  return 0;
}

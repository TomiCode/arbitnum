#include "main.h"
#include "Number.h"

int main(int argc, char ** argv)
{
  Number a;
  Number b;

  printf("%x %x %x\n", 290, 190, 290 + 190);
  
  a = 290;
  b = 190;
  a += b;
  a.PrintHex();
  
  return 0;
}

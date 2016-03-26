#include "main.h"
#include "Number.h"

int main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;

  Number a;
  Number b;
  Number c;

  a = 0x11114;
  c = 0x03;
  b = 0x01014;

  printf("%6x %6x %6x.\n", 0x11114, 0x01014, 0x11114 - 0x01014);

  a -= b;

  // c = a + b;
  
  // printf("--- Drawing result for 'a':\n");
  a.PrintHex();
  b.PrintHex();
  // c.PrintHex();
  return 0;
}

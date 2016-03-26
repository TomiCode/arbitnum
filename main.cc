#include "main.h"
#include "Number.h"

int main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;

  Number a;
  Number b;
  Number c;

  a = 0xffffff;
  c = 0x03;
  b = 0xfff;

  printf("%x %x %lx.\n", 0xffffff, 0xfff, 0xffffffL * 0xfffL);

  a *= b;
  c = a * b;
  
  // printf("--- Drawing result for 'a':\n");
  a.PrintHex();
  b.PrintHex();
  c.PrintHex();
  return 0;
}

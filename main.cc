#include "main.h"
#include "Number.h"

int main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;

  Number a;
  Number b;
  // Number c;

  a = 12;
  b = 15;
  printf("%x %x %x.\n", 12, 15, 15 - 12);
  a -= b;
  
  // printf("--- Drawing result for 'a':\n");
  a.PrintHex();
  b.PrintHex();
  printf("a <= b ? %s.\n", a <= b ? "yes" : "no");
  return 0;
}

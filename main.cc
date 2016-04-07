#include "main.h"
#include "Number.h"

int main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;

  Number a;
  Number b;
  // Number c;

  a = 0x15151507;
  b = 0x0A;
  printf("%x %x %x.\n", 0x15151507, 0x0A, 0x15151507 / 0x0A);
  
  a /= b;
  // printf("--- Drawing result for 'a':\n");
  a.PrintHex();
  b.PrintHex();
  printf("a <= b ? %s.\n", a <= b ? "yes" : "no");
  return 0;
}

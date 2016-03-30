#include "main.h"
#include "Number.h"

int main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;

  Number a;
  Number b;
  // Number c;

  a = 0x15263748U;
  b = 0x15263748U;
  // a = 0x15263748U;
  // c = 0x03;
  // b = 0x046219U;
  
  //printf("%x %x %lx.\n", 0x15263748U, 0x046219U, 0x15263748LU - 0x046219LU);

  // a -= b;
  
  // printf("--- Drawing result for 'a':\n");
  a.PrintHex();
  b.PrintHex();
  printf("a <= b ? %s.\n", a <= b ? "yes" : "no");
  return 0;
}

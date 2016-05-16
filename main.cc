#include "main.h"
#include "Number.h"

int main(int argc, char ** argv)
{
  (void)argc;
  (void)argv; 

  Number a = 0x7101;
  Number b = 0x8213;
  
  a.PrintHex();
  b.PrintHex();
  a ^= b;
  a.PrintHex();
  //Number b = "123456789123456789123456789";
  // Number b = "123123123";
  // a %= b;


  // printf("%lu.\n", b.c_str_size()); 
  // a.PrintHex();
  // char text[128] = {0};
  // a.c_str(text, 128);

  // printf("%s\n", text);

  // a = (long long)12;

  // Number a;
  // Number c;

  // a = 0xFFFFFF;
  // c = 0x02;

  // a.PrintHex();
  // c.testIt();
  // c.PrintHex();
  
  // a /= c;
  // a.PrintHex();
  // a.PrintHex();
  // c.PrintHex();

  // a /= c;

  // a.PrintHex();


  // a.PrintHex();
  // a.testIt();
  // a.PrintHex();

  /* {{{ Working project.. 
  int counter = 0;
  char * text = (char*)malloc(sizeof(char) * 16024);
  char * op = NULL;
  char operation = 0;

  if (text == NULL)
    return 0;

  scanf("%d", &counter);
  Number a;
  Number b;

  while(counter > 0) {
    scanf("%s", text);
    op = text;
    while(*op != '\0') {
      if (*op == '+' || *op == '-' || *op == '*' || *op == '/') {
        operation = *op;
        *op = '\0';
        break;
      }
      op++;
    }
    a = text;
    b = (op + 1);

    switch(operation) {
      case '+':
        a += b;
        break;
      case '-':
        a -= b;
        break;
      case '*':
        a *= b;
        break;
      case '/':
        a /= b;
        break;
      default:
      ;
    }
    a.c_str(text, 16024);
    printf("%s\n", text);
    counter--;
  }
  free(text);
  }}} */
  return 0;
}

#include "main.h"
#include "Number.h"

int main(int argc, char ** argv)
{
  (void)argc;
  (void)argv; 
  
  /* {{{ Working project..  */
  int counter = 0;
  char * text = (char*)malloc(sizeof(char) * 1024);
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
    a.c_str(text, 1024);
    printf("%s\n", text);
    counter--;
  }
  free(text);
  /* }}} */
  return 0;
}

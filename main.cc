#include "main.h"
#include "Number.h"

int main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;

  int counter = 0;
  
  char txt[1024] = {0};
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
    b = (op+1);

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
    a.c_str(txt, 1024);
    printf("%s\n", txt);
    counter--;
  }
  free(text);

  // Number c;
  // a = "-123456";
  // Number b;
  //Number c;
  //Number b;
  
  // c = "23384798427394293794727412894289182471294879712497812398759723762318768172687468127648716286487126485789327597923759837295779329587328942737821894862148762141249234789438";
  // b = "634784273478238949327736487268746287365832092098423095987239847238759872395792379883728983742387927598372958723987598237678264837293772537982738947237592463287463826483287647347982376852384876325792837489523758932789592375872839748732757892376723987587982376987239888342748792374987328957832749732985739827593298579823758327597328579832758972389573982759837203295328957932751287384972198471284791278724187421894712874982174721498712874982174798124871298749821";

  // c = "345";
  // b = "122";
  
  // b /= c;
  // b.c_str(txt, 1024);
  // c.PrintHex();
  // c = (int)345;
  // printf("Result: %s.\n", txt);
  return 0;
}

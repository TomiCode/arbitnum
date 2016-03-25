#ifndef NUMBER_H
#define NUMBER_H
#define CHAR_BITS 8

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

class Number {
private:
  uint8_t *pDigits;
  bool     bNegative;
  size_t   iSize;

  bool allocateType(size_t size, uint8_t ** out_ptr);

  template<class T>
    bool readStdType(T value);

public:
  Number();
  Number(const Number &copy);
  ~Number();

  void PrintHex();

  /* Assign standard C/C++ types. */
  void operator = (const int64_t value);
  void operator = (const uint64_t value);
  void operator = (const int32_t value);
  void operator = (const uint32_t value);
  void operator = (const int16_t value);
  void operator = (const uint16_t value);
  void operator = (const int8_t value);
  void operator = (const uint8_t value);
  
  /* Assign char buffer as number. */
  // bool operator = (const char * buffer);

  /* Assign number to number. Copy over digits. */
  Number& operator = (const Number &value);
  Number& operator = (Number &value);

  /* Comparison operators. */
  bool operator <  (const Number &rhs);
  bool operator >  (const Number &rhs);
  bool operator <= (const Number &rhs);
  bool operator >= (const Number &rhs);
  
  bool operator == (const Number &rhs);
  bool operator != (const Number &rhs);

  Number& operator += (const Number &rhs);
  const Number operator + (const Number &value);
  //void operator -= (const Number &rhs);
};

#endif

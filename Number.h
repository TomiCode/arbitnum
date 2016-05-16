#ifndef NUMBER_H
#define NUMBER_H

#define LOG_10_2 0.30102999566398119
#define LOG_2_10 3.32192809488736234

#define CHAR_BITS 8
#define MIN(a,b) (((a < b)) ? (a) : (b))
#define MAX(a,b) (((a > b)) ? (a) : (b))

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

class Number {
private:
  uint8_t *pDigits;
  bool     bNegative;
  size_t   iSize;

  void __init();

  bool __allocate(size_t size, uint8_t ** out_ptr);
  bool __reallocate(size_t size, uint8_t ** out_ptr);
  bool __cpymemory(void *ptr, size_t size);
  bool __applysize();

  bool __isvalid() const;
  bool __iszero() const;

  /* Comparison for equal sizes. Beware, no NULL protection. */
  bool __compare_greater(uint8_t *base_start, uint8_t *param_start, size_t size);
  bool __compare_less(uint8_t *base_start, uint8_t *param_start, size_t size);
  bool __compare_equal(uint8_t *base_start, uint8_t *param_start, size_t size);

  inline void __internal_add(uint8_t *operand, size_t operand_size, uint8_t *param, size_t param_size);
  inline void __internal_sub(uint8_t *operand, size_t operand_size, uint8_t *param, size_t param_size, uint8_t *result);
  
  void __internal_div(uint8_t *operand, size_t operand_size, uint8_t *param, size_t param_size, uint8_t *result, size_t result_size);

  /* For small operations like Number * 10, etc. */
  /* Of course, for internal usage only, nobody should use this else where, */
  /* Probably I'll use this for decimal conversions ;) */
  void __digit_sum(uint8_t param);
  void __digit_mul(uint8_t param);

  void __safe_small_div(uint8_t *array, size_t size, uint8_t param, uint8_t *mod);
  void __safe_small_mul(uint8_t *digits, size_t size, uint8_t val, size_t *valid_size);

  bool __operator_sum(const Number &param);
  bool __operator_sub(const Number &param, bool __op = false);
  bool __operator_mul(const Number &param);
  bool __operator_div(const Number &param);
  bool __operator_mod(const Number &param);

  template<typename T>
    bool readStdType(T value);

public:
  Number();
  Number(const Number &copy);
  Number(const int value);
  Number(const long value);
  Number(const long long value);
  Number(const unsigned int value);
  Number(const unsigned long value);
  Number(const unsigned long long value);
  Number(const char * buffer);
  
  ~Number();

  /* Number memory view. */
  void PrintHex();
  
  /* Assign standard C/C++ types. */
  Number& operator = (const int value);
  Number& operator = (const unsigned int value);
  Number& operator = (const long value);
  Number& operator = (const unsigned long value);
  Number& operator = (const long long value);
  Number& operator = (const unsigned long long value);
  
  /* Assign char buffer as number. */
  Number& operator = (const char * buffer);

  /* Assign number to number. Copy digits. */
  Number& operator = (const Number &value);

  /* Comparison operators. */
  bool operator <  (const Number &param);
  bool operator >  (const Number &param);
  bool operator <= (const Number &param);
  bool operator >= (const Number &param);
  bool operator == (const Number &param);
  bool operator != (const Number &param);

  Number& operator += (const Number &param);
  Number& operator -= (const Number &param);
  Number& operator *= (const Number &param);
  Number& operator /= (const Number &param);
  Number& operator %= (const Number &param);
  
  const Number operator + (const Number &param);
  const Number operator - (const Number &param);
  const Number operator * (const Number &param);
  const Number operator / (const Number &param);
  const Number operator % (const Number &param);

  size_t c_str_size();
  char * c_str(char * buffer, size_t size);

  void testIt();
};

#endif

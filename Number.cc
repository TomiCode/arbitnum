#include "Number.h"

Number::Number()
{
  this->pDigits = NULL;
  this->bNegative = false;
  this->iSize = 0;
}

Number::Number(const Number &copy)
{
  printf("Copy constructor call.\n");
 
  this->pDigits = NULL;
  this->bNegative = false;
  this->iSize = 0;

  if (copy.iSize <= 0 || copy.pDigits == NULL)
    return;

  uint8_t *digits = NULL;
  uint8_t *digit_end = NULL;
  uint8_t *copyDigits = copy.pDigits;

  if (!this->allocateType(copy.iSize, &digits)) {
    printf("Can not allocate digit memory.\n");
    return;
  }

  digit_end = (digits + this->iSize);
  for(; digits < digit_end; digits++, copyDigits++) {
    *digits = *copyDigits;
  }
  // this->operator=(copy);
}

Number::~Number()
{
  if (this->pDigits != NULL) {
    free(this->pDigits);
  }
}

bool Number::allocateType(size_t size, uint8_t ** out_ptr)
{
  if (size <= 0)
    return false;

  uint8_t *digit_ptr = (uint8_t*)malloc(sizeof(char) * size);
  if (digit_ptr == NULL)
    return false;

  if (this->pDigits != NULL) {
    printf("[allocateType]\t Freeing [%p]: %p.\n", this, this->pDigits);
    free(this->pDigits);
    this->iSize = 0;
  }

  this->pDigits = digit_ptr;
  this->iSize = size;
  if (out_ptr != NULL) {
    *out_ptr = digit_ptr;
  }
  printf("[allocateType]\t [%p]: %p.\n",this, this->pDigits);
  return true;
}

template<class T>
bool Number::readStdType(T value)
{
  T _val = value;
  uint8_t *digits = NULL;
  uint8_t *digit_end = NULL;

  if (!this->allocateType(sizeof(T), &digits)) {
    printf("Can not create digits for number.\n");
    return false;
  }

  printf("[readStdType]\t [%p] %p, size: %lu\n", this, this->pDigits, this->iSize);
  digit_end = (digits + this->iSize);

  if (_val < 0) {
    _val *= (-1);
    this->bNegative = true;
  }

  for(; digits != digit_end; digits++) {
    *digits = (uint8_t)(_val & 0xff);
    _val = _val >> CHAR_BITS;
    // printf("Digit [%02x].\n", *digits);
  } 
  return true;
}

void Number::PrintHex(void)
{
  printf("[%p/%p]: ", this, this->pDigits);
  if (this->iSize <= 0)
    return;

  if (this->pDigits == NULL)
    return;

  if (this->bNegative)
    printf("Negative number, ");

  uint8_t *numEnd = (this->pDigits + this->iSize - 1);

  for(; numEnd >= this->pDigits; numEnd--) {
    printf("%02x ", *numEnd);
  }
  putchar('\n');
}

void Number::operator = (const int64_t rhs)
{
  this->readStdType<int64_t>(rhs);
}

void Number::operator = (const uint64_t value)
{
  this->readStdType<uint64_t>(value);
}

void Number::operator = (const int32_t value)
{
  this->readStdType<int32_t>(value);
}

void Number::operator = (const uint32_t value)
{
  this->readStdType<uint32_t>(value);
}

void Number::operator = (const int16_t value)
{
  this->readStdType<int16_t>(value);
}

void Number::operator = (const uint16_t value)
{
  this->readStdType<uint16_t>(value);
}

void Number::operator = (const int8_t value)
{
  this->readStdType<int8_t>(value);
}

void Number::operator = (const uint8_t value)
{
  this->readStdType<uint8_t>(value);
}

Number& Number::operator = (const Number &value)
{
  printf("[%p] %p => [%p] %p.\n", this, this->pDigits, &value, value.pDigits);
  if (this == &value) {
    printf("Can not assign self.\n");
    return *this;
  }
  
  if (value.iSize <= 0 || value.pDigits == NULL) {
    printf("Size unknown.\n");
    return *this;
  }

  uint8_t *digits     = NULL;
  uint8_t *digit_end  = NULL;
  uint8_t *copyDigits = value.pDigits;
  
  if (!this->allocateType(value.iSize, &digits)) {
    printf("Can not allocate digits.\n");
    return *this;
  }

  digit_end = (digits + this->iSize);
  printf("Copying from %p to %p.\n", copyDigits, digits);
  for (; digits < digit_end; digits++, copyDigits++) {
    *digits = *copyDigits;
  }
  return *this;
}

Number& Number::operator+= (const Number &rhs)
{
  printf("Sum operator.\n");
  if (rhs.pDigits == NULL || rhs.iSize <= 0 || this->pDigits == NULL || this->iSize <= 0) {
    return *this;
  }

  printf("operation += on pointers: %p and %p.\n", this, &rhs);

  size_t  newSize = (rhs.iSize > this->iSize ? rhs.iSize : this->iSize) + 1; 
  uint8_t *newDigits = (uint8_t*)realloc(this->pDigits, sizeof(uint8_t) * newSize);
  if (newDigits == NULL) {
    printf("Can not reallocate result digits. (Internal error).\n");
    return *this;
  }

  this->iSize = newSize;
  this->pDigits = newDigits;

  uint16_t result = 0;
  uint8_t  carry = 0;
  for (size_t i = 0; i < this->iSize; i++) {
    if (i < rhs.iSize) {
      result = this->pDigits[i] + rhs.pDigits[i] + carry;
      this->pDigits[i] = (uint8_t)(result & 0xFF);
      carry = (uint8_t)(result >> CHAR_BITS);
    } else if (carry != 0) {
      result = this->pDigits[i] + carry;
      this->pDigits[i] = (uint8_t)(result & 0xFF);
      carry = (uint8_t)(result >> CHAR_BITS);
    } else {
      break;
    }
  }
  return *this;
}

const Number Number::operator + (const Number &value)
{
  return Number(*this) += value;
}

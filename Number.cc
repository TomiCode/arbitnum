#include "Number.h"
#include "Log.h"

Number::Number()
{
  this->pDigits   = NULL;
  this->iSize     = 0;
  this->bNegative = false;
}

Number::Number(const Number &copy)
{
  this->pDigits   = NULL;
  this->iSize     = 0;
  this->bNegative = false;

  if (copy.iSize <= 0 || copy.pDigits == NULL)
    return;

  if (!this->__allocate(copy.iSize, NULL)) {
    FAILPRINT("Can not allocate digits memory.\n");
    return;
  }

  if (!this->__cpymemory(copy.pDigits, copy.iSize)) {
    FAILPRINT("Memory copy failed.\n");
    return;
  }
}

Number::~Number()
{
  if (this->pDigits != NULL) {
    free(this->pDigits);
  }
}

bool Number::__allocate(size_t size, uint8_t ** out_ptr)
{
  if (size <= 0)
    return false;

  uint8_t *digit_ptr = (uint8_t*)malloc(sizeof(char) * size);
  if (digit_ptr == NULL)
    return false;

  if (this->pDigits != NULL) {
    INFOPRINT("Freeing [%p]: %p.\n", this, this->pDigits);
    free(this->pDigits);
    this->iSize = 0;
  }

  this->pDigits = digit_ptr;
  this->iSize = size;
  if (out_ptr != NULL) {
    *out_ptr = digit_ptr;
  }
  return true;
}

bool Number::__cpymemory(void *ptr, size_t size)
{
  if (ptr == NULL || size <= 0) {
    FAILPRINT("Can not copy non-existent buffer.\n");
    return false;
  }

  if (size > this->iSize) {
    FAILPRINT("Can not copy larger memory buffer.\n");
    return false;
  }
  
  uint8_t *digits = this->pDigits;
  uint8_t *dptr = (uint8_t*)ptr;
  for(size_t c = 0; c < this->iSize && c < size; c++) {
    *(digits++) = *(dptr++);
  }
  return true;
}

bool Number::__applysize()
{
  if (this->pDigits == NULL || this->iSize <= 0)
    return false;

  uint8_t *digitend = (this->pDigits + this->iSize - 1);
  size_t  newsize   = this->iSize;

  for (; digitend >= this->pDigits; digitend--) {
    if (*digitend == 0x00)
      newsize--;
    else
      break;
  }
  
  if (newsize == this->iSize)
    return true;

  WARNPRINT("Shrinking memory allocation %lu => %lu.\n", this->iSize, newsize);

  uint8_t *digitptr = (uint8_t*)realloc(this->pDigits, sizeof(uint8_t) * newsize);
  if (digitptr == NULL) {
    FAILPRINT("Can not shrink memory allocation. (nil).\n");
    return false;
  }

  this->iSize   = newsize;
  this->pDigits = digitptr;
  return true;
}

template<class T>
bool Number::readStdType(T value)
{
  T _val = value;

  if (!this->__allocate(sizeof(T), NULL)) {
    FAILPRINT("Can not create digits for number.\n");
    return false;
  }

  INFOPRINT("[%p] %p, size: %lu\n", this, this->pDigits, this->iSize);

  if (_val < 0) {
    _val *= (-1);
    this->bNegative = true;
  }
  
  if (!this->__cpymemory(&value, sizeof(T))) {
    FAILPRINT("Can not copy Type memory (%lu).\n", sizeof(T));
    return false;
  }

  if (!this->__applysize()) {
    FAILPRINT("Changing size failed.\n");
    return false;
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
  if (this == &value) {
    FAILPRINT("Can not assign self.\n");
    return *this;
  }
  
  if (value.iSize <= 0 || value.pDigits == NULL) {
    FAILPRINT("Size unknown.\n");
    return *this;
  }

  if (!this->__allocate(value.iSize, NULL)) {
    FAILPRINT("Can not allocate digits.\n");
    return *this;
  }

  if (!this->__cpymemory(value.pDigits, value.iSize)) {
    FAILPRINT("Error while copying memory.\n");
    return *this;
  }
  return *this;
}

Number& Number::operator += (const Number &rhs)
{
  if (rhs.pDigits == NULL || rhs.iSize <= 0 || this->pDigits == NULL || this->iSize <= 0) {
    return *this;
  }
  
  size_t  newSize = MAX(rhs.iSize, this->iSize) + 1; 
  uint8_t *newDigits = (uint8_t*)realloc(this->pDigits, sizeof(uint8_t) * newSize);
  if (newDigits == NULL) {
    printf("Can not reallocate result digits. (Internal error).\n");
    return *this;
  }

  this->iSize   = newSize;
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

Number& Number::operator -= (const Number &rhs)
{
  if (rhs.pDigits == NULL || rhs.iSize <= 0 || this->pDigits == NULL || this->iSize <= 0) {
    return *this;
  }
  size_t newsize = MAX(rhs.iSize, this->iSize);
  uint8_t *newDigits = NULL;

  if (this->iSize != newsize) {
    newDigits = (uint8_t*)realloc(this->pDigits, sizeof(uint8_t) * newsize);
    if (newDigits == NULL) {
      FAILPRINT("Can not allocate new digit buffer.\n");
      return *this;
    }
  }

  if (newDigits != NULL) {
    this->pDigits = newDigits;
    this->iSize = newsize;
  }
  newDigits = this->pDigits;
  uint8_t *rhsDigit = rhs.pDigits;
  uint8_t  carry  = 0;

  for (size_t s = 0; s < newsize; s++, newDigits++) {
    WARNPRINT("newDigits %02x rhsDigit %02x.\n", *newDigits, *rhsDigit);
    if (*newDigits < *rhsDigit) {
      *newDigits = (uint8_t)(((uint16_t)(1 << CHAR_BITS) | *newDigits) - carry);
      carry = 1;
    } else {
      *newDigits = *newDigits - carry;
      carry = 0;
    }
    *newDigits -= *(rhsDigit++);
    WARNPRINT("Result %02x.\n", *newDigits);
  }
  this->__applysize();
  return *this;
}

Number& Number::operator *= (const Number &value)
{
  /* To myself: Remember this is a bad solution to some problems. I should add some asserts here, to have a bit more fun. */
  if (value.iSize <= 0 || value.pDigits == NULL || this->iSize <= 0 || this->pDigits == NULL) {
    return *this;
  }
  size_t s_new = this->iSize + value.iSize;
  size_t s_old = this->iSize;
  
  uint8_t *p_digit  = this->pDigits;
  uint8_t *p_result = (uint8_t*)malloc(sizeof(uint8_t) * s_new);
  if (p_result == NULL) {
    FAILPRINT("Can not allocate result number.\n");
    return *this;
  }
  memset(p_result, 0, sizeof(uint8_t) * s_new);
  this->pDigits = p_result;
  this->iSize   = s_new; 

  uint16_t u_res = 0;
  uint8_t *p_lhs = NULL;
  uint8_t *p_rhs = NULL;

  for (size_t s_val = 0; s_val < value.iSize; s_val++) {
    p_result = (this->pDigits + s_val);
    p_rhs    = (value.pDigits + s_val);
    p_lhs    = p_digit;

    for (size_t s_opr = 0; s_opr < s_old || u_res > 0; s_opr++, p_result++, p_lhs++) {
      u_res   = (uint16_t)((*p_rhs) * (*p_lhs)) + u_res + (*p_result);
      *p_result = (uint8_t)(u_res & 0xFF);
      u_res   = (u_res >> CHAR_BITS);

      WARNPRINT("u_res %04x, p_rhs %02x, p_lhs %02x, p_result %02x.\n", u_res, *p_rhs, *p_lhs, *p_result);
    }
  }
  free(p_digit);
  p_digit = NULL;

  this->__applysize();
  return *this;
}

const Number Number::operator * (const Number &value)
{
  return Number(*this) *= value;
}

const Number Number::operator - (const Number &value)
{
  return Number(*this) -= value;
}

const Number Number::operator + (const Number &value)
{
  return Number(*this) += value;
}

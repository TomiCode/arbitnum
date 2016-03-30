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

bool Number::__reallocate(size_t size, uint8_t ** out_ptr)
{
  uint8_t *ndigits = (uint8_t*)realloc(this->pDigits, sizeof(uint8_t) * size);
  if (ndigits == NULL) {
    FAILPRINT("Can not modify memory allocation.\n");
    return false;
  }
  this->iSize   = size;
  this->pDigits = ndigits;

  if (out_ptr != NULL) {
    *out_ptr = ndigits;
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

bool Number::__isvalid() const
{
  if (this->pDigits == NULL || this->iSize <= 0)
    return false;

  return true;
}

bool Number::__compare_greater(uint8_t *base_start, uint8_t *param_start, size_t size)
{
  uint8_t * baseptr  = (base_start  + size - 1);
  uint8_t * paramptr = (param_start + size - 1);

  for (; baseptr >= base_start; baseptr--, paramptr--) {
    if (*baseptr > *paramptr)
      return true;
    else if (*baseptr < *paramptr)
      return false;
  }
  return false;
}

bool Number::__compare_less(uint8_t *base_start, uint8_t *param_start, size_t size)
{
  uint8_t *baseptr  = (base_start  + size - 1);
  uint8_t *paramptr = (param_start + size - 1);

  for(; baseptr >= base_start; baseptr--, paramptr--) {
    if (*baseptr < *paramptr)
      return true;
    else if (*baseptr > *paramptr)
      return false;
  }
  return false;
}

bool Number::__compare_equal(uint8_t *base_start, uint8_t *param_start, size_t size)
{
  uint8_t *baseptr  = (base_start  + size - 1);
  uint8_t *paramptr = (param_start + size - 1);

  for(; baseptr >= base_start; baseptr--, paramptr--) {
    if (*baseptr != *paramptr)
      return false;
  }
  return true;
}

bool Number::__operator_sum(const Number &param)
{
  if (this == &param) {
    WARNPRINT("Self operation not supported.\n");
    return false;
  }

  if (!this->__isvalid()) {
    FAILPRINT("Invalid self.\n");
    return false;
  }

  if (!param.__isvalid()) {
    FAILPRINT("Operator param invalid.\n");
    return false;
  }

  size_t   nsize  = MAX(this->iSize, param.iSize) + 1;
  uint8_t *pardig = param.pDigits;
  uint8_t *digits = NULL;
  uint16_t result = 0;

  if (!this->__reallocate(nsize, &digits)) {
    FAILPRINT("Can not reallocate memory.\n");
    return false;
  }

  for (size_t s = 0; s < nsize; s++, digits++) {
    if (s < param.iSize) {
      result  = ((uint16_t)(*digits + *(pardig++))) + result;
      *digits = (uint8_t)(result & 0xFF);
      result  = (uint8_t)(result >> CHAR_BITS); 
    } else if (result != 0) {
      result  = ((uint16_t)(*digits)) + result;
      *digits = (uint8_t)(result & 0xFF);
      result  = (uint8_t)(result >> CHAR_BITS);
    } else break;
  }
  
  if (!this->__applysize()) {
    FAILPRINT("Can not change allocated memory region.\n");
  }
  return true;
}

bool Number::__operator_sub(const Number &param, bool __op)
{
  if (this == &param) {
    WARNPRINT("Self operation not supported.\n");
    return false;
  }

  if (!this->__isvalid()) {
    FAILPRINT("Invalid self.\n");
    return false;
  }

  if (!param.__isvalid()) {
    FAILPRINT("Operator param invalid.\n");
    return false;
  }

  size_t nsize = MAX(this->iSize, param.iSize);
  uint8_t *pardig = NULL;
  uint8_t *digits = NULL;
  uint8_t *result = NULL;
  
  if (this->iSize != nsize) {
    if (!this->__reallocate(nsize, &result)) {
      FAILPRINT("Error while memory reallocate.\n");
      return false;
    }
  } else {
    result = this->pDigits;
  }

  pardig = __op ? this->pDigits : param.pDigits;
  digits = __op ? param.pDigits : this->pDigits;

  uint8_t carry = 0;
  for (size_t s = 0; s < nsize; s++, digits++, result++, pardig++) {
    if (*digits < *pardig) {
      *result = (uint8_t)(((uint16_t)(1 << CHAR_BITS) | *digits) - *pardig - carry); 
      carry   = 0x01;
    } else {
      *result = (uint8_t)(*digits - *pardig - carry);
      carry   = 0x00;
    }
  }

  if (!this->__applysize()) {
    FAILPRINT("Can not change allocated memory region.\n");
  }
  return true;
}

bool Number::__operator_mul(const Number &param)
{
  if (this == &param) {
    WARNPRINT("Self operation not supported.\n");
    return false;
  }
  
  if (!this->__isvalid()) {
    FAILPRINT("Invalid self.\n");
    return false;
  }

  if (!param.__isvalid()) {
    FAILPRINT("Operator param invalid.\n");
    return false;
  }

  size_t sres = this->iSize + param.iSize;
  uint8_t * const res_digits = (uint8_t*)malloc(sizeof(uint8_t) * sres);
  
  if (res_digits == NULL) {
    FAILPRINT("Can not allocate result number.\n");
    return false;
  }
  memset(res_digits, 0, sizeof(uint8_t) * sres);

  uint8_t *resdig = NULL;
  uint8_t *pardig = NULL;
  uint8_t *digits = NULL;
  uint16_t result = 0;

  for (size_t ps = 0; ps < param.iSize; ps++) {
    resdig = (res_digits + ps);
    pardig = (param.pDigits + ps);
    digits = this->pDigits;

    WARNPRINT("Starting %02lu cycle.\n", ps);
    for (size_t so = 0; so < this->iSize || result > 0; so++, digits++, resdig++) {
      result  = ((uint16_t)((*pardig) * (*digits)) + result + (*resdig));
      *resdig = (uint8_t)(result & 0xFF);
      result  = (result >> CHAR_BITS);

      INFOPRINT("result %02x, pardig %02x, digits %02x, resdig %02x.\n", result, *pardig, *digits, *resdig);
    }
  }

  WARNPRINT("Freeing self digits %p and assign result %p.\n", this->pDigits, res_digits);
  free(this->pDigits);
  
  this->pDigits = res_digits;
  this->iSize   = sres;

  if (!this->__applysize()) {
    FAILPRINT("Can not change allocated result size.\n");
  }
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

bool Number::operator < (const Number &param)
{
  if (this->bNegative && !param.bNegative) {
    return true;
  } else if (!this->bNegative && param.bNegative) {
    return false;
  } else if (this->bNegative && param.bNegative) {
    if (this->iSize > param.iSize) {
      return true;
    } else if (this->iSize < param.iSize) {
      return false;
    } else return this->__compare_greater(this->pDigits, param.pDigits, this->iSize);
    
  } else {
    if (this->iSize < param.iSize) {
      return true;
    } else if (this->iSize > param.iSize) {
      return false;
    } else return this->__compare_less(this->pDigits, param.pDigits, this->iSize);
  }
}

bool Number::operator > (const Number &param)
{
  if (!this->bNegative && param.bNegative) {
    return true;
  } else if (this->bNegative && !param.bNegative) {
    return false;
  } else if (this->bNegative && param.bNegative) {
    if (this->iSize < param.iSize) {
      return true;
    } else if (this->iSize > param.iSize) {
      return false;
    } else return this->__compare_less(this->pDigits, param.pDigits, this->iSize);
  } else {
    if (this->iSize > param.iSize) {
      return true;
    } else if (this->iSize < param.iSize) {
      return false;
    } else return this->__compare_greater(this->pDigits, param.pDigits, this->iSize);
  }
}

bool Number::operator >= (const Number &param)
{
  return !(this->operator<(param));
}

bool Number::operator <= (const Number &param)
{
  return !(this->operator>(param));
}

bool Number::operator == (const Number &param)
{
  if (this->bNegative != param.bNegative) {
    return false;
  } else if (this->iSize != param.iSize) {
    return false;
  }
  return this->__compare_equal(this->pDigits, param.pDigits, this->iSize);
}

bool Number::operator != (const Number &param)
{
  return !(this->operator==(param));
}

Number& Number::operator += (const Number &param)
{
  if ((this->bNegative && param.bNegative) || 
      (!this->bNegative && !param.bNegative)) {
    
    this->__operator_sum(param);
    return *this;
  }
  /* Negative number operation: WIP - doesn't work. :D */
  this->__operator_sum(param);
  return *this;
}

Number& Number::operator -= (const Number &rhs)
{
  this->__operator_sub(rhs, true);
  return *this;
}

Number& Number::operator *= (const Number &value)
{
  this->__operator_mul(value);
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

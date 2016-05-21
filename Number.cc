#include "Number.h"
#include "Log.h"

#define BIT_OP_AND 0x01
#define BIT_OP_OR  0x02
#define BIT_OP_XOR 0x04

Number::Number()
{
  this->__init();
}

Number::Number(const Number &copy)
{
  this->__init();
  
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

Number::Number(const int value)
{
  this->__init();
  this->readStdType<int>(value);
}

Number::Number(const long value)
{
  this->__init();
  this->readStdType<long>(value);
}

Number::Number(const long long value)
{
  this->__init();
  this->readStdType<uint64_t>(value);
}

Number::Number(const unsigned int value)
{
  this->__init();
  this->readStdType<unsigned int>(value);
}

Number::Number(const unsigned long value)
{
  this->__init();
  this->readStdType<unsigned long>(value);
}

Number::Number(const unsigned long long value)
{
  this->__init();
  this->readStdType<unsigned long long>(value);
}

Number::Number(const char * buffer)
{
  this->pDigits   = NULL;
  this->iSize     = 0;
  this->bNegative = 0;

  this->operator=(buffer);
}

Number::~Number()
{
  if (this->pDigits != NULL) {
    free(this->pDigits);
  }
}

void Number::__init()
{
  this->pDigits   = NULL;
  this->iSize     = 0;
  this->bNegative = false;
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
  if (newsize != 0 && digitptr == NULL) {
    FAILPRINT("Can not shrink memory allocation. (nil).\n");
    return false;
  } else if (newsize == 0) {
    WARNPRINT("Clearing number allocation. (zero detection)\n");
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

inline void Number::__internal_add(uint8_t *operand, size_t operand_size, uint8_t *param, size_t param_size)
{
  uint16_t result = 0;

  for(size_t sz = 0; sz < operand_size; sz++, operand++) {
    if (sz < param_size) {
      result    = ((uint16_t)(*operand + *(param++))) + result;
      *operand  = (uint8_t)(result & 0xFF);
      result    = (uint8_t)(result >> CHAR_BITS);
    } else if (result != 0) {
      result    = ((uint16_t)(*operand)) + result;
      *operand  = (uint8_t)(result & 0xFF);
      result    = (uint8_t)(result >> CHAR_BITS);
    } else break;
  }
}

inline void Number::__internal_sub(uint8_t *operand, size_t operand_size, uint8_t *param, size_t param_size, uint8_t *result)
{
  uint8_t carry = 0;
  for (size_t sz = 0; sz < operand_size; sz++, operand++, param++, result++) {
    if (sz < param_size) {
      if (*operand < (*param + carry)) {
        *result = (uint8_t)(((uint16_t)(1 << CHAR_BITS) | *operand) - *param - carry);
        carry   = 0x01;
      } else {
        *result = (uint8_t)(*operand - *param - carry);
        carry   = 0x00;
      }
    } else {
      *result = (uint8_t)(*operand - carry);
      carry   = 0x00;
    }
  }
}

void Number::__internal_div(uint8_t *operand, size_t operand_size, uint8_t *param, size_t param_size, uint8_t *result, size_t result_size)
{
  size_t csize = param_size;
  uint8_t *digits = operand + (operand_size - param_size);
  uint8_t *resdig = result != NULL ? (result + (result_size - param_size)) : NULL;
  uint8_t *oprdig = NULL;

  for (; digits >= operand; ) {
    if (csize > param_size) {
      for (oprdig = (digits + csize - 1); *oprdig == 0 && oprdig > digits; oprdig--) {
        csize--;
      }
    } else if (csize < param_size) {
      if (resdig != NULL) resdig--;
      digits--;
      csize++;
      continue;
    } else if (csize == param_size && this->__compare_less(digits, param, param_size)) {
      oprdig = digits;
      for (size_t pr = 0; pr < param_size; pr++, oprdig++) {
        if (*oprdig != 0) {
          csize++;
          break;
        }
      }
      if (resdig != NULL) resdig--;
      digits--;
      continue;
    }
    this->__internal_sub(digits, csize, param, param_size, digits);
    if (resdig != NULL) (*resdig)++;
  }
}

void Number::__digit_sum(uint8_t param)
{
  if (param == 0) return;
  
  // TODO: check if there is space to add this value.
  this->__internal_add(this->pDigits, this->iSize, &param, sizeof(uint8_t));
}

void Number::__digit_mul(uint8_t param)
{
  if (param <= 1) return;

  uint8_t *digits = this->pDigits;
  uint16_t result = 0;

  for (size_t so = 0; so < this->iSize || result > 0; so++, digits++) {
    result  = ((uint16_t)(param * (*digits)) + result);
    *digits = (uint8_t)(result & 0xFF);
    result  = (result >> CHAR_BITS);
  }
}

void Number::__safe_small_div(uint8_t *array, size_t size, uint8_t param, uint8_t *mod)
{
  uint8_t  *digits = (array + size - 1);
  uint16_t operand = *digits;
  uint8_t  result  = 0;

  while (digits >= array) { 
    if (operand < param) {
      *digits = result;
      result  = 0;
      operand = (operand << CHAR_BITS) | *(--digits);
      continue;
    } else {
      operand = (operand - param);
      result++;
    }
  }

  if (mod != NULL) {
    *mod = (uint8_t)(operand >> CHAR_BITS);
  }
}

void Number::__safe_small_mul(uint8_t *digits, size_t size, uint8_t val, size_t *valid_size)
{
  if (digits == NULL) {
    FAILPRINT("Can not access digits pointer.\n");
    return;
  }

  uint16_t result  = 0;
  uint8_t  *base_ptr = digits;

  for (size_t sz = 0; sz < size || result > 0; sz++, digits++) {
    if (size > sz) {
      result = ((uint16_t)(*digits) * val) + result;
    }
    *digits = (uint8_t)(result & 0xFF);
    result  = (uint8_t)(result >> CHAR_BITS);
  }

  *valid_size = size;
  for (uint8_t *base = (base_ptr + size - 1); base >= base_ptr; base--) {
    if (*base == 0x00) 
      (*valid_size)--;
    else break;
  }
  // INFOPRINT("Current size: %lu => %lu\n", size, *valid_size);
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

  if (!this->__reallocate(MAX(this->iSize, param.iSize) + 1, NULL)) {
    FAILPRINT("Can not reallocate memory.\n");
    return false;
  }

  this->__internal_add(this->pDigits, this->iSize, param.pDigits, param.iSize);
  if (!this->__applysize()) {
    FAILPRINT("Error while shrinking of memory allocation.\n");
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
  uint8_t *result = NULL;
  
  if (this->iSize != nsize) {
    if (!this->__reallocate(nsize, &result)) {
      FAILPRINT("Error while memory reallocate.\n");
      return false;
    }
  } else {
    result = this->pDigits;
  }

  if (*this < param) {
    __op = !__op;
    this->bNegative = true;
  }

  this->__internal_sub((__op ? param.pDigits : this->pDigits),
          this->iSize, (__op ? this->pDigits : param.pDigits), 
          param.iSize, result);

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
      if (so < this->iSize) {
        result  = ((uint16_t)((*pardig) * (*digits)) + result + (*resdig));
      }
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

bool Number::__operator_div(const Number &param)
{
  if (this == &param) {
    FAILPRINT("Self operation not supported.\n");
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

  if (this->iSize < param.iSize) {
    FAILPRINT("Parameter has more digits - freeing number..\n");
    
    free(this->pDigits);
    this->pDigits = NULL;
    this->iSize   = 0;
    
    return true;
  }

  size_t sres = MAX(this->iSize, param.iSize);
  uint8_t * const result = (uint8_t*)malloc(sizeof(uint8_t) * sres);
  if (result == NULL) {
    FAILPRINT("Can not allocate result digits.\n");
    return false;
  }
  memset(result, 0, sizeof(uint8_t) * sres);
  
  this->__internal_div(this->pDigits, this->iSize, param.pDigits, param.iSize, result, sres);

  WARNPRINT("Removing operand digits!\n");
  free(this->pDigits);

  this->pDigits = result;
  this->iSize   = sres;
  this->__applysize();
  return true;
}

bool Number::__operator_mod(const Number &param)
{
  if (this == &param) {
    FAILPRINT("Self operation not supported.\n");
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

  if (this->iSize < param.iSize) {
    INFOPRINT("Nothing to do. (param has more digits)\n");
    return true;
  }
  this->__internal_div(this->pDigits, this->iSize, param.pDigits, param.iSize, NULL, 0);
  this->__applysize();
  return true;
}

bool Number::__operator_bit(const Number &param, uint8_t type)
{
  if (this == &param) {
    FAILPRINT("Self operation not supported.\n");
    return false;
  }

  if (!this->__isvalid()) {
    FAILPRINT("Invalid self.\n");
    return false;
  }

  if (!param.__isvalid()) {
    FAILPRINT("Invalid operator parameter.\n");
    return false;
  }

  size_t nsize = MAX(this->iSize, param.iSize);
  uint8_t *digits = this->pDigits;
  uint8_t *pardig = param.pDigits;

  if (nsize != this->iSize) {
    INFOPRINT("Reallocation of digits for result. (%lu => %lu)\n", this->iSize, nsize);
    digits = (uint8_t*)realloc(this->pDigits, nsize);
    if (digits == NULL) {
      FAILPRINT("Can not reallocate result number.\n");
      return false;
    }
    
    memset((digits + this->iSize), 0, nsize - this->iSize);
    this->pDigits = digits;
    this->iSize   = nsize;
  }

  for (size_t sz = 0; sz < param.iSize; sz++, digits++, pardig++) {
    INFOPRINT("Bit operation for type: %02x.\n", type);
    if (type == BIT_OP_AND) {
      *digits = (*digits & *pardig);
    } else if (type == BIT_OP_OR) { 
      *digits = (*digits | *pardig);
    } else if (type == BIT_OP_XOR) {
      *digits = (*digits ^ *pardig);
    }
  }
  this->__applysize();
  return true;
}

bool Number::__operator_bit_shift_left(const size_t param)
{
  if (!this->__isvalid()) {
    FAILPRINT("Invalid self.\n");
    return false;
  }
  if (param == 0) return true;

  uint8_t *digits  = NULL;
  uint8_t *ending  = NULL;
  uint8_t *valptr  = NULL;
  uint16_t lastval = 0;

  uint8_t bit_sh = (param % CHAR_BITS);
  size_t byte_sh = (param / CHAR_BITS);

  /* Need to reallocate memory region. */
  digits = (uint8_t*)realloc(this->pDigits, sizeof(uint8_t) * (this->iSize + byte_sh + 1));
  if (digits == NULL) {
    FAILPRINT("Can not reallocate result buffer.\n");
    return false;
  }

  INFOPRINT("Moving %lu bytes and %u bits in number.\n", byte_sh, bit_sh);
  this->pDigits = digits;
  this->iSize   = (this->iSize + byte_sh + 1);

  ending = (this->pDigits + this->iSize);
  
  for (size_t ls = 0; ls < byte_sh && digits < ending; ls++, digits++) {
    for (valptr = (digits + byte_sh); valptr < ending; valptr += byte_sh) {
      lastval = *valptr;
      *valptr = *digits;
      *digits = lastval;
    }
    *digits = 0;
  }

  lastval = 0;
  for (digits = (this->pDigits + byte_sh); digits < ending; digits++) {
    lastval = (*digits << bit_sh) | lastval;
    *digits = (uint8_t)(lastval & 0xFF);
    lastval = (lastval >> CHAR_BITS);
  }
  this->__applysize();
  return true;
}

bool Number::__operator_bit_shift_right(const size_t param)
{
  if (!this->__isvalid()) {
    FAILPRINT("Invalid self.\n");
    return false;
  }
  if (param == 0) return true;

  uint8_t *digits = (this->pDigits + this->iSize - 1);
  uint8_t *valptr = NULL;
  uint16_t lastval = 0;

  uint8_t bit_sh = (param % CHAR_BITS);
  size_t byte_sh = (param / CHAR_BITS);

  for (size_t rs = 0; rs < byte_sh && digits >= this->pDigits; rs++, digits--) {
    for (valptr = (digits - byte_sh); valptr >= this->pDigits; valptr -= byte_sh) {
      lastval = *valptr;
      *valptr = *digits;
      *digits = lastval;
    }
    *digits = 0;
  }

  if (bit_sh != 0) {
    for (digits = (this->pDigits + this->iSize - 1 - byte_sh); digits >= this->pDigits; digits--) {
      lastval = ((*digits << CHAR_BITS) >> bit_sh) | (lastval << CHAR_BITS);
      *digits = (uint8_t)(lastval >> CHAR_BITS);
      lastval = (lastval & 0xFF);
    }
  }

  return this->__applysize();
}

/* This is a internal template, not for public usage. */
/* Should be a type with direct memory access! */
template<class T>
bool Number::readStdType(T value)
{
  if (!this->__allocate(sizeof(T), NULL)) {
    FAILPRINT("Can not create digits for number.\n");
    return false;
  }

  INFOPRINT("[%p] %p, size: %lu\n", this, this->pDigits, this->iSize);

  if (value < 0) {
    value *= (-1);
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
  if (this->iSize <= 0 || this->pDigits == NULL) {
    printf(" (nil)\n");
    return;
  }

  if (this->bNegative)
    printf("Negative number, ");

  for(uint8_t *numEnd = (this->pDigits + this->iSize - 1); numEnd >= this->pDigits; numEnd--) {
    printf("%02x ", *numEnd);
  }
  putchar('\n');
}

/* {{{ Standard C/C++ type assign operator */
Number& Number::operator = (const int value)
{
  this->readStdType<int>(value);
  return *this;
}

Number& Number::operator = (const unsigned int value)
{
  this->readStdType<unsigned int>(value);
  return *this;
}

Number& Number::operator = (const long value)
{
  this->readStdType<long>(value);
  return *this;
}

Number& Number::operator = (const unsigned long value)
{
  this->readStdType<unsigned long>(value);
  return *this;
}

Number& Number::operator = (const long long value)
{
  this->readStdType<long long>(value);
  return *this;
}

Number& Number::operator = (const unsigned long long value)
{
  this->readStdType<unsigned long long>(value);
  return *this;
}
/* }}} */

Number& Number::operator = (const char * buffer)
{
  if (*buffer == '-') {
    this->bNegative = true;
    buffer++;
  } else {
    this->bNegative = false;
  }

  size_t nsize = strlen(buffer);
  INFOPRINT("Number buffer size: %lu.\n", nsize);

  for (const char * buf = buffer; *buf != 0; buf++) {
    if (*buf >= '0' && *buf <= '9') continue;
    else {
      FAILPRINT("Character buffer contais invalid character '%c' (0x%02x).\n", *buf, *buf);
      return *this;
    }
  }

  nsize = (size_t)((LOG_2_10 * nsize) / CHAR_BITS) + 1;
  if (!this->__allocate(nsize, NULL)) {
    FAILPRINT("Can not create digits buffer.\n");
    return *this;
  }
  memset(this->pDigits, 0, sizeof(uint8_t) * nsize);

  INFOPRINT("Allocated %lu, for number.\n", nsize);
  for (; *buffer != 0; buffer++) {
     this->__digit_mul(10);
     this->__digit_sum(*buffer - '0');
  }
  this->__applysize();
  return *this;
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
  if (this->bNegative == param.bNegative) {
    this->__operator_sum(param);
  } else if (this->bNegative && !param.bNegative) {
    this->__operator_sub(param, true);
  } else if (!this->bNegative && param.bNegative) {
    this->__operator_sub(param, false);
  }

  return *this;
}

Number& Number::operator -= (const Number &param)
{
  if (!this->bNegative && !param.bNegative) {
    this->__operator_sub(param, false);
  } else if (this->bNegative && param.bNegative) {
    this->__operator_sub(param, true);
  } else {
    this->__operator_sum(param);
  }

  return *this;
}

Number& Number::operator *= (const Number &param)
{
  this->bNegative = this->bNegative ^ param.bNegative;
  this->__operator_mul(param);
  return *this;
}

Number& Number::operator /= (const Number &param)
{
  this->bNegative = this->bNegative ^ param.bNegative;
  this->__operator_div(param);
  return *this;
}

Number& Number::operator %= (const Number &param)
{
  /* This operation depends on self-negativeness. */
  /* Of course, this operator is a remainder not a MODULO. */
  this->__operator_mod(param);
  return *this;
}

const Number Number::operator + (const Number &param)
{
  return Number(*this) += param;
}

const Number Number::operator - (const Number &param)
{
  return Number(*this) -= param;
}

const Number Number::operator * (const Number &param)
{
  return Number(*this) *= param;
}

const Number Number::operator / (const Number &param)
{
  return Number(*this) /= param;
}

const Number Number::operator % (const Number &param)
{
  return Number(*this) /= param;
}

Number& Number::operator &= (const Number &param)
{
  this->__operator_bit(param, BIT_OP_AND);
  return *this;
}

Number& Number::operator |= (const Number &param)
{
  this->__operator_bit(param, BIT_OP_OR);
  return *this;
}

Number& Number::operator ^= (const Number &param)
{
  this->__operator_bit(param, BIT_OP_XOR);
  return *this;
}

Number& Number::operator <<= (const size_t param)
{
  this->__operator_bit_shift_left(param);
  return *this;
}

Number& Number::operator >>= (const size_t param)
{
  this->__operator_bit_shift_right(param);
  return *this;
}

const Number Number::operator & (const Number &param)
{
  return Number(*this) &= param;
}

const Number Number::operator | (const Number &param)
{
  return Number(*this) |= param;
}

const Number Number::operator ^ (const Number &param)
{
  return Number(*this) ^= param;
}

const Number Number::operator << (const size_t param)
{
  return Number(*this) <<= param;
}

const Number Number::operator >> (const size_t param)
{
  return Number(*this) >>= param;
}

Number::operator bool()
{
  return this->__isvalid();
}

/* Not very digit precise.. but it should work. */
size_t Number::c_str_size()
{
  return (size_t)(LOG_10_2 * CHAR_BITS * (this->iSize + 1));
}

char * Number::c_str(char *buffer, size_t size)
{
  if (buffer == NULL)
    return NULL;

  if (size < this->c_str_size()) {
    FAILPRINT("Character buffer to small.\n");
    return NULL;
  }

  if (!this->__isvalid()) {
    INFOPRINT("Not valid number, result is zero.\n");

    *(buffer++) =  '0';
    *(buffer) = '\0';
    return buffer;
  }

  size_t  csize = this->iSize;

  char *buff = buffer;
  char *ending = NULL;

  uint8_t *digits = (uint8_t*)malloc(sizeof(uint8_t) * this->iSize);
  uint8_t *param = NULL;
  uint8_t  result = 0;

  if (digits == NULL) {
    FAILPRINT("Can not allocate temporary work buffer!\n");
    return NULL;
  }
  memcpy(digits, this->pDigits, this->iSize);
  
  while (csize > 0) {
    this->__safe_small_div(digits, csize, 10, &result);
    *(buff++) = (result + '0');
    for (param = (digits + csize - 1); param >= digits; param--) {
      if (*param != 0) break;
      else csize--;
    }
  }

  if (this->bNegative) {
    *(buff++) = '-';
  }
  *buff = '\0';
  ending = (--buff);
  
  /* Flip character table. */
  for (buff = buffer; ending > buff; ending--, buff++) {
    result = *buff;
    *buff = *ending;
    *ending = result;
  }

  free(digits);
  return buffer;
}


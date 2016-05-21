ArbitNum
========

ArbitNum is a C++ class that allow arithmetic and bitwise operations on very long numbers.

Example
-------

Using this class should be as easy, as using an build in type. Here is an example, using Number-type variables
```
#include <stdio.h>
#include "Number.h"

int main(int argc, char ** argv)
{
  (void)argc;
  (void)argv; 
  
  Number first = "12345814728741928741275651287676148";
  Number second;
  
  second = 534126;
  
  if (!first || !second) {
    printf("One variable is not initialized.\n");
    return 0;
  }

  Number result = first * second;
  if (!result) {
    printf("Result variable is invalid.\n");
    return 0;
  }

  char number_text[128] =  {0};
  if (result.c_str(number_text, 128) == NULL) {
    printf("Number conversion failed.\n");
    return 0;
  }
  printf("%s\n", number_text);

  return 0;
}
```

This example shows, how variables can be assigned and the conversion between Number and a decimal character array.

Usage
-----

This arbitrary number class is using four C-language headers and nothing more!
For compilation, You may use any C++ compiler with any C++ standard out there.

*Anything else?* Nope, just include the `Number.h` file and the class is ready to go!

Required header files for compilation:
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
```

Reference
--------- 

### Constructor
```
Number( void );
Number( Number );
Number( int );
Number( unsigned int );
Number( long )
Number( unsigned long );
Number( long long );
Number( unsigned long long );
Number( char * );
```
Number variable initialized without parameters is not valid. Casting the variable into an `boolean` will result as `false`.

### Assignment operator
#### Simple assignment
```
Number variable;
...

variable = ( Number );

variable = ( int );
variable = ( unsigned int );
variable = ( long );
variable = ( unsigned long );
variable = ( long long );
variable = ( unsigned long long );

variable = ( const char * );
```
All assignment operators returns Number references (Using a chain of operators is allowed).

#### Arithmetic assignment
```
Number variable;
...

variable += ( Number );
variable -= ( Number );
variable *= ( Number );
variable /= ( Number );
variable %= ( Number );
```

#### Bit operators
``` 
Number variable;
...

variable &= ( Number );
variable |= ( Number );
variable ^= ( Number );
variable <<= ( size_t );
variable >>= ( size_t );
```
Only left and right shift operators currently have the `size_t` limitation.

### Type comparison
Like all build in types, this implements all kind of comparison operators.

```
Number a, b;
...

if ( a < b ) { ... }
if ( a <= b ) { ... }

if ( a > b ) { ... }
if ( a >= b ) { ... }

if ( a == b ) { ... }
if ( a != b ) { ... }
```

### Arithmetic operators 
```
Number a,b;
...

Number c = a + b;
Number c = a - b;
Number c = a * b;
Number c = a / b;
Number c = a % b;
```

### Bit manipulation
```
Number a,b;
...

Number c = a & b;
Number c = a | b;
Number c = a ^ b;
```

### Bit-shifting
```
Number c = ( Number ) << ( size_t );
Number c = ( Number ) >> ( size_t );
```

### Checking if a number is valid
In the C/C++ family all variables that have a value ( differs from nil ) are casted into a boolean as true.
With this type, this conversion works exactly like this! If the number is uninitialized or just nil, an boolean cast will result as false.
```
Number var;

if (!var) { ... }
```
In this snipped var is uninitialized, and the if-statement will be executed.

### Decimal character array conversion
#### Character array size

Checking needed character space for fitting the whole number into an array
```
Number var;
...

( size_t ) var.c_str_size( void );
```

#### Writing characters into buffer
To create the character array with the number:
```
(char *) var.c_str( ( char * ), ( size_t ) );
```
The first parameter is a pointer to the begining of a character array and the second parameter is the maximum size what this function should have access to.

If the return value is `NULL` an error occurred while conversion. Most often the allocated character array is to small, to fit all digits.

Bugs and Features
-----------------

Feel free to report any bugs found in my source. 

*Want a new feature?* Open a new ticket and I will take care of that. I'm open to any new features, that will be implemented in the code.

License
-------

The ArbitNum class uses [The BSD 3-Clause License](LICENSE).
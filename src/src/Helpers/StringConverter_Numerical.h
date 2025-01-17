#ifndef HELPERS_STRINGCONVERTER_NUMERICAL_H
#define HELPERS_STRINGCONVERTER_NUMERICAL_H

#include <Arduino.h>

/********************************************************************************************\
   Convert a char string to integer
 \*********************************************************************************************/

// FIXME: change original code so it uses String and String.toInt()
unsigned long str2int(const char *string);

/********************************************************************************************\
   Check if valid float and convert string to float.
 \*********************************************************************************************/
bool          string2float(const String& string,
                           float       & floatvalue);

/*********************************************************************************************\
   Workaround for removing trailing white space when String() converts a float with 0 decimals
\*********************************************************************************************/
String toString(const float& value,
                unsigned int decimalPlaces = 2);

String ull2String(uint64_t value,
                  uint8_t  base = 10);

String ll2String(int64_t value,
                 uint8_t base = 10);

String doubleToString(const double& value,
                      unsigned int  decimalPlaces     = 2,
                      bool          trimTrailingZeros = false);

#endif // ifndef HELPERS_STRINGCONVERTER_NUMERICAL_H

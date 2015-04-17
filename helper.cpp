#include "helper.h"


#include <stdio.h>
#include <math.h>

#define MICRO "µ"
#define PREFIX_START (-24)
/* Smallest power of then for which there is a prefix defined.
   If the set of prefixes will be extended, change this constant
   and update the table "prefix". */

Helper::Helper()
{

}

Helper::~Helper()
{

}

/**
 * @brief Helper::double2eng
 * @param value input number
 * @param digits number of significant digits to round to
 * @param numeric Replace SI-Prefix with En notation
 * @return formated string
 *
 * Converts the incomming double value to an enigneering notation.
 * If /p numeric == true, the format will be "#.#e3", if false,
 * "#.# k".
 */
char* Helper::double2eng (double value, int digits, bool numeric)
{
  char *prefix[] = {
  "y", "z", "a", "f", "p", "n", MICRO, "m", "",
  "k", "M", "G", "T", "P", "E", "Z", "Y"
  };
#define PREFIX_END (PREFIX_START+(int)((sizeof(prefix)/sizeof(char *)-1)*3))

      int expof10;
      static unsigned char result[100];
      unsigned char *res = result;

      if (value < 0.)
        {
            *res++ = '-';
            value = -value;
        }
      if (value == 0.)
        {
        return "0.0";
        }

      expof10 = (int) log10(value);
      if(expof10 > 0)
        expof10 = (expof10/3)*3;
      else
        expof10 = (-expof10+3)/3*(-3);

      value *= pow(10,-expof10);

      if (value >= 1000.)
         { value /= 1000.0; expof10 += 3; }
      else if(value >= 100.0)
         digits -= 2;
      else if(value >= 10.0)
         digits -= 1;

      if(numeric || (expof10 < PREFIX_START) ||
                    (expof10 > PREFIX_END))
        sprintf(res, "%.*fe%d", digits-1, value, expof10);
      else
        sprintf(res, "%.*f %s", digits-1, value,
          prefix[(expof10-PREFIX_START)/3]);
      return result;
}

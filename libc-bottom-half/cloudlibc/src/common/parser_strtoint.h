// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

// Parser of integer literals as performed by strtol(), scanf(), etc.

// Result of parsing.
bool have_number = false;
bool have_overflow = false;
int_t number = 0;

{
  // Negative or positive number?
  bool negative = false;
  if (allow_negative && PEEK(0) == '-') {
    negative = true;
    SKIP(1);
  } else if (PEEK(0) == '+') {
    SKIP(1);
  }

  // Determine the base.
  if ((base == 0 || base == 16) && PEEK(0) == '0' &&
      (PEEK(1) == 'x' || PEEK(1) == 'X') &&
      ((PEEK(2) >= '0' && PEEK(2) <= '9') ||
       (PEEK(2) >= 'A' && PEEK(2) <= 'F') ||
       (PEEK(2) >= 'a' && PEEK(2) <= 'f'))) {
    SKIP(2);
    base = 16;
  } else if (base == 0) {
    base = PEEK(0) == '0' ? 8 : 10;
  }

  // Only perform conversion if the base is valid.
  if (base >= 2 && base <= 36) {
    uint_fast8_t radix = base;

    // Determine the highest value up to which we can parse so that the
    // next digit does not cause an overflow.
    uintmax_t ceil;
    uint_fast8_t last_digit;
    if (negative) {
      ceil = -(min / radix);
      last_digit = -(min % radix);
    } else {
      ceil = max / radix;
      last_digit = max % radix;
    }

    uintmax_t value = 0;
    for (;;) {
      // Parse next digit.
      uint_fast8_t digit;
      if (PEEK(0) >= '0' && PEEK(0) <= '9')
        digit = PEEK(0) - '0';
      else if (PEEK(0) >= 'A' && PEEK(0) <= 'Z')
        digit = PEEK(0) - 'A' + 10;
      else if (PEEK(0) >= 'a' && PEEK(0) <= 'z')
        digit = PEEK(0) - 'a' + 10;
      else
        break;
      if (digit >= radix)
        break;
      SKIP(1);

      // Add it to result.
      have_number = true;
      if (value > ceil || (value == ceil && digit > last_digit)) {
        // Addition of the new digit would cause an overflow.
        have_overflow = true;
      } else {
        value = value * radix + digit;
      }
    }

    if (have_overflow) {
      // Set value to min or max depending whether the input is negative
      // and whether the output type is signed.
      number = (int_t)-1 >= 0 || !negative ? max : min;
    } else {
      // Return parsed value.
      number = negative ? -value : value;
    }
  }
}

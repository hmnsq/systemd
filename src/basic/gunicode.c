/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* gunicode.c - Unicode manipulation functions
 *
 *  Copyright (C) 1999, 2000 Tom Tromey
 *  Copyright © 2000, 2005 Red Hat, Inc.
 */

#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "gunicode.h"

#ವ್ಯಾಖ್ಯಾನಿಸು unichar uint32_t

/**
 * g_utf8_prev_char:
 * @p: a pointer to a position within a UTF-8 encoded string
 *
 * Finds the previous UTF-8 character in the string before @p.
 *
 * @p does not have to be at the beginning of a UTF-8 character. No check
 * is made to see if the character found is actually valid other than
 * it starts with an appropriate byte. If @p might be the first
 * character of the string, you must use g_utf8_find_prev_char() instead.
 *
 * Return value: a pointer to the found character.
 **/
ಅಕ್ಷರ *
utf8_prev_char (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p)
{
  ಪ್ರತಿಜ್ಞಾ(p);

  ಗಾಗಿ (;;)
    {
      p--;
      ಆದರೆ ((*p & 0xc0) != 0x80)
        ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)p;
    }
}

ರಚನೆ Interval
{
  unichar ಆರಂಭ, ಕೊನೆ;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
interval_compare (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *elt)
{
  unichar c = (unichar) (ದೀರ್ಘ) (ಕೀಲಿ);
  ರಚನೆ Interval *interval = (ರಚನೆ Interval *)elt;

  ಆದರೆ (c < interval->ಆರಂಭ)
    ಹಿಂತಿರುಗಿ -1;
  ಆದರೆ (c > interval->ಕೊನೆ)
    ಹಿಂತಿರುಗಿ +1;

  ಹಿಂತಿರುಗಿ 0;
}

/*
 * NOTE:
 *
 * The tables for g_unichar_iswide() and g_unichar_iswide_cjk() are
 * generated from the Unicode Character Database's file
 * extracted/DerivedEastAsianWidth.txt using the gen-iswide-table.py
 * in this way:
 *
 *   ./gen-iswide-table.py < path/to/ucd/extracted/DerivedEastAsianWidth.txt | fmt
 *
 * Last update for Unicode 6.0.
 */

/**
 * g_unichar_iswide:
 * @c: a Unicode character
 *
 * Determines if a character is typically rendered in a double-width
 * cell.
 *
 * Return value: %TRUE if the character is wide
 **/
bool
unichar_iswide (unichar c)
{
  /* See NOTE earlier for how to update this table. */
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Interval wide[] = {
    {0x1100, 0x115F}, {0x2329, 0x232A}, {0x2E80, 0x2E99}, {0x2E9B, 0x2EF3},
    {0x2F00, 0x2FD5}, {0x2FF0, 0x2FFB}, {0x3000, 0x303E}, {0x3041, 0x3096},
    {0x3099, 0x30FF}, {0x3105, 0x312D}, {0x3131, 0x318E}, {0x3190, 0x31BA},
    {0x31C0, 0x31E3}, {0x31F0, 0x321E}, {0x3220, 0x3247}, {0x3250, 0x32FE},
    {0x3300, 0x4DBF}, {0x4E00, 0xA48C}, {0xA490, 0xA4C6}, {0xA960, 0xA97C},
    {0xAC00, 0xD7A3}, {0xF900, 0xFAFF}, {0xFE10, 0xFE19}, {0xFE30, 0xFE52},
    {0xFE54, 0xFE66}, {0xFE68, 0xFE6B}, {0xFF01, 0xFF60}, {0xFFE0, 0xFFE6},
    {0x1B000, 0x1B001}, {0x1F200, 0x1F202}, {0x1F210, 0x1F23A},
    {0x1F240, 0x1F248}, {0x1F250, 0x1F251},
    {0x1F300, 0x1F567}, /* Miscellaneous Symbols and Pictographs */
    {0x20000, 0x2FFFD}, {0x30000, 0x3FFFD},
  };

  ಆದರೆ (bsearch ((ಶೂನ್ಯ *)(uintptr_t)c, wide, ELEMENTSOF(wide), ಗಾತ್ರ wide[0],
               interval_compare))
    ಹಿಂತಿರುಗಿ true;

  ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ utf8_skip_ದತ್ತಾಂಶ[256] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

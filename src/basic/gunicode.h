/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* gunicode.h - Unicode manipulation functions
 *
 *  Copyright (C) 1999, 2000 Tom Tromey
 *  Copyright © 2000, 2005 Red Hat, Inc.
 */
#رهنمود once

#ضمن "forward.h"

حرف *utf8_prev_char (ثابت حرف *p);

خارجي ثابت حرف utf8_skip_داده[256];

/**
 * g_utf8_next_char:
 * @p: Pointer to the start of a valid UTF-8 character
 *
 * Skips to the next character in a UTF-8 string. The string must be
 * valid; this macro is as fast as possible, and has no error-checking.
 * You would use this macro to iterate over a string character by
 * character. The macro returns the start of the next UTF-8 character.
 * Before using this macro, use g_utf8_validate() to validate strings
 * that may contain invalid UTF-8.
 */
#تعریف_کن utf8_بعدی_char(p) (حرف *)((p) + utf8_skip_داده[*(ثابت بیعلامت حرف *)(p)])

bool unichar_iswide (uint32_t c);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* gunicode.h - Unicode manipulation functions
 *
 *  Copyright (C) 1999, 2000 Tom Tromey
 *  Copyright © 2000, 2005 Red Hat, Inc.
 */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਅੱਖਰ *utf8_prev_char (ਅਚਲ ਅੱਖਰ *p);

ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ utf8_skip_ਅੰਕੜੇ[256];

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ utf8_ਅਗਲਾ_char(p) (ਅੱਖਰ *)((p) + utf8_skip_ਅੰਕੜੇ[*(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)(p)])

bool unichar_iswide (uint32_t c);

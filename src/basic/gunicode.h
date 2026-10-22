/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* gunicode.h - Unicode manipulation functions
 *
 *  Copyright (C) 1999, 2000 Tom Tromey
 *  Copyright © 2000, 2005 Red Hat, Inc.
 */
#निर्देशः once

#अन्तर्भाव "forward.h"

वर्ण *utf8_पूर्व_char (ध्रुव वर्ण *p);

बाह्य ध्रुव वर्ण utf8_skip_दत्त[256];

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
#परिभाषय utf8_अग्र_char(p) (वर्ण *)((p) + utf8_skip_दत्त[*(ध्रुव अचिह्नित वर्ण *)(p)])

bool unichar_iswide (uint32_t c);

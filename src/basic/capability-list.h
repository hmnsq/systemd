/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#define CAPABILITY_TO_STRING_MAX (2 + 2 + 1)

ಧ್ರುವ ವರ್ಣ* capability_to_name(ಪೂರ್ಣಾಙ್ಕ id);
ಧ್ರುವ ವರ್ಣ* capability_to_string(ಪೂರ್ಣಾಙ್ಕ id, ವರ್ಣ buf[ಸ್ಥಿರ CAPABILITY_TO_STRING_MAX]);
#define CAPABILITY_TO_STRING(id) capability_to_string(id, (ವರ್ಣ[CAPABILITY_TO_STRING_MAX]) {})

ಪೂರ್ಣಾಙ್ಕ capability_from_name(ಧ್ರುವ ವರ್ಣ *name);
unsigned capability_list_length(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ capability_set_to_string(uint64_t set, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ capability_set_to_string_negative(uint64_t set, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ capability_set_to_strv(uint64_t set, ವರ್ಣ ***ret);
ಪೂರ್ಣಾಙ್ಕ capability_set_from_string(ಧ್ರುವ ವರ್ಣ *s, uint64_t *ret);

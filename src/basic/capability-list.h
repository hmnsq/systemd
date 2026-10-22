/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#ವ್ಯಾಖ್ಯಾನಿಸು CAPABILITY_ಗಮ್ಯ_STRING_MAX (2 + 2 + 1)

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* capability_ಗಮ್ಯ_name(ಪೂರ್ಣಾಂಕ id);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* capability_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ id, ಅಕ್ಷರ buf[ಸ್ಥಿರ CAPABILITY_ಗಮ್ಯ_STRING_MAX]);
#ವ್ಯಾಖ್ಯಾನಿಸು CAPABILITY_ಗಮ್ಯ_STRING(id) capability_ಗಮ್ಯ_string(id, (ಅಕ್ಷರ[CAPABILITY_ಗಮ್ಯ_STRING_MAX]) {})

ಪೂರ್ಣಾಂಕ capability_from_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
ಚಿಹ್ನೆರಹಿತ capability_list_ಉದ್ದಳತೆ(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ capability_ಗಣ_ಗಮ್ಯ_string(uint64_t ಗಣ, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ capability_ಗಣ_ಗಮ್ಯ_string_negative(uint64_t ಗಣ, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ capability_ಗಣ_ಗಮ್ಯ_strv(uint64_t ಗಣ, ಅಕ್ಷರ ***ret);
ಪೂರ್ಣಾಂಕ capability_ಗಣ_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uint64_t *ret);

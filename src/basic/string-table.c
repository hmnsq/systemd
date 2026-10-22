/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* string_ಕೋಷ್ಟಕ_lookup_ಗಮ್ಯ_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಕೋಷ್ಟಕ, size_t len, ssize_t i) {
        ಆದರೆ (i < 0 || i >= (ssize_t) len)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(ಕೋಷ್ಟಕ);

        ಹಿಂತಿರುಗಿ ಕೋಷ್ಟಕ[i];
}

ssize_t string_ಕೋಷ್ಟಕ_lookup_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಕೋಷ್ಟಕ, size_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ) {
        assert_return(ಕೋಷ್ಟಕ, -EINVAL);

        ಆದರೆ (!ಕೀಲಿ)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಗಾಗಿ (size_t i = 0; i < len; ++i)
                ಆದರೆ (streq_ptr(ಕೋಷ್ಟಕ[i], ಕೀಲಿ))
                        ಹಿಂತಿರುಗಿ (ssize_t) i;

        ಹಿಂತಿರುಗಿ -EINVAL;
}

ssize_t string_ಕೋಷ್ಟಕ_lookup_from_string_with_boolean(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಕೋಷ್ಟಕ, size_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ssize_t yes) {
        ಆದರೆ (!ಕೀಲಿ)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಪೂರ್ಣಾಂಕ b = parse_boolean(ಕೀಲಿ);
        ಆದರೆ (b == 0)
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (b > 0)
                ಹಿಂತಿರುಗಿ yes;

        ಹಿಂತಿರುಗಿ string_ಕೋಷ್ಟಕ_lookup_from_string(ಕೋಷ್ಟಕ, len, ಕೀಲಿ);
}

ಪೂರ್ಣಾಂಕ string_ಕೋಷ್ಟಕ_lookup_ಗಮ್ಯ_string_fallback(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಕೋಷ್ಟಕ, size_t len, ssize_t i, size_t max, ಅಕ್ಷರ **ret) {
        ಅಕ್ಷರ *s;

        ಪ್ರತಿಜ್ಞಾ(ಕೋಷ್ಟಕ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (i < 0 || i > (ssize_t) max)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಆದರೆ (i < (ssize_t) len && ಕೋಷ್ಟಕ[i])
                s = strdup(ಕೋಷ್ಟಕ[i]);
        ಇಲ್ಲದಿದ್ದರೆ
                s = asprintf_ಸುರಕ್ಷಿತ("%zd", i);
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = s;
        ಹಿಂತಿರುಗಿ 0;
}

ssize_t string_ಕೋಷ್ಟಕ_lookup_from_string_fallback(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಕೋಷ್ಟಕ, size_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t max) {
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ssize_t i = string_ಕೋಷ್ಟಕ_lookup_from_string(ಕೋಷ್ಟಕ, len, s);
        ಆದರೆ (i >= 0)
                ಹಿಂತಿರುಗಿ i;

        ಚಿಹ್ನೆರಹಿತ u;
        ಆದರೆ (ಸುರಕ್ಷಿತ_atou(s, &u) < 0)
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (u > max)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಹಿಂತಿರುಗಿ u;
}

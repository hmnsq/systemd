/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ರಚನೆ strbuf {
        ಅಕ್ಷರ *buf;
        size_t len;
        ರಚನೆ strbuf_ಗಂಟು *ಬೇರು;

        size_t nodes_ಎಣಿಕೆ;
        size_t ಒಳ_ಎಣಿಕೆ;
        size_t ಒಳ_len;
        size_t dedup_len;
        size_t dedup_ಎಣಿಕೆ;
};

ರಚನೆ strbuf_ಗಂಟು {
        size_t ಮೌಲ್ಯ_off;
        size_t ಮೌಲ್ಯ_len;

        ರಚನೆ strbuf_ಮಗು_ನಮೂದು *children;
        uint8_t children_ಎಣಿಕೆ;
};

ರಚನೆ strbuf_ಮಗು_ನಮೂದು {
        uint8_t c;
        ರಚನೆ strbuf_ಗಂಟು *ಮಗು;
};

ರಚನೆ strbuf* strbuf_new(ಶೂನ್ಯ);
ssize_t strbuf_ಕೂಡಿಸು_string_ತುಂಬಿದ(ರಚನೆ strbuf *str, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ssize_t strbuf_ಕೂಡಿಸು_string(ರಚನೆ strbuf *str, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಹಿಂತಿರುಗಿ strbuf_ಕೂಡಿಸು_string_ತುಂಬಿದ(str, s, SIZE_MAX);
}
ಶೂನ್ಯ strbuf_complete(ರಚನೆ strbuf *str);
ರಚನೆ strbuf* strbuf_free(ರಚನೆ strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(ರಚನೆ strbuf*, strbuf_free);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಸಂರಚನಾ strbuf {
        ವರ್ಣ *buf;
        size_t len;
        ಸಂರಚನಾ strbuf_ಗಂಟು *ಬೇರು;

        size_t nodes_ಎಣಿಕೆ;
        size_t in_ಎಣಿಕೆ;
        size_t in_len;
        size_t dedup_len;
        size_t dedup_ಎಣಿಕೆ;
};

ಸಂರಚನಾ strbuf_ಗಂಟು {
        size_t ಮೌಲ್ಯ_off;
        size_t ಮೌಲ್ಯ_len;

        ಸಂರಚನಾ strbuf_child_entry *children;
        uint8_t children_ಎಣಿಕೆ;
};

ಸಂರಚನಾ strbuf_child_entry {
        uint8_t c;
        ಸಂರಚನಾ strbuf_ಗಂಟು *child;
};

ಸಂರಚನಾ strbuf* strbuf_new(ಶೂನ್ಯ);
ssize_t strbuf_add_string_full(ಸಂರಚನಾ strbuf *str, ಧ್ರುವ ವರ್ಣ *s, size_t len);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ssize_t strbuf_add_string(ಸಂರಚನಾ strbuf *str, ಧ್ರುವ ವರ್ಣ *s) {
        ಪ್ರತ್ಯಾಗಮ strbuf_add_string_full(str, s, SIZE_MAX);
}
ಶೂನ್ಯ strbuf_complete(ಸಂರಚನಾ strbuf *str);
ಸಂರಚನಾ strbuf* strbuf_free(ಸಂರಚನಾ strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(ಸಂರಚನಾ strbuf*, strbuf_free);

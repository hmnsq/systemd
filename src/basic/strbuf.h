/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਬਣਤਰ strbuf {
        ਅੱਖਰ *buf;
        size_t len;
        ਬਣਤਰ strbuf_ਗੰਢ *ਜੜ੍ਹ;

        size_t nodes_ਗਿਣਤੀ;
        size_t ਵਿੱਚ_ਗਿਣਤੀ;
        size_t ਵਿੱਚ_len;
        size_t dedup_len;
        size_t dedup_ਗਿਣਤੀ;
};

ਬਣਤਰ strbuf_ਗੰਢ {
        size_t ਮੁੱਲ_off;
        size_t ਮੁੱਲ_len;

        ਬਣਤਰ strbuf_ਬੱਚਾ_ਇੰਦਰਾਜ *children;
        uint8_t children_ਗਿਣਤੀ;
};

ਬਣਤਰ strbuf_ਬੱਚਾ_ਇੰਦਰਾਜ {
        uint8_t c;
        ਬਣਤਰ strbuf_ਗੰਢ *ਬੱਚਾ;
};

ਬਣਤਰ strbuf* strbuf_new(ਖਾਲੀ);
ssize_t strbuf_ਜੋੜੋ_string_ਭਰਿਆ(ਬਣਤਰ strbuf *str, ਅਚਲ ਅੱਖਰ *s, size_t len);
ਸਥਿਰ ਸਤਰਅੰਦਰ ssize_t strbuf_ਜੋੜੋ_string(ਬਣਤਰ strbuf *str, ਅਚਲ ਅੱਖਰ *s) {
        ਵਾਪਸ strbuf_ਜੋੜੋ_string_ਭਰਿਆ(str, s, SIZE_MAX);
}
ਖਾਲੀ strbuf_complete(ਬਣਤਰ strbuf *str);
ਬਣਤਰ strbuf* strbuf_free(ਬਣਤਰ strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(ਬਣਤਰ strbuf*, strbuf_free);

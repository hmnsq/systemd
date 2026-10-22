/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

ساخت strbuf {
        حرف *buf;
        size_t len;
        ساخت strbuf_گرہ *جڑ;

        size_t nodes_گنتی;
        size_t اندر_گنتی;
        size_t اندر_len;
        size_t dedup_len;
        size_t dedup_گنتی;
};

ساخت strbuf_گرہ {
        size_t قدر_off;
        size_t قدر_len;

        ساخت strbuf_بچہ_اندراج *children;
        uint8_t children_گنتی;
};

ساخت strbuf_بچہ_اندراج {
        uint8_t c;
        ساخت strbuf_گرہ *بچہ;
};

ساخت strbuf* strbuf_new(خالی);
ssize_t strbuf_جوڑو_string_بھرا(ساخت strbuf *str, ثابت حرف *s, size_t len);
ساکن اندرونی ssize_t strbuf_جوڑو_string(ساخت strbuf *str, ثابت حرف *s) {
        واپس strbuf_جوڑو_string_بھرا(str, s, SIZE_MAX);
}
خالی strbuf_complete(ساخت strbuf *str);
ساخت strbuf* strbuf_free(ساخت strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(ساخت strbuf*, strbuf_free);

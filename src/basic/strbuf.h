/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

بنية strbuf {
        حرف *buf;
        size_t len;
        بنية strbuf_گرہ *جڑ;

        size_t nodes_گنتی;
        size_t وچ_گنتی;
        size_t وچ_len;
        size_t dedup_len;
        size_t dedup_گنتی;
};

بنية strbuf_گرہ {
        size_t قدر_off;
        size_t قدر_len;

        بنية strbuf_بچہ_اندراج *children;
        uint8_t children_گنتی;
};

بنية strbuf_بچہ_اندراج {
        uint8_t c;
        بنية strbuf_گرہ *بچہ;
};

بنية strbuf* strbuf_new(فراغ);
ssize_t strbuf_جوڑو_string_بھریا(بنية strbuf *str, ثابت حرف *s, size_t len);
ساكن مضمن ssize_t strbuf_جوڑو_string(بنية strbuf *str, ثابت حرف *s) {
        أرجع strbuf_جوڑو_string_بھریا(str, s, SIZE_MAX);
}
فراغ strbuf_complete(بنية strbuf *str);
بنية strbuf* strbuf_free(بنية strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية strbuf*, strbuf_free);

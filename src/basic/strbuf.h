/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

بنية strbuf {
        حرف *buf;
        size_t len;
        بنية strbuf_ڳنڍ *پاڙ;

        size_t nodes_ڳڻپ;
        size_t اندريون_ڳڻپ;
        size_t اندريون_len;
        size_t dedup_len;
        size_t dedup_ڳڻپ;
};

بنية strbuf_ڳنڍ {
        size_t قدر_off;
        size_t قدر_len;

        بنية strbuf_ٻار_داخلا *children;
        uint8_t children_ڳڻپ;
};

بنية strbuf_ٻار_داخلا {
        uint8_t c;
        بنية strbuf_ڳنڍ *ٻار;
};

بنية strbuf* strbuf_new(فراغ);
ssize_t strbuf_جوڙيو_string_ڀريل(بنية strbuf *str, ثابت حرف *s, size_t len);
ساكن مضمن ssize_t strbuf_جوڙيو_string(بنية strbuf *str, ثابت حرف *s) {
        أرجع strbuf_جوڙيو_string_ڀريل(str, s, SIZE_MAX);
}
فراغ strbuf_complete(بنية strbuf *str);
بنية strbuf* strbuf_free(بنية strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية strbuf*, strbuf_free);

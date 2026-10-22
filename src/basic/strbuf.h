/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

بنية strbuf {
        حرف *buf;
        size_t len;
        بنية strbuf_كلي *سيو;

        size_t nodes_كدي;
        size_t in_كدي;
        size_t in_len;
        size_t dedup_len;
        size_t dedup_كدي;
};

بنية strbuf_كلي {
        size_t درج_off;
        size_t درج_len;

        بنية strbuf_child_entry *children;
        uint8_t children_كدي;
};

بنية strbuf_child_entry {
        uint8_t c;
        بنية strbuf_كلي *child;
};

بنية strbuf* strbuf_new(فراغ);
ssize_t strbuf_add_string_full(بنية strbuf *str, ثابت حرف *s, size_t len);
ساكن مضمن ssize_t strbuf_add_string(بنية strbuf *str, ثابت حرف *s) {
        أرجع strbuf_add_string_full(str, s, SIZE_MAX);
}
فراغ strbuf_complete(بنية strbuf *str);
بنية strbuf* strbuf_free(بنية strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية strbuf*, strbuf_free);

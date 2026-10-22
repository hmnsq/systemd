/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

بنية strbuf {
        حرف *buf;
        size_t len;
        بنية strbuf_گرہ *جڑ;

        size_t nodes_گنتی;
        size_t in_گنتی;
        size_t in_len;
        size_t dedup_len;
        size_t dedup_گنتی;
};

بنية strbuf_گرہ {
        size_t قدر_off;
        size_t قدر_len;

        بنية strbuf_child_entry *children;
        uint8_t children_گنتی;
};

بنية strbuf_child_entry {
        uint8_t c;
        بنية strbuf_گرہ *child;
};

بنية strbuf* strbuf_new(فراغ);
ssize_t strbuf_add_string_full(بنية strbuf *str, ثابت حرف *s, size_t len);
ساكن مضمن ssize_t strbuf_add_string(بنية strbuf *str, ثابت حرف *s) {
        أرجع strbuf_add_string_full(str, s, SIZE_MAX);
}
فراغ strbuf_complete(بنية strbuf *str);
بنية strbuf* strbuf_free(بنية strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية strbuf*, strbuf_free);

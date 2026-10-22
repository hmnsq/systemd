/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

بنية strbuf {
        حرف *buf;
        size_t len;
        بنية strbuf_node *root;

        size_t nodes_count;
        size_t in_count;
        size_t in_len;
        size_t dedup_len;
        size_t dedup_count;
};

بنية strbuf_node {
        size_t value_off;
        size_t value_len;

        بنية strbuf_child_entry *children;
        uint8_t children_count;
};

بنية strbuf_child_entry {
        uint8_t c;
        بنية strbuf_node *child;
};

بنية strbuf* strbuf_new(فراغ);
ssize_t strbuf_add_string_full(بنية strbuf *str, ثابت حرف *s, size_t len);
ساكن مضمن ssize_t strbuf_add_string(بنية strbuf *str, ثابت حرف *s) {
        ܦܢܐ strbuf_add_string_full(str, s, SIZE_MAX);
}
فراغ strbuf_complete(بنية strbuf *str);
بنية strbuf* strbuf_free(بنية strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية strbuf*, strbuf_free);

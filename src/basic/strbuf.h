/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

কাঠামো strbuf {
        অক্ষর *buf;
        size_t len;
        কাঠামো strbuf_গ্রন্থি *মূল;

        size_t nodes_গুনতি;
        size_t in_গুনতি;
        size_t in_len;
        size_t dedup_len;
        size_t dedup_গুনতি;
};

কাঠামো strbuf_গ্রন্থি {
        size_t মান_off;
        size_t মান_len;

        কাঠামো strbuf_child_entry *children;
        uint8_t children_গুনতি;
};

কাঠামো strbuf_child_entry {
        uint8_t c;
        কাঠামো strbuf_গ্রন্থি *child;
};

কাঠামো strbuf* strbuf_new(শূন্য);
ssize_t strbuf_add_string_full(কাঠামো strbuf *str, ধ্রুবক অক্ষর *s, size_t len);
স্থির অন্তঃরেখ ssize_t strbuf_add_string(কাঠামো strbuf *str, ধ্রুবক অক্ষর *s) {
        ফেরত strbuf_add_string_full(str, s, SIZE_MAX);
}
শূন্য strbuf_complete(কাঠামো strbuf *str);
কাঠামো strbuf* strbuf_free(কাঠামো strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(কাঠামো strbuf*, strbuf_free);

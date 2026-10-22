/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

সংরচনা strbuf {
        ৱর্ণ *buf;
        size_t len;
        সংরচনা strbuf_গ্ৰন্থি *মূল;

        size_t nodes_গুনতি;
        size_t in_গুনতি;
        size_t in_len;
        size_t dedup_len;
        size_t dedup_গুনতি;
};

সংরচনা strbuf_গ্ৰন্থি {
        size_t মান_off;
        size_t মান_len;

        সংরচনা strbuf_child_entry *children;
        uint8_t children_গুনতি;
};

সংরচনা strbuf_child_entry {
        uint8_t c;
        সংরচনা strbuf_গ্ৰন্থি *child;
};

সংরচনা strbuf* strbuf_new(শূন্য);
ssize_t strbuf_add_string_full(সংরচনা strbuf *str, ধ্রুৱ ৱর্ণ *s, size_t len);
স্থির অন্তঃস্থ ssize_t strbuf_add_string(সংরচনা strbuf *str, ধ্রুৱ ৱর্ণ *s) {
        ঘূৰাই strbuf_add_string_full(str, s, SIZE_MAX);
}
শূন্য strbuf_complete(সংরচনা strbuf *str);
সংরচনা strbuf* strbuf_free(সংরচনা strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(সংরচনা strbuf*, strbuf_free);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

sa_nracanaa strbuf {
        var_na *buf;
        size_t len;
        sa_nracanaa strbuf_node *root;

        size_t nodes_count;
        size_t in_count;
        size_t in_len;
        size_t dedup_len;
        size_t dedup_count;
};

sa_nracanaa strbuf_node {
        size_t value_off;
        size_t value_len;

        sa_nracanaa strbuf_child_entry *children;
        uint8_t children_count;
};

sa_nracanaa strbuf_child_entry {
        uint8_t c;
        sa_nracanaa strbuf_node *child;
};

sa_nracanaa strbuf* strbuf_new(void);
ssize_t strbuf_add_string_full(sa_nracanaa strbuf *str, const var_na *s, size_t len);
static anta_hstha ssize_t strbuf_add_string(sa_nracanaa strbuf *str, const var_na *s) {
        ត្រឡប់ strbuf_add_string_full(str, s, SIZE_MAX);
}
void strbuf_complete(sa_nracanaa strbuf *str);
sa_nracanaa strbuf* strbuf_free(sa_nracanaa strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(sa_nracanaa strbuf*, strbuf_free);

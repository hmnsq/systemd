/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

ഘടന strbuf {
        അക്ഷരം *buf;
        size_t len;
        ഘടന strbuf_കണ്ണി *വേര്;

        size_t nodes_എണ്ണം;
        size_t in_എണ്ണം;
        size_t in_len;
        size_t dedup_len;
        size_t dedup_എണ്ണം;
};

ഘടന strbuf_കണ്ണി {
        size_t മൂല്യം_off;
        size_t മൂല്യം_len;

        ഘടന strbuf_child_entry *children;
        uint8_t children_എണ്ണം;
};

ഘടന strbuf_child_entry {
        uint8_t c;
        ഘടന strbuf_കണ്ണി *child;
};

ഘടന strbuf* strbuf_new(ശൂന്യം);
ssize_t strbuf_add_string_full(ഘടന strbuf *str, സ്ഥിരം അക്ഷരം *s, size_t len);
സ്ഥിതം വരിയിൽ ssize_t strbuf_add_string(ഘടന strbuf *str, സ്ഥിരം അക്ഷരം *s) {
        തിരികെ strbuf_add_string_full(str, s, SIZE_MAX);
}
ശൂന്യം strbuf_complete(ഘടന strbuf *str);
ഘടന strbuf* strbuf_free(ഘടന strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(ഘടന strbuf*, strbuf_free);

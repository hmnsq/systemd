/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

ساختار strbuf {
        نویسه *buf;
        size_t len;
        ساختار strbuf_گره *ریشه;

        size_t nodes_شمار;
        size_t in_شمار;
        size_t in_len;
        size_t dedup_len;
        size_t dedup_شمار;
};

ساختار strbuf_گره {
        size_t مقدار_off;
        size_t مقدار_len;

        ساختار strbuf_child_entry *children;
        uint8_t children_شمار;
};

ساختار strbuf_child_entry {
        uint8_t c;
        ساختار strbuf_گره *child;
};

ساختار strbuf* strbuf_new(تهی);
ssize_t strbuf_add_string_full(ساختار strbuf *str, ثابت نویسه *s, size_t len);
ایستا درونخطی ssize_t strbuf_add_string(ساختار strbuf *str, ثابت نویسه *s) {
        بازگشت strbuf_add_string_full(str, s, SIZE_MAX);
}
تهی strbuf_complete(ساختار strbuf *str);
ساختار strbuf* strbuf_free(ساختار strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(ساختار strbuf*, strbuf_free);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

بنية strbuf {
        حرف *buf;
        size_t len;
        بنية strbuf_عقدة *جذر;

        size_t nodes_تعداد;
        size_t in_تعداد;
        size_t in_len;
        size_t dedup_len;
        size_t dedup_تعداد;
};

بنية strbuf_عقدة {
        size_t قيمة_off;
        size_t قيمة_len;

        بنية strbuf_child_entry *children;
        uint8_t children_تعداد;
};

بنية strbuf_child_entry {
        uint8_t c;
        بنية strbuf_عقدة *child;
};

بنية strbuf* strbuf_new(فراغ);
ssize_t strbuf_add_string_full(بنية strbuf *str, ثابت حرف *s, size_t len);
ساكن مضمن ssize_t strbuf_add_string(بنية strbuf *str, ثابت حرف *s) {
        إرجاع strbuf_add_string_full(str, s, SIZE_MAX);
}
فراغ strbuf_complete(بنية strbuf *str);
بنية strbuf* strbuf_free(بنية strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية strbuf*, strbuf_free);

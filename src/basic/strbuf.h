/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

கட்டமைப்பு strbuf {
        எழுத்து *buf;
        size_t len;
        கட்டமைப்பு strbuf_முனை *வேர்;

        size_t nodes_மொத்தம்;
        size_t in_மொத்தம்;
        size_t in_len;
        size_t dedup_len;
        size_t dedup_மொத்தம்;
};

கட்டமைப்பு strbuf_முனை {
        size_t மதிப்பு_off;
        size_t மதிப்பு_len;

        கட்டமைப்பு strbuf_child_entry *children;
        uint8_t children_மொத்தம்;
};

கட்டமைப்பு strbuf_child_entry {
        uint8_t c;
        கட்டமைப்பு strbuf_முனை *child;
};

கட்டமைப்பு strbuf* strbuf_new(வெறுமை);
ssize_t strbuf_add_string_full(கட்டமைப்பு strbuf *str, மாறிலி எழுத்து *s, size_t len);
நிலையான உள்வரி ssize_t strbuf_add_string(கட்டமைப்பு strbuf *str, மாறிலி எழுத்து *s) {
        திரும்பு strbuf_add_string_full(str, s, SIZE_MAX);
}
வெறுமை strbuf_complete(கட்டமைப்பு strbuf *str);
கட்டமைப்பு strbuf* strbuf_free(கட்டமைப்பு strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(கட்டமைப்பு strbuf*, strbuf_free);

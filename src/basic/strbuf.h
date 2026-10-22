/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

संरचना strbuf {
        वर्ण *buf;
        size_t len;
        संरचना strbuf_गाँठ *मूल;

        size_t nodes_गिनती;
        size_t in_गिनती;
        size_t in_len;
        size_t dedup_len;
        size_t dedup_गिनती;
};

संरचना strbuf_गाँठ {
        size_t मूल्य_off;
        size_t मूल्य_len;

        संरचना strbuf_child_entry *children;
        uint8_t children_गिनती;
};

संरचना strbuf_child_entry {
        uint8_t c;
        संरचना strbuf_गाँठ *child;
};

संरचना strbuf* strbuf_new(शून्य);
ssize_t strbuf_add_string_full(संरचना strbuf *str, ध्रुव वर्ण *s, size_t len);
स्थिर अन्तःस्थ ssize_t strbuf_add_string(संरचना strbuf *str, ध्रुव वर्ण *s) {
        प्रत्यागम strbuf_add_string_full(str, s, SIZE_MAX);
}
शून्य strbuf_complete(संरचना strbuf *str);
संरचना strbuf* strbuf_free(संरचना strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(संरचना strbuf*, strbuf_free);

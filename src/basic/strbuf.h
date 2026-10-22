/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

संरचना strbuf {
        वर्ण *buf;
        माप_प्रकार len;
        संरचना strbuf_गाँठ *मूल;

        माप_प्रकार nodes_गिनती;
        माप_प्रकार in_गिनती;
        माप_प्रकार in_len;
        माप_प्रकार dedup_len;
        माप_प्रकार dedup_गिनती;
};

संरचना strbuf_गाँठ {
        माप_प्रकार मूल्य_off;
        माप_प्रकार मूल्य_len;

        संरचना strbuf_child_entry *children;
        uint8_t children_गिनती;
};

संरचना strbuf_child_entry {
        uint8_t c;
        संरचना strbuf_गाँठ *child;
};

संरचना strbuf* strbuf_new(शून्य);
ssize_t strbuf_add_string_full(संरचना strbuf *str, ध्रुव वर्ण *s, माप_प्रकार len);
स्थिर अन्तःस्थ ssize_t strbuf_add_string(संरचना strbuf *str, ध्रुव वर्ण *s) {
        प्रत्यागम strbuf_add_string_full(str, s, SIZE_MAX);
}
शून्य strbuf_complete(संरचना strbuf *str);
संरचना strbuf* strbuf_free(संरचना strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(संरचना strbuf*, strbuf_free);

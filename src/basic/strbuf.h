/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

संरचना strbuf {
        अक्षर *buf;
        माप_प्रकार len;
        संरचना strbuf_गाठ *मूळ;

        माप_प्रकार nodes_मोजणी;
        माप_प्रकार in_मोजणी;
        माप_प्रकार in_len;
        माप_प्रकार dedup_len;
        माप_प्रकार dedup_मोजणी;
};

संरचना strbuf_गाठ {
        माप_प्रकार मूल्य_off;
        माप_प्रकार मूल्य_len;

        संरचना strbuf_child_entry *children;
        uint8_t children_मोजणी;
};

संरचना strbuf_child_entry {
        uint8_t c;
        संरचना strbuf_गाठ *child;
};

संरचना strbuf* strbuf_new(शून्य);
ssize_t strbuf_add_string_full(संरचना strbuf *str, स्थिरांक अक्षर *s, माप_प्रकार len);
स्थिर ओळीत ssize_t strbuf_add_string(संरचना strbuf *str, स्थिरांक अक्षर *s) {
        परत strbuf_add_string_full(str, s, SIZE_MAX);
}
शून्य strbuf_complete(संरचना strbuf *str);
संरचना strbuf* strbuf_free(संरचना strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(संरचना strbuf*, strbuf_free);

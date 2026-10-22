/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

संरचना strbuf {
        वर्ण *buf;
        माप_प्रकार len;
        संरचना strbuf_गाँठो *जरा;

        माप_प्रकार nodes_गन्ती;
        माप_प्रकार in_गन्ती;
        माप_प्रकार in_len;
        माप_प्रकार dedup_len;
        माप_प्रकार dedup_गन्ती;
};

संरचना strbuf_गाँठो {
        माप_प्रकार मान_off;
        माप_प्रकार मान_len;

        संरचना strbuf_child_entry *children;
        uint8_t children_गन्ती;
};

संरचना strbuf_child_entry {
        uint8_t c;
        संरचना strbuf_गाँठो *child;
};

संरचना strbuf* strbuf_new(शून्य);
ssize_t strbuf_add_string_full(संरचना strbuf *str, अचर वर्ण *s, माप_प्रकार len);
स्थिर अन्तःस्थ ssize_t strbuf_add_string(संरचना strbuf *str, अचर वर्ण *s) {
        फर्काऊ strbuf_add_string_full(str, s, SIZE_MAX);
}
शून्य strbuf_complete(संरचना strbuf *str);
संरचना strbuf* strbuf_free(संरचना strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(संरचना strbuf*, strbuf_free);

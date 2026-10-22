/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

काष्ठा strbuf {
        अक्षर *buf;
        माप_प्रकार len;
        काष्ठा strbuf_गाँठ *मूल;

        माप_प्रकार nodes_गिनती;
        माप_प्रकार in_गिनती;
        माप_प्रकार in_len;
        माप_प्रकार dedup_len;
        माप_प्रकार dedup_गिनती;
};

काष्ठा strbuf_गाँठ {
        माप_प्रकार मूल्य_off;
        माप_प्रकार मूल्य_len;

        काष्ठा strbuf_child_entry *children;
        uint8_t children_गिनती;
};

काष्ठा strbuf_child_entry {
        uint8_t c;
        काष्ठा strbuf_गाँठ *child;
};

काष्ठा strbuf* strbuf_new(व्योम);
ssize_t strbuf_add_string_full(काष्ठा strbuf *str, स्थिर अक्षर *s, माप_प्रकार len);
अटल अंतरभूत ssize_t strbuf_add_string(काष्ठा strbuf *str, स्थिर अक्षर *s) {
        वापस strbuf_add_string_full(str, s, SIZE_MAX);
}
व्योम strbuf_complete(काष्ठा strbuf *str);
काष्ठा strbuf* strbuf_free(काष्ठा strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(काष्ठा strbuf*, strbuf_free);

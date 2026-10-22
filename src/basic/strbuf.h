/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ସଂରଚନା strbuf {
        ଅକ୍ଷର *buf;
        size_t len;
        ସଂରଚନା strbuf_ଗଣ୍ଠି *ମୂଳ;

        size_t nodes_ଗଣନା;
        size_t in_ଗଣନା;
        size_t in_len;
        size_t dedup_len;
        size_t dedup_ଗଣନା;
};

ସଂରଚନା strbuf_ଗଣ୍ଠି {
        size_t ମୂଲ୍ୟ_off;
        size_t ମୂଲ୍ୟ_len;

        ସଂରଚନା strbuf_child_entry *children;
        uint8_t children_ଗଣନା;
};

ସଂରଚନା strbuf_child_entry {
        uint8_t c;
        ସଂରଚନା strbuf_ଗଣ୍ଠି *child;
};

ସଂରଚନା strbuf* strbuf_new(ଶୂନ୍ୟ);
ssize_t strbuf_add_string_full(ସଂରଚନା strbuf *str, ଅଚଳ ଅକ୍ଷର *s, size_t len);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ssize_t strbuf_add_string(ସଂରଚନା strbuf *str, ଅଚଳ ଅକ୍ଷର *s) {
        ଫେରନ୍ତୁ strbuf_add_string_full(str, s, SIZE_MAX);
}
ଶୂନ୍ୟ strbuf_complete(ସଂରଚନା strbuf *str);
ସଂରଚନା strbuf* strbuf_free(ସଂରଚନା strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(ସଂରଚନା strbuf*, strbuf_free);

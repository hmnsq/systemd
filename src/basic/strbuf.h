/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

સંરચના strbuf {
        અક્ષર *buf;
        size_t len;
        સંરચના strbuf_ગાંઠ *મૂળ;

        size_t nodes_ગણતરી;
        size_t in_ગણતરી;
        size_t in_len;
        size_t dedup_len;
        size_t dedup_ગણતરી;
};

સંરચના strbuf_ગાંઠ {
        size_t કિંમત_off;
        size_t કિંમત_len;

        સંરચના strbuf_child_entry *children;
        uint8_t children_ગણતરી;
};

સંરચના strbuf_child_entry {
        uint8_t c;
        સંરચના strbuf_ગાંઠ *child;
};

સંરચના strbuf* strbuf_new(શૂન્ય);
ssize_t strbuf_add_string_full(સંરચના strbuf *str, અચલ અક્ષર *s, size_t len);
સ્થિર અંતઃરેખ ssize_t strbuf_add_string(સંરચના strbuf *str, અચલ અક્ષર *s) {
        પાછા strbuf_add_string_full(str, s, SIZE_MAX);
}
શૂન્ય strbuf_complete(સંરચના strbuf *str);
સંરચના strbuf* strbuf_free(સંરચના strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(સંરચના strbuf*, strbuf_free);

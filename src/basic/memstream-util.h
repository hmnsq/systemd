/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം ഘടന MemStream {
        FILE *f;
        അക്ഷരം *buf;
        size_t sz;
} MemStream;

ശൂന്യം memstream_done(MemStream *m);
FILE* memstream_init(MemStream *m);
പൂർണ്ണസംഖ്യ memstream_finalize(MemStream *m, അക്ഷരം **ret_buf, size_t *ret_size);

/* This finalizes the passed memstream. */
പൂർണ്ണസംഖ്യ memstream_dump_internal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

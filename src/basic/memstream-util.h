/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef struct MemStream {
        FILE *f;
        char *buf;
        size_t sz;
} MemStream;

void memstream_done(MemStream *m);
FILE* memstream_init(MemStream *m);
số_nguyên memstream_finalize(MemStream *m, char **ret_buf, size_t *ret_size);

/* This finalizes the passed memstream. */
số_nguyên memstream_dump_internal(
                số_nguyên level,
                số_nguyên error,
                const char *file,
                số_nguyên line,
                const char *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

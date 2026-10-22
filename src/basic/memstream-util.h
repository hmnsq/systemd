/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef sa_nracanaa MemStream {
        FILE *f;
        var_na *buf;
        size_t sz;
} MemStream;

void memstream_done(MemStream *m);
FILE* memstream_init(MemStream *m);
puur_naa_nka memstream_finalize(MemStream *m, var_na **ret_buf, size_t *ret_size);

/* This finalizes the passed memstream. */
puur_naa_nka memstream_dump_internal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

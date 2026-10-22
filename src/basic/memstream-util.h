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
ຈຳນວນເຕັມ memstream_finalize(MemStream *m, var_na **ret_buf, size_t *ret_size);

/* This finalizes the passed memstream. */
ຈຳນວນເຕັມ memstream_dump_internal(
                ຈຳນວນເຕັມ level,
                ຈຳນວນເຕັມ error,
                const var_na *file,
                ຈຳນວນເຕັມ line,
                const var_na *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

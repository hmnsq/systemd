/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

تعريفنوع بنية MemStream {
        FILE *f;
        حرف *buf;
        size_t sz;
} MemStream;

فراغ memstream_done(MemStream *m);
FILE* memstream_init(MemStream *m);
صحيح memstream_finalize(MemStream *m, حرف **ret_buf, size_t *ret_size);

/* This finalizes the passed memstream. */
صحيح memstream_dump_internal(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح line,
                ثابت حرف *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

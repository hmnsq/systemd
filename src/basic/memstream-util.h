/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

تعریفنوع ساختار MemStream {
        FILE *f;
        نویسه *buf;
        size_t sz;
} MemStream;

تهی memstream_done(MemStream *m);
FILE* memstream_init(MemStream *m);
صحیح memstream_finalize(MemStream *m, نویسه **ret_buf, size_t *ret_size);

/* This finalizes the passed memstream. */
صحیح memstream_dump_internal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

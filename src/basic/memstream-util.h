/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை கட்டமைப்பு MemStream {
        FILE *f;
        எழுத்து *buf;
        size_t sz;
} MemStream;

வெறுமை memstream_done(MemStream *m);
FILE* memstream_init(MemStream *m);
முழு memstream_finalize(MemStream *m, எழுத்து **ret_buf, size_t *ret_size);

/* This finalizes the passed memstream. */
முழு memstream_dump_internal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারলেখ কাঠামো MemStream {
        FILE *f;
        অক্ষর *buf;
        size_t sz;
} MemStream;

শূন্য memstream_done(MemStream *m);
FILE* memstream_init(MemStream *m);
পূর্ণসংখ্যা memstream_finalize(MemStream *m, অক্ষর **ret_buf, size_t *ret_আকার);

/* This finalizes the passed memstream. */
পূর্ণসংখ্যা memstream_dump_internal(
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

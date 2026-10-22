/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারনির্দেশ সংরচনা MemStream {
        FILE *f;
        বর্ণ *buf;
        size_t sz;
} MemStream;

শূন্য memstream_done(MemStream *m);
FILE* memstream_init(MemStream *m);
পূর্ণাঙ্ক memstream_finalize(MemStream *m, বর্ণ **ret_buf, size_t *ret_size);

/* This finalizes the passed memstream. */
পূর্ণাঙ্ক memstream_dump_internal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুব বর্ণ *file,
                পূর্ণাঙ্ক সারি,
                ধ্রুব বর্ণ *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

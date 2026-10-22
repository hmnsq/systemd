/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା MemStream {
        FILE *f;
        ଅକ୍ଷର *buf;
        size_t sz;
} MemStream;

ଶୂନ୍ୟ memstream_done(MemStream *m);
FILE* memstream_init(MemStream *m);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା memstream_finalize(MemStream *m, ଅକ୍ଷର **ret_buf, size_t *ret_size);

/* This finalizes the passed memstream. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା memstream_dump_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

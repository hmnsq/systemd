/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

تعريفنوع بنية Mem_دھارا {
        FILE *f;
        حرف *buf;
        size_t sz;
} Mem_دھارا;

فراغ memstream_پورا(Mem_دھارا *m);
FILE* memstream_init(Mem_دھارا *m);
صحيح memstream_finalize(Mem_دھارا *m, حرف **ret_buf, size_t *ret_ناپ);

/* This finalizes the passed memstream. */
صحيح memstream_dump_اندرونی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                Mem_دھارا *m);
#تعریف_کرو memstream_dump(level, m)                                        \
        memstream_dump_اندرونی(level, 0, PROJECT_FILE, __LINE__, __func__, m)

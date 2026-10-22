/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

تعريفنوع بنية Mem_وهڪرو {
        FILE *f;
        حرف *buf;
        size_t sz;
} Mem_وهڪرو;

فراغ memstream_پورو(Mem_وهڪرو *m);
FILE* memstream_init(Mem_وهڪرو *m);
صحيح memstream_finalize(Mem_وهڪرو *m, حرف **ret_buf, size_t *ret_سائيز);

/* This finalizes the passed memstream. */
صحيح memstream_dump_اندروني(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سٽ,
                ثابت حرف *func,
                Mem_وهڪرو *m);
#وصف_ڪريو memstream_dump(level, m)                                        \
        memstream_dump_اندروني(level, 0, PROJECT_FILE, __LINE__, __func__, m)

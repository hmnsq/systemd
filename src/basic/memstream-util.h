/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

تعريفنوع بنية Mem_جریان {
        FILE *f;
        حرف *buf;
        size_t sz;
} Mem_جریان;

فراغ memstream_انجامشده(Mem_جریان *m);
FILE* memstream_init(Mem_جریان *m);
صحيح memstream_finalize(Mem_جریان *m, حرف **ret_buf, size_t *ret_اندازه);

/* This finalizes the passed memstream. */
صحيح memstream_dump_داخلی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                Mem_جریان *m);
#تعریف_کن memstream_dump(level, m)                                        \
        memstream_dump_داخلی(level, 0, PROJECT_FILE, __LINE__, __func__, m)

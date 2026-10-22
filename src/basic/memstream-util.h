/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

تعريفنوع بنية Mem_بهیر {
        FILE *f;
        حرف *buf;
        size_t sz;
} Mem_بهیر;

فراغ memstream_بشپړ(Mem_بهیر *m);
FILE* memstream_init(Mem_بهیر *m);
صحيح memstream_finalize(Mem_بهیر *m, حرف **ret_buf, size_t *ret_اندازه);

/* This finalizes the passed memstream. */
صحيح memstream_dump_دننی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                Mem_بهیر *m);
#تعریف_کړه memstream_dump(level, m)                                        \
        memstream_dump_دننی(level, 0, PROJECT_FILE, __LINE__, __func__, m)

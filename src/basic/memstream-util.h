/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

قسمتعریف ساخت Mem_دھارا {
        FILE *f;
        حرف *buf;
        size_t sz;
} Mem_دھارا;

خالی memstream_مکمل(Mem_دھارا *m);
FILE* memstream_init(Mem_دھارا *m);
صحیح memstream_finalize(Mem_دھارا *m, حرف **ret_buf, size_t *ret_سائز);

/* This finalizes the passed memstream. */
صحیح memstream_dump_اندرونی(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                Mem_دھارا *m);
#تعریف_کرو memstream_dump(level, m)                                        \
        memstream_dump_اندرونی(level, 0, PROJECT_FILE, __LINE__, __func__, m)

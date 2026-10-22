/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "log.h"
#ضمن "memstream-util.h"

فراغ memstream_انجامشده(Mem_جریان *m) {
        أكد(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        امن_fclose(m->f);

        /* Then, free buffer. */
        حرر(m->buf);
}

FILE* memstream_init(Mem_جریان *m) {
        أكد(m);
        أكد(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        أرجع m->f;
}

صحيح memstream_finalize(Mem_جریان *m, حرف **ret_buf, size_t *ret_اندازه) {
        صحيح r;

        أكد(m);
        أكد(m->f);
        أكد(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        إذا (r < 0)
                أرجع r;

        m->f = امن_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        إذا (!m->buf)
                أرجع -ENOMEM;

        أكد(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        إذا (ret_اندازه)
                *ret_اندازه = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        أرجع 0;
}

صحيح memstream_dump_داخلی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                Mem_جریان *m) {

        _cleanup_free_ حرف *buf = عدم;
        صحيح r;

        أكد(m);

        r = memstream_finalize(m, &buf, عدم);
        إذا (r < 0)
                أرجع log_پر_errno(level, r, "Failed to flush memstream: %m");

        أرجع log_dump_داخلی(level, error, file, سطر, func, buf);
}

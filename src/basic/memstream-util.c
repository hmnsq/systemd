/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "log.h"
#شامل "memstream-util.h"

تهی memstream_done(MemStream *m) {
        أكد(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        safe_fclose(m->f);

        /* Then, free buffer. */
        آزاد(m->buf);
}

FILE* memstream_init(MemStream *m) {
        أكد(m);
        أكد(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        بازگشت m->f;
}

صحیح memstream_finalize(MemStream *m, نویسه **ret_buf, size_t *ret_size) {
        صحیح r;

        أكد(m);
        أكد(m->f);
        أكد(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        اگر (r < 0)
                بازگشت r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        اگر (!m->buf)
                بازگشت -ENOMEM;

        أكد(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        اگر (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        بازگشت 0;
}

صحیح memstream_dump_internal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                MemStream *m) {

        _cleanup_free_ نویسه *buf = پوچ;
        صحیح r;

        أكد(m);

        r = memstream_finalize(m, &buf, پوچ);
        اگر (r < 0)
                بازگشت log_full_errno(level, r, "Failed to flush memstream: %m");

        بازگشت log_dump_internal(level, error, file, سطر, func, buf);
}

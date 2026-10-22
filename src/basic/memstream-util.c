/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "log.h"
#شامل "memstream-util.h"

خالی memstream_مکمل(Mem_دھارا *m) {
        أكد(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        محفوظ_fclose(m->f);

        /* Then, free buffer. */
        آزاد(m->buf);
}

FILE* memstream_init(Mem_دھارا *m) {
        أكد(m);
        أكد(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        واپس m->f;
}

صحیح memstream_finalize(Mem_دھارا *m, حرف **ret_buf, size_t *ret_سائز) {
        صحیح r;

        أكد(m);
        أكد(m->f);
        أكد(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        اگر (r < 0)
                واپس r;

        m->f = محفوظ_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        اگر (!m->buf)
                واپس -ENOMEM;

        أكد(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        اگر (ret_سائز)
                *ret_سائز = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        واپس 0;
}

صحیح memstream_dump_اندرونی(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                Mem_دھارا *m) {

        _cleanup_free_ حرف *buf = تہی;
        صحیح r;

        أكد(m);

        r = memstream_finalize(m, &buf, تہی);
        اگر (r < 0)
                واپس log_بھرا_errno(level, r, "Failed to flush memstream: %m");

        واپس log_dump_اندرونی(level, error, file, سطر, func, buf);
}

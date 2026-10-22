/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memstream-util.h"

ਖਾਲੀ memstream_ਪੂਰਾ(Mem_ਧਾਰਾ *m) {
        ਪ੍ਰਤਿਜ੍ਞਾ(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        ਸੁਰੱਖਿਅਤ_fclose(m->f);

        /* Then, free buffer. */
        ਮੁਕਤ(m->buf);
}

FILE* memstream_init(Mem_ਧਾਰਾ *m) {
        ਪ੍ਰਤਿਜ੍ਞਾ(m);
        ਪ੍ਰਤਿਜ੍ਞਾ(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        ਵਾਪਸ m->f;
}

ਪੂਰਨ memstream_finalize(Mem_ਧਾਰਾ *m, ਅੱਖਰ **ret_buf, size_t *ret_ਮਾਪ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(m);
        ਪ੍ਰਤਿਜ੍ਞਾ(m->f);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        m->f = ਸੁਰੱਖਿਅਤ_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        ਜੇ (!m->buf)
                ਵਾਪਸ -ENOMEM;

        ਪ੍ਰਤਿਜ੍ਞਾ(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        ਜੇ (ret_ਮਾਪ)
                *ret_ਮਾਪ = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        ਵਾਪਸ 0;
}

ਪੂਰਨ memstream_dump_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                Mem_ਧਾਰਾ *m) {

        _cleanup_free_ ਅੱਖਰ *buf = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(m);

        r = memstream_finalize(m, &buf, ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(level, r, "Failed to flush memstream: %m");

        ਵਾਪਸ log_dump_ਅੰਦਰੂਨੀ(level, error, file, ਸਤਰ, func, buf);
}

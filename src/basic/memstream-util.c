/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "memstream-util.h"

શૂન્ય memstream_done(MemStream *m) {
        પ્રતિજ્ઞા(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        safe_fclose(m->f);

        /* Then, free buffer. */
        મુક્ત(m->buf);
}

FILE* memstream_init(MemStream *m) {
        પ્રતિજ્ઞા(m);
        પ્રતિજ્ઞા(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        પાછા m->f;
}

પૂર્ણાંક memstream_finalize(MemStream *m, અક્ષર **ret_buf, size_t *ret_size) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(m);
        પ્રતિજ્ઞા(m->f);
        પ્રતિજ્ઞા(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        જો (r < 0)
                પાછા r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        જો (!m->buf)
                પાછા -ENOMEM;

        પ્રતિજ્ઞા(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        જો (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        પાછા 0;
}

પૂર્ણાંક memstream_dump_internal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                MemStream *m) {

        _cleanup_free_ અક્ષર *buf = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(m);

        r = memstream_finalize(m, &buf, ખાલી);
        જો (r < 0)
                પાછા log_full_errno(level, r, "Failed to flush memstream: %m");

        પાછા log_dump_internal(level, error, file, લીટી, func, buf);
}

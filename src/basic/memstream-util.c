/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memstream-util.h"

ಶೂನ್ಯ memstream_done(MemStream *m) {
        ಪ್ರತಿಜ್ಞಾ(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        safe_fclose(m->f);

        /* Then, free buffer. */
        ಮುಕ್ತ(m->buf);
}

FILE* memstream_init(MemStream *m) {
        ಪ್ರತಿಜ್ಞಾ(m);
        ಪ್ರತಿಜ್ಞಾ(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        ಪ್ರತ್ಯಾಗಮ m->f;
}

ಪೂರ್ಣಾಙ್ಕ memstream_finalize(MemStream *m, ವರ್ಣ **ret_buf, size_t *ret_size) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(m);
        ಪ್ರತಿಜ್ಞಾ(m->f);
        ಪ್ರತಿಜ್ಞಾ(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        ಯದಿ (!m->buf)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತಿಜ್ಞಾ(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        ಯದಿ (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ memstream_dump_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                MemStream *m) {

        _cleanup_free_ ವರ್ಣ *buf = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(m);

        r = memstream_finalize(m, &buf, ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(level, r, "Failed to flush memstream: %m");

        ಪ್ರತ್ಯಾಗಮ log_dump_internal(level, error, file, ಸಾಲು, func, buf);
}

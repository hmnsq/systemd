/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memstream-util.h"

ಶೂನ್ಯ memstream_ಮುಗಿದ(Mem_ಹರಿವು *m) {
        ಪ್ರತಿಜ್ಞಾ(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        ಸುರಕ್ಷಿತ_fclose(m->f);

        /* Then, free buffer. */
        ಬಿಡುಗಡೆ(m->buf);
}

FILE* memstream_init(Mem_ಹರಿವು *m) {
        ಪ್ರತಿಜ್ಞಾ(m);
        ಪ್ರತಿಜ್ಞಾ(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        ಹಿಂತಿರುಗಿ m->f;
}

ಪೂರ್ಣಾಂಕ memstream_finalize(Mem_ಹರಿವು *m, ಅಕ್ಷರ **ret_buf, size_t *ret_ಗಾತ್ರ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(m);
        ಪ್ರತಿಜ್ಞಾ(m->f);
        ಪ್ರತಿಜ್ಞಾ(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        m->f = ಸುರಕ್ಷಿತ_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        ಆದರೆ (!m->buf)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಪ್ರತಿಜ್ಞಾ(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        ಆದರೆ (ret_ಗಾತ್ರ)
                *ret_ಗಾತ್ರ = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ memstream_dump_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                Mem_ಹರಿವು *m) {

        _cleanup_free_ ಅಕ್ಷರ *buf = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(m);

        r = memstream_finalize(m, &buf, ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(level, r, "Failed to flush memstream: %m");

        ಹಿಂತಿರುಗಿ log_dump_ಆಂತರಿಕ(level, error, file, ಸಾಲು, func, buf);
}

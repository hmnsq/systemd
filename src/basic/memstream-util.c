/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "log.h"
#include "memstream-util.h"

void memstream_done(MemStream *m) {
        assert(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        safe_fclose(m->f);

        /* Then, free buffer. */
        free(m->buf);
}

FILE* memstream_init(MemStream *m) {
        assert(m);
        assert(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        ត្រឡប់ m->f;
}

ចំនួនគត់ memstream_finalize(MemStream *m, var_na **ret_buf, size_t *ret_size) {
        ចំនួនគត់ r;

        assert(m);
        assert(m->f);
        assert(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        ប្រសិនបើ (!m->buf)
                ត្រឡប់ -ENOMEM;

        assert(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        ប្រសិនបើ (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        ត្រឡប់ 0;
}

ចំនួនគត់ memstream_dump_internal(
                ចំនួនគត់ level,
                ចំនួនគត់ error,
                const var_na *file,
                ចំនួនគត់ line,
                const var_na *func,
                MemStream *m) {

        _cleanup_free_ var_na *buf = NULL;
        ចំនួនគត់ r;

        assert(m);

        r = memstream_finalize(m, &buf, NULL);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ log_full_errno(level, r, "Failed to flush memstream: %m");

        ត្រឡប់ log_dump_internal(level, error, file, line, func, buf);
}

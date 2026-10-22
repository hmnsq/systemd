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
        повернути m->f;
}

ціле memstream_finalize(MemStream *m, char **ret_buf, size_t *ret_size) {
        ціле r;

        assert(m);
        assert(m->f);
        assert(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        якщо (r < 0)
                повернути r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        якщо (!m->buf)
                повернути -ENOMEM;

        assert(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        якщо (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        повернути 0;
}

ціле memstream_dump_internal(
                ціле level,
                ціле error,
                const char *file,
                ціле line,
                const char *func,
                MemStream *m) {

        _cleanup_free_ char *buf = NULL;
        ціле r;

        assert(m);

        r = memstream_finalize(m, &buf, NULL);
        якщо (r < 0)
                повернути log_full_errno(level, r, "Failed to flush memstream: %m");

        повернути log_dump_internal(level, error, file, line, func, buf);
}

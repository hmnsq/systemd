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
        반환 m->f;
}

정수 memstream_finalize(MemStream *m, char **ret_buf, size_t *ret_size) {
        정수 r;

        assert(m);
        assert(m->f);
        assert(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        만약 (r < 0)
                반환 r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        만약 (!m->buf)
                반환 -ENOMEM;

        assert(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        만약 (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        반환 0;
}

정수 memstream_dump_internal(
                정수 level,
                정수 error,
                const char *file,
                정수 line,
                const char *func,
                MemStream *m) {

        _cleanup_free_ char *buf = NULL;
        정수 r;

        assert(m);

        r = memstream_finalize(m, &buf, NULL);
        만약 (r < 0)
                반환 log_full_errno(level, r, "Failed to flush memstream: %m");

        반환 log_dump_internal(level, error, file, line, func, buf);
}

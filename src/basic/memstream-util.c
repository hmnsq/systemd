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
        返回 m->f;
}

整数 memstream_finalize(MemStream *m, char **ret_buf, size_t *ret_size) {
        整数 r;

        assert(m);
        assert(m->f);
        assert(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        如果 (r < 0)
                返回 r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        如果 (!m->buf)
                返回 -ENOMEM;

        assert(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        如果 (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        返回 0;
}

整数 memstream_dump_internal(
                整数 level,
                整数 error,
                const char *file,
                整数 line,
                const char *func,
                MemStream *m) {

        _cleanup_free_ char *buf = NULL;
        整数 r;

        assert(m);

        r = memstream_finalize(m, &buf, NULL);
        如果 (r < 0)
                返回 log_full_errno(level, r, "Failed to flush memstream: %m");

        返回 log_dump_internal(level, error, file, line, func, buf);
}

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
        врати m->f;
}

цео memstream_finalize(MemStream *m, char **ret_buf, size_t *ret_size) {
        цео r;

        assert(m);
        assert(m->f);
        assert(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        ако (r < 0)
                врати r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        ако (!m->buf)
                врати -ENOMEM;

        assert(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        ако (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        врати 0;
}

цео memstream_dump_internal(
                цео level,
                цео error,
                const char *file,
                цео line,
                const char *func,
                MemStream *m) {

        _cleanup_free_ char *buf = NULL;
        цео r;

        assert(m);

        r = memstream_finalize(m, &buf, NULL);
        ако (r < 0)
                врати log_full_errno(level, r, "Failed to flush memstream: %m");

        врати log_dump_internal(level, error, file, line, func, buf);
}

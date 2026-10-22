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
        ᬯᬮᬶ m->f;
}

puur_naa_nka memstream_finalize(MemStream *m, var_na **ret_buf, size_t *ret_size) {
        puur_naa_nka r;

        assert(m);
        assert(m->f);
        assert(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        ᬬᬾᬦ᭄ (!m->buf)
                ᬯᬮᬶ -ENOMEM;

        assert(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        ᬬᬾᬦ᭄ (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        ᬯᬮᬶ 0;
}

puur_naa_nka memstream_dump_internal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                MemStream *m) {

        _cleanup_free_ var_na *buf = NULL;
        puur_naa_nka r;

        assert(m);

        r = memstream_finalize(m, &buf, NULL);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ log_full_errno(level, r, "Failed to flush memstream: %m");

        ᬯᬮᬶ log_dump_internal(level, error, file, line, func, buf);
}

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
        ສົ່ງຄືນ m->f;
}

ຈຳນວນເຕັມ memstream_finalize(MemStream *m, var_na **ret_buf, size_t *ret_size) {
        ຈຳນວນເຕັມ r;

        assert(m);
        assert(m->f);
        assert(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        ຖ້າ (!m->buf)
                ສົ່ງຄືນ -ENOMEM;

        assert(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        ຖ້າ (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ memstream_dump_internal(
                ຈຳນວນເຕັມ level,
                ຈຳນວນເຕັມ error,
                const var_na *file,
                ຈຳນວນເຕັມ line,
                const var_na *func,
                MemStream *m) {

        _cleanup_free_ var_na *buf = NULL;
        ຈຳນວນເຕັມ r;

        assert(m);

        r = memstream_finalize(m, &buf, NULL);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_full_errno(level, r, "Failed to flush memstream: %m");

        ສົ່ງຄືນ log_dump_internal(level, error, file, line, func, buf);
}

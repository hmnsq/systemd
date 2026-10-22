/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "memstream-util.h"

வெறுமை memstream_done(MemStream *m) {
        ப்ரதிஜ்ஞா(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        safe_fclose(m->f);

        /* Then, free buffer. */
        விடுவி(m->buf);
}

FILE* memstream_init(MemStream *m) {
        ப்ரதிஜ்ஞா(m);
        ப்ரதிஜ்ஞா(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        திரும்பு m->f;
}

முழு memstream_finalize(MemStream *m, எழுத்து **ret_buf, size_t *ret_size) {
        முழு r;

        ப்ரதிஜ்ஞா(m);
        ப்ரதிஜ்ஞா(m->f);
        ப்ரதிஜ்ஞா(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        எனில் (r < 0)
                திரும்பு r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        எனில் (!m->buf)
                திரும்பு -ENOMEM;

        ப்ரதிஜ்ஞா(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        எனில் (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        திரும்பு 0;
}

முழு memstream_dump_internal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                MemStream *m) {

        _cleanup_free_ எழுத்து *buf = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(m);

        r = memstream_finalize(m, &buf, வெற்று);
        எனில் (r < 0)
                திரும்பு log_full_errno(level, r, "Failed to flush memstream: %m");

        திரும்பு log_dump_internal(level, error, file, வரி, func, buf);
}

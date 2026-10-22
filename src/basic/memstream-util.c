/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "fileio.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "memstream-util.h"

শূন্য memstream_done(MemStream *m) {
        প্রতিজ্ঞা(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        safe_fclose(m->f);

        /* Then, free buffer. */
        মুক্ত(m->buf);
}

FILE* memstream_init(MemStream *m) {
        প্রতিজ্ঞা(m);
        প্রতিজ্ঞা(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        ঘূৰাই m->f;
}

পূর্ণাঙ্ক memstream_finalize(MemStream *m, ৱর্ণ **ret_buf, size_t *ret_size) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(m);
        প্রতিজ্ঞা(m->f);
        প্রতিজ্ঞা(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        যদি (r < 0)
                ঘূৰাই r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        যদি (!m->buf)
                ঘূৰাই -ENOMEM;

        প্রতিজ্ঞা(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        যদি (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        ঘূৰাই 0;
}

পূর্ণাঙ্ক memstream_dump_internal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                MemStream *m) {

        _cleanup_free_ ৱর্ণ *buf = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(m);

        r = memstream_finalize(m, &buf, রিক্ত);
        যদি (r < 0)
                ঘূৰাই log_full_errno(level, r, "Failed to flush memstream: %m");

        ঘূৰাই log_dump_internal(level, error, file, সাৰি, func, buf);
}

/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "memstream-util.h"

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
        ফেরত m->f;
}

পূর্ণসংখ্যা memstream_finalize(MemStream *m, অক্ষর **ret_buf, size_t *ret_আকার) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(m);
        প্রতিজ্ঞা(m->f);
        প্রতিজ্ঞা(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        যদি (r < 0)
                ফেরত r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        যদি (!m->buf)
                ফেরত -ENOMEM;

        প্রতিজ্ঞা(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        যদি (ret_আকার)
                *ret_আকার = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        ফেরত 0;
}

পূর্ণসংখ্যা memstream_dump_internal(
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                MemStream *m) {

        _cleanup_free_ অক্ষর *buf = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(m);

        r = memstream_finalize(m, &buf, ফাঁকা);
        যদি (r < 0)
                ফেরত log_full_errno(level, r, "Failed to flush memstream: %m");

        ফেরত log_dump_internal(level, error, file, সারি, func, buf);
}

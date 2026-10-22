/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "memstream-util.h"

ଶୂନ୍ୟ memstream_done(MemStream *m) {
        ପ୍ରତିଜ୍ଞା(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        safe_fclose(m->f);

        /* Then, free buffer. */
        ମୁକ୍ତ(m->buf);
}

FILE* memstream_init(MemStream *m) {
        ପ୍ରତିଜ୍ଞା(m);
        ପ୍ରତିଜ୍ଞା(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        ଫେରନ୍ତୁ m->f;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memstream_finalize(MemStream *m, ଅକ୍ଷର **ret_buf, size_t *ret_size) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(m);
        ପ୍ରତିଜ୍ଞା(m->f);
        ପ୍ରତିଜ୍ଞା(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        ଯଦି (!m->buf)
                ଫେରନ୍ତୁ -ENOMEM;

        ପ୍ରତିଜ୍ଞା(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        ଯଦି (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memstream_dump_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                MemStream *m) {

        _cleanup_free_ ଅକ୍ଷର *buf = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(m);

        r = memstream_finalize(m, &buf, ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_full_errno(level, r, "Failed to flush memstream: %m");

        ଫେରନ୍ତୁ log_dump_internal(level, error, file, ଧାଡ଼ି, func, buf);
}

/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memstream-util.h"

शून्य memstream_समाप्तम्(Mem_प्रवाहः *m) {
        प्रतिज्ञा(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        safe_fclose(m->f);

        /* Then, free buffer. */
        मुक्त(m->buf);
}

FILE* memstream_init(Mem_प्रवाहः *m) {
        प्रतिज्ञा(m);
        प्रतिज्ञा(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        प्रत्यागम m->f;
}

पूर्णाङ्क memstream_finalize(Mem_प्रवाहः *m, वर्ण **ret_buf, size_t *ret_आकार) {
        पूर्णाङ्क r;

        प्रतिज्ञा(m);
        प्रतिज्ञा(m->f);
        प्रतिज्ञा(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        यदि (r < 0)
                प्रत्यागम r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        यदि (!m->buf)
                प्रत्यागम -ENOMEM;

        प्रतिज्ञा(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        यदि (ret_आकार)
                *ret_आकार = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        प्रत्यागम 0;
}

पूर्णाङ्क memstream_dump_आन्तरिकम्(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                Mem_प्रवाहः *m) {

        _cleanup_free_ वर्ण *buf = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(m);

        r = memstream_finalize(m, &buf, रिक्त);
        यदि (r < 0)
                प्रत्यागम log_पूर्णम्_errno(level, r, "Failed to flush memstream: %m");

        प्रत्यागम log_dump_आन्तरिकम्(level, error, file, पङ्क्तिः, func, buf);
}

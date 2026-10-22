/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memstream-util.h"

शून्य memstream_done(MemStream *m) {
        प्रतिज्ञा(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        safe_fclose(m->f);

        /* Then, free buffer. */
        मुक्त(m->buf);
}

खाता* memstream_init(MemStream *m) {
        प्रतिज्ञा(m);
        प्रतिज्ञा(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        परत m->f;
}

पूर्णांक memstream_finalize(MemStream *m, अक्षर **ret_buf, माप_प्रकार *ret_size) {
        पूर्णांक r;

        प्रतिज्ञा(m);
        प्रतिज्ञा(m->f);
        प्रतिज्ञा(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        ख_अक्षर_दो('\0', m->f);

        r = fflush_and_check(m->f);
        जर (r < 0)
                परत r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        जर (!m->buf)
                परत -ENOMEM;

        प्रतिज्ञा(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        जर (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        परत 0;
}

पूर्णांक memstream_dump_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिरांक अक्षर *file,
                पूर्णांक ओळ,
                स्थिरांक अक्षर *func,
                MemStream *m) {

        _cleanup_free_ अक्षर *buf = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(m);

        r = memstream_finalize(m, &buf, रिक्त);
        जर (r < 0)
                परत log_full_errno(level, r, "Failed to flush memstream: %m");

        परत log_dump_internal(level, error, file, ओळ, func, buf);
}

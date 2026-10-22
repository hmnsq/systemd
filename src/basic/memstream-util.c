/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "alloc-util.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "log.h"
#समावेश "memstream-util.h"

व्योम memstream_done(MemStream *m) {
        निश्चित(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        safe_fclose(m->f);

        /* Then, free buffer. */
        मुक्त(m->buf);
}

खाता* memstream_init(MemStream *m) {
        निश्चित(m);
        निश्चित(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        वापस m->f;
}

पूर्णांक memstream_finalize(MemStream *m, अक्षर **ret_buf, माप_प्रकार *ret_आकार) {
        पूर्णांक r;

        निश्चित(m);
        निश्चित(m->f);
        निश्चित(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        ख_अक्षर_दो('\0', m->f);

        r = fflush_and_check(m->f);
        अगर (r < 0)
                वापस r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        अगर (!m->buf)
                वापस -ENOMEM;

        निश्चित(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        अगर (ret_आकार)
                *ret_आकार = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        वापस 0;
}

पूर्णांक memstream_dump_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                MemStream *m) {

        _cleanup_free_ अक्षर *buf = शून्य;
        पूर्णांक r;

        निश्चित(m);

        r = memstream_finalize(m, &buf, शून्य);
        अगर (r < 0)
                वापस log_full_errno(level, r, "Failed to flush memstream: %m");

        वापस log_dump_internal(level, error, file, पंक्ति, func, buf);
}

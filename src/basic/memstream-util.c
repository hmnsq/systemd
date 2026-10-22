/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "memstream-util.h"

శూన్య memstream_పూర్తి(Mem_ప్రవాహం *m) {
        ప్రతిజ్ఞా(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        సురక్షిత_fclose(m->f);

        /* Then, free buffer. */
        విడుదల(m->buf);
}

FILE* memstream_init(Mem_ప్రవాహం *m) {
        ప్రతిజ్ఞా(m);
        ప్రతిజ్ఞా(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        తిరిగి m->f;
}

పూర్ణాంకం memstream_finalize(Mem_ప్రవాహం *m, అక్షరం **ret_buf, size_t *ret_కొలత) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(m);
        ప్రతిజ్ఞా(m->f);
        ప్రతిజ్ఞా(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        ఒకవేళ (r < 0)
                తిరిగి r;

        m->f = సురక్షిత_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        ఒకవేళ (!m->buf)
                తిరిగి -ENOMEM;

        ప్రతిజ్ఞా(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        ఒకవేళ (ret_కొలత)
                *ret_కొలత = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        తిరిగి 0;
}

పూర్ణాంకం memstream_dump_అంతర్గత(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                Mem_ప్రవాహం *m) {

        _cleanup_free_ అక్షరం *buf = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(m);

        r = memstream_finalize(m, &buf, ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి log_నిండిన_errno(level, r, "Failed to flush memstream: %m");

        తిరిగి log_dump_అంతర్గత(level, error, file, పంక్తి, func, buf);
}

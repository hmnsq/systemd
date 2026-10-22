/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "memstream-util.h"

ശൂന്യം memstream_done(MemStream *m) {
        പ്രതിജ്ഞാ(m);

        /* First, close file stream, as the buffer may be reallocated on close. */
        safe_fclose(m->f);

        /* Then, free buffer. */
        സ്വതന്ത്രം(m->buf);
}

FILE* memstream_init(MemStream *m) {
        പ്രതിജ്ഞാ(m);
        പ്രതിജ്ഞാ(!m->f);

        m->f = open_memstream_unlocked(&m->buf, &m->sz);
        തിരികെ m->f;
}

പൂർണ്ണസംഖ്യ memstream_finalize(MemStream *m, അക്ഷരം **ret_buf, size_t *ret_size) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(m);
        പ്രതിജ്ഞാ(m->f);
        പ്രതിജ്ഞാ(ret_buf);

        /* Add terminating NUL, so that the output buffer is a valid string. */
        fputc('\0', m->f);

        r = fflush_and_check(m->f);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        m->f = safe_fclose(m->f);

        /* On fclose(), the buffer may be reallocated, and may trigger OOM. */
        എങ്കിൽ (!m->buf)
                തിരികെ -ENOMEM;

        പ്രതിജ്ഞാ(m->sz > 0);

        *ret_buf = TAKE_PTR(m->buf);
        എങ്കിൽ (ret_size)
                *ret_size = m->sz - 1;

        m->sz = 0; /* For safety when the MemStream object will be reused later. */
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ memstream_dump_internal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                MemStream *m) {

        _cleanup_free_ അക്ഷരം *buf = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(m);

        r = memstream_finalize(m, &buf, ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ log_full_errno(level, r, "Failed to flush memstream: %m");

        തിരികെ log_dump_internal(level, error, file, വരി, func, buf);
}

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Mem_ਧਾਰਾ {
        FILE *f;
        ਅੱਖਰ *buf;
        size_t sz;
} Mem_ਧਾਰਾ;

ਖਾਲੀ memstream_ਪੂਰਾ(Mem_ਧਾਰਾ *m);
FILE* memstream_init(Mem_ਧਾਰਾ *m);
ਪੂਰਨ memstream_finalize(Mem_ਧਾਰਾ *m, ਅੱਖਰ **ret_buf, size_t *ret_ਮਾਪ);

/* This finalizes the passed memstream. */
ਪੂਰਨ memstream_dump_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                Mem_ਧਾਰਾ *m);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ memstream_dump(level, m)                                        \
        memstream_dump_ਅੰਦਰੂਨੀ(level, 0, PROJECT_FILE, __LINE__, __func__, m)

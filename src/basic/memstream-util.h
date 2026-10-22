/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ MemStream {
        FILE *f;
        ವರ್ಣ *buf;
        size_t sz;
} MemStream;

ಶೂನ್ಯ memstream_done(MemStream *m);
FILE* memstream_init(MemStream *m);
ಪೂರ್ಣಾಙ್ಕ memstream_finalize(MemStream *m, ವರ್ಣ **ret_buf, size_t *ret_size);

/* This finalizes the passed memstream. */
ಪೂರ್ಣಾಙ್ಕ memstream_dump_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

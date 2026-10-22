/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Mem_ಹರಿವು {
        FILE *f;
        ಅಕ್ಷರ *buf;
        size_t sz;
} Mem_ಹರಿವು;

ಶೂನ್ಯ memstream_ಮುಗಿದ(Mem_ಹರಿವು *m);
FILE* memstream_init(Mem_ಹರಿವು *m);
ಪೂರ್ಣಾಂಕ memstream_finalize(Mem_ಹರಿವು *m, ಅಕ್ಷರ **ret_buf, size_t *ret_ಗಾತ್ರ);

/* This finalizes the passed memstream. */
ಪೂರ್ಣಾಂಕ memstream_dump_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                Mem_ಹರಿವು *m);
#ವ್ಯಾಖ್ಯಾನಿಸು memstream_dump(level, m)                                        \
        memstream_dump_ಆಂತರಿಕ(level, 0, PROJECT_FILE, __LINE__, __func__, m)

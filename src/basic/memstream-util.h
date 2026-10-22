/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश संरचना MemStream {
        FILE *f;
        वर्ण *buf;
        size_t sz;
} MemStream;

शून्य memstream_done(MemStream *m);
FILE* memstream_init(MemStream *m);
पूर्णाङ्क memstream_finalize(MemStream *m, वर्ण **ret_buf, size_t *ret_size);

/* This finalizes the passed memstream. */
पूर्णाङ्क memstream_dump_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

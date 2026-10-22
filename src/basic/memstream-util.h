/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश संरचना MemStream {
        खाता *f;
        वर्ण *buf;
        माप_प्रकार sz;
} MemStream;

शून्य memstream_done(MemStream *m);
खाता* memstream_init(MemStream *m);
पूर्णाङ्क memstream_finalize(MemStream *m, वर्ण **ret_buf, माप_प्रकार *ret_size);

/* This finalizes the passed memstream. */
पूर्णाङ्क memstream_dump_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

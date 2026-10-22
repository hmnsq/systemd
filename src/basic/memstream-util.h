/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारव्याख्या संरचना MemStream {
        खाता *f;
        अक्षर *buf;
        माप_प्रकार sz;
} MemStream;

शून्य memstream_done(MemStream *m);
खाता* memstream_init(MemStream *m);
पूर्णांक memstream_finalize(MemStream *m, अक्षर **ret_buf, माप_प्रकार *ret_size);

/* This finalizes the passed memstream. */
पूर्णांक memstream_dump_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिरांक अक्षर *file,
                पूर्णांक ओळ,
                स्थिरांक अक्षर *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

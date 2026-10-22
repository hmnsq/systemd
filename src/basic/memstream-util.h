/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

प्रकार काष्ठा MemStream {
        खाता *f;
        अक्षर *buf;
        माप_प्रकार sz;
} MemStream;

व्योम memstream_done(MemStream *m);
खाता* memstream_init(MemStream *m);
पूर्णांक memstream_finalize(MemStream *m, अक्षर **ret_buf, माप_प्रकार *ret_आकार);

/* This finalizes the passed memstream. */
पूर्णांक memstream_dump_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                MemStream *m);
#घोषणा memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

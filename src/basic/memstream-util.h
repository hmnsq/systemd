/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश संरचना Mem_प्रवाहः {
        FILE *f;
        वर्ण *buf;
        size_t sz;
} Mem_प्रवाहः;

शून्य memstream_समाप्तम्(Mem_प्रवाहः *m);
FILE* memstream_init(Mem_प्रवाहः *m);
पूर्णाङ्क memstream_finalize(Mem_प्रवाहः *m, वर्ण **ret_buf, size_t *ret_आकार);

/* This finalizes the passed memstream. */
पूर्णाङ्क memstream_dump_आन्तरिकम्(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                Mem_प्रवाहः *m);
#परिभाषय memstream_dump(level, m)                                        \
        memstream_dump_आन्तरिकम्(level, 0, PROJECT_FILE, __LINE__, __func__, m)

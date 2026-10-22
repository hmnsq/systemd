/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા સંરચના MemStream {
        FILE *f;
        અક્ષર *buf;
        size_t sz;
} MemStream;

શૂન્ય memstream_done(MemStream *m);
FILE* memstream_init(MemStream *m);
પૂર્ણાંક memstream_finalize(MemStream *m, અક્ષર **ret_buf, size_t *ret_size);

/* This finalizes the passed memstream. */
પૂર્ણાંક memstream_dump_internal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                MemStream *m);
#define memstream_dump(level, m)                                        \
        memstream_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, m)

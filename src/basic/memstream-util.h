/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం నిర్మాణం Mem_ప్రవాహం {
        FILE *f;
        అక్షరం *buf;
        size_t sz;
} Mem_ప్రవాహం;

శూన్య memstream_పూర్తి(Mem_ప్రవాహం *m);
FILE* memstream_init(Mem_ప్రవాహం *m);
పూర్ణాంకం memstream_finalize(Mem_ప్రవాహం *m, అక్షరం **ret_buf, size_t *ret_కొలత);

/* This finalizes the passed memstream. */
పూర్ణాంకం memstream_dump_అంతర్గత(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                Mem_ప్రవాహం *m);
#నిర్వచించు memstream_dump(level, m)                                        \
        memstream_dump_అంతర్గత(level, 0, PROJECT_FILE, __LINE__, __func__, m)

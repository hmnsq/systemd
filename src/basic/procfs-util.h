/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_get_pid_max(uint64_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_get_threads_max(uint64_t *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_tasks_set_limit(uint64_t limit);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_to_nsec(uint64_t ticks);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_cpu_get_usage(nsec_t *ret);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_memory_get_used(uint64_t *ret) {
        ଫେରନ୍ତୁ procfs_memory_get(ଖାଲି, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା convert_meminfo_ମୂଲ୍ୟ_to_uint64_bytes(ଅଚଳ ଅକ୍ଷର *s, uint64_t *ret);

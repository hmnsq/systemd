/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಙ್ಕ procfs_get_pid_max(uint64_t *ret);
ಪೂರ್ಣಾಙ್ಕ procfs_get_threads_max(uint64_t *ret);

ಪೂರ್ಣಾಙ್ಕ procfs_tasks_set_limit(uint64_t limit);
ಪೂರ್ಣಾಙ್ಕ procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_to_nsec(uint64_t ticks);
ಪೂರ್ಣಾಙ್ಕ procfs_cpu_get_usage(nsec_t *ret);

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

ಪೂರ್ಣಾಙ್ಕ procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

ಪೂರ್ಣಾಙ್ಕ procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ procfs_memory_get_used(uint64_t *ret) {
        ಪ್ರತ್ಯಾಗಮ procfs_memory_get(ರಿಕ್ತ, ret);
}

ಪೂರ್ಣಾಙ್ಕ convert_meminfo_ಮೌಲ್ಯ_to_uint64_bytes(ಧ್ರುವ ವರ್ಣ *s, uint64_t *ret);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಂಕ procfs_get_pid_max(uint64_t *ret);
ಪೂರ್ಣಾಂಕ procfs_get_ಎಳೆಗಳು_max(uint64_t *ret);

ಪೂರ್ಣಾಂಕ procfs_tasks_ಗಣ_ಎಲ್ಲೆ(uint64_t limit);
ಪೂರ್ಣಾಂಕ procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_ಗಮ್ಯ_nsec(uint64_t ticks);
ಪೂರ್ಣಾಂಕ procfs_cpu_get_usage(nsec_t *ret);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

ಪೂರ್ಣಾಂಕ procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

ಪೂರ್ಣಾಂಕ procfs_ಸ್ಮರಣೆ_get(uint64_t *ret_ಒಟ್ಟು, uint64_t *ret_ಬಳಸಿದ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ procfs_ಸ್ಮರಣೆ_get_ಬಳಸಿದ(uint64_t *ret) {
        ಹಿಂತಿರುಗಿ procfs_ಸ್ಮರಣೆ_get(ಖಾಲಿ, ret);
}

ಪೂರ್ಣಾಂಕ convert_meminfo_ಮೌಲ್ಯ_ಗಮ್ಯ_uint64_ಬೈಟುಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uint64_t *ret);

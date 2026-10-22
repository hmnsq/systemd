/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

पूर्णाङ्क procfs_get_pid_max(uint64_t *ret);
पूर्णाङ्क procfs_get_threads_max(uint64_t *ret);

पूर्णाङ्क procfs_tasks_set_limit(uint64_t limit);
पूर्णाङ्क procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_to_nsec(uint64_t ticks);
पूर्णाङ्क procfs_cpu_get_usage(nsec_t *ret);

प्रकारनिर्देश संरचना ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t प्रणाली;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

पूर्णाङ्क procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

पूर्णाङ्क procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used);
स्थिर अन्तःस्थ पूर्णाङ्क procfs_memory_get_used(uint64_t *ret) {
        फर्काऊ procfs_memory_get(रिक्त, ret);
}

पूर्णाङ्क convert_meminfo_मान_to_uint64_bytes(अचर वर्ण *s, uint64_t *ret);

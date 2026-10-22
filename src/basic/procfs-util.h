/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

पूर्णांक procfs_get_pid_max(uint64_t *ret);
पूर्णांक procfs_get_threads_max(uint64_t *ret);

पूर्णांक procfs_tasks_set_limit(uint64_t limit);
पूर्णांक procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_to_nsec(uint64_t ticks);
पूर्णांक procfs_cpu_get_usage(nsec_t *ret);

प्रकारव्याख्या संरचना ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t प्रणाली;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

पूर्णांक procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

पूर्णांक procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used);
स्थिर ओळीत पूर्णांक procfs_memory_get_used(uint64_t *ret) {
        परत procfs_memory_get(रिक्त, ret);
}

पूर्णांक convert_meminfo_मूल्य_to_uint64_bytes(स्थिरांक अक्षर *s, uint64_t *ret);

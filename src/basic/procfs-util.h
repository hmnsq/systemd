/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

पूर्णांक procfs_get_pid_max(uint64_t *ret);
पूर्णांक procfs_get_threads_max(uint64_t *ret);

पूर्णांक procfs_tasks_set_limit(uint64_t limit);
पूर्णांक procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_to_nsec(uint64_t ticks);
पूर्णांक procfs_cpu_get_usage(nsec_t *ret);

प्रकार काष्ठा ProcfsCpuTicks {
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

पूर्णांक procfs_memory_get(uint64_t *ret_योग, uint64_t *ret_used);
अटल अंतरभूत पूर्णांक procfs_memory_get_used(uint64_t *ret) {
        वापस procfs_memory_get(शून्य, ret);
}

पूर्णांक convert_meminfo_मूल्य_to_uint64_bytes(स्थिर अक्षर *s, uint64_t *ret);

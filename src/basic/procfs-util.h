/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

முழு procfs_get_pid_max(uint64_t *ret);
முழு procfs_get_threads_max(uint64_t *ret);

முழு procfs_tasks_set_limit(uint64_t limit);
முழு procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_to_nsec(uint64_t ticks);
முழு procfs_cpu_get_usage(nsec_t *ret);

வகைவரை கட்டமைப்பு ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

முழு procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

முழு procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used);
நிலையான உள்வரி முழு procfs_memory_get_used(uint64_t *ret) {
        திரும்பு procfs_memory_get(வெற்று, ret);
}

முழு convert_meminfo_மதிப்பு_to_uint64_bytes(மாறிலி எழுத்து *s, uint64_t *ret);

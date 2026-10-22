/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

صحیح procfs_get_pid_max(uint64_t *ret);
صحیح procfs_get_threads_max(uint64_t *ret);

صحیح procfs_tasks_set_limit(uint64_t limit);
صحیح procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_to_nsec(uint64_t ticks);
صحیح procfs_cpu_get_usage(nsec_t *ret);

تعریفنوع ساختار ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

صحیح procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

صحیح procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used);
ایستا درونخطی صحیح procfs_memory_get_used(uint64_t *ret) {
        بازگشت procfs_memory_get(پوچ, ret);
}

صحیح convert_meminfo_مقدار_to_uint64_bytes(ثابت نویسه *s, uint64_t *ret);

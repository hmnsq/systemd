/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

صحيح procfs_get_pid_max(uint64_t *ret);
صحيح procfs_get_نخها_max(uint64_t *ret);

صحيح procfs_tasks_مجموعه_حد(uint64_t limit);
صحيح procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_مقصد_nsec(uint64_t ticks);
صحيح procfs_cpu_get_usage(nsec_t *ret);

تعريفنوع بنية ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

صحيح procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

صحيح procfs_حافظه_get(uint64_t *ret_مجموع, uint64_t *ret_استفاده_شده);
ساكن مضمن صحيح procfs_حافظه_get_استفاده_شده(uint64_t *ret) {
        أرجع procfs_حافظه_get(عدم, ret);
}

صحيح convert_meminfo_مقدار_مقصد_uint64_بایتها(ثابت حرف *s, uint64_t *ret);

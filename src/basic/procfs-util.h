/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

صحيح procfs_get_pid_max(uint64_t *ret);
صحيح procfs_get_تارونه_max(uint64_t *ret);

صحيح procfs_tasks_سېټ_حد(uint64_t limit);
صحيح procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_منزل_nsec(uint64_t ticks);
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

صحيح procfs_حافظه_get(uint64_t *ret_ټول, uint64_t *ret_کارول_شوی);
ساكن مضمن صحيح procfs_حافظه_get_کارول_شوی(uint64_t *ret) {
        أرجع procfs_حافظه_get(عدم, ret);
}

صحيح convert_meminfo_ارزښت_منزل_uint64_بایټونه(ثابت حرف *s, uint64_t *ret);

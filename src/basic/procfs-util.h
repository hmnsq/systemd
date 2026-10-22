/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

صحیح procfs_get_pid_max(uint64_t *ret);
صحیح procfs_get_دھاگے_max(uint64_t *ret);

صحیح procfs_tasks_سیٹ_حد_بندی(uint64_t limit);
صحیح procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_مقصد_nsec(uint64_t ticks);
صحیح procfs_cpu_get_usage(nsec_t *ret);

قسمتعریف ساخت ProcfsCpuTicks {
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

صحیح procfs_حافظہ_get(uint64_t *ret_کل, uint64_t *ret_مستعمل);
ساکن اندرونی صحیح procfs_حافظہ_get_مستعمل(uint64_t *ret) {
        واپس procfs_حافظہ_get(تہی, ret);
}

صحیح convert_meminfo_قدر_مقصد_uint64_بائٹیں(ثابت حرف *s, uint64_t *ret);

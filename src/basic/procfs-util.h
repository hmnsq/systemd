/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਪੂਰਨ procfs_get_pid_max(uint64_t *ret);
ਪੂਰਨ procfs_get_ਧਾਗੇ_max(uint64_t *ret);

ਪੂਰਨ procfs_tasks_ਸੈੱਟ_ਹੱਦਬੰਦੀ(uint64_t limit);
ਪੂਰਨ procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_ਮੰਜ਼ਿਲ_nsec(uint64_t ticks);
ਪੂਰਨ procfs_cpu_get_usage(nsec_t *ret);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

ਪੂਰਨ procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

ਪੂਰਨ procfs_ਸਮ੍ਰਿਤੀ_get(uint64_t *ret_ਕੁੱਲ, uint64_t *ret_ਵਰਤਿਆ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ procfs_ਸਮ੍ਰਿਤੀ_get_ਵਰਤਿਆ(uint64_t *ret) {
        ਵਾਪਸ procfs_ਸਮ੍ਰਿਤੀ_get(ਨਿਰਾ, ret);
}

ਪੂਰਨ convert_meminfo_ਮੁੱਲ_ਮੰਜ਼ਿਲ_uint64_ਬਾਈਟਾਂ(ਅਚਲ ਅੱਖਰ *s, uint64_t *ret);

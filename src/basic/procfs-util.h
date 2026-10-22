/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

পূর্ণাঙ্ক procfs_get_pid_max(uint64_t *ret);
পূর্ণাঙ্ক procfs_get_threads_max(uint64_t *ret);

পূর্ণাঙ্ক procfs_tasks_set_limit(uint64_t limit);
পূর্ণাঙ্ক procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_to_nsec(uint64_t ticks);
পূর্ণাঙ্ক procfs_cpu_get_usage(nsec_t *ret);

প্রকারনির্দেশ সংরচনা ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

পূর্ণাঙ্ক procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

পূর্ণাঙ্ক procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used);
স্থির অন্তঃস্থ পূর্ণাঙ্ক procfs_memory_get_used(uint64_t *ret) {
        ঘূৰাই procfs_memory_get(রিক্ত, ret);
}

পূর্ণাঙ্ক convert_meminfo_মান_to_uint64_bytes(ধ্রুৱ ৱর্ণ *s, uint64_t *ret);

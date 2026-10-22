/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

പൂർണ്ണസംഖ്യ procfs_get_pid_max(uint64_t *ret);
പൂർണ്ണസംഖ്യ procfs_get_threads_max(uint64_t *ret);

പൂർണ്ണസംഖ്യ procfs_tasks_set_limit(uint64_t limit);
പൂർണ്ണസംഖ്യ procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_to_nsec(uint64_t ticks);
പൂർണ്ണസംഖ്യ procfs_cpu_get_usage(nsec_t *ret);

തരനിർവചനം ഘടന ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

പൂർണ്ണസംഖ്യ procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

പൂർണ്ണസംഖ്യ procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ procfs_memory_get_used(uint64_t *ret) {
        തിരികെ procfs_memory_get(ശൂന്യ, ret);
}

പൂർണ്ണസംഖ്യ convert_meminfo_മൂല്യം_to_uint64_bytes(സ്ഥിരം അക്ഷരം *s, uint64_t *ret);

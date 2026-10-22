/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પૂર્ણાંક procfs_get_pid_max(uint64_t *ret);
પૂર્ણાંક procfs_get_threads_max(uint64_t *ret);

પૂર્ણાંક procfs_tasks_set_limit(uint64_t limit);
પૂર્ણાંક procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_to_nsec(uint64_t ticks);
પૂર્ણાંક procfs_cpu_get_usage(nsec_t *ret);

પ્રકારવ્યાખ્યા સંરચના ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

પૂર્ણાંક procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

પૂર્ણાંક procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used);
સ્થિર અંતઃરેખ પૂર્ણાંક procfs_memory_get_used(uint64_t *ret) {
        પાછા procfs_memory_get(ખાલી, ret);
}

પૂર્ણાંક convert_meminfo_કિંમત_to_uint64_bytes(અચલ અક્ષર *s, uint64_t *ret);

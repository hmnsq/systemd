/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

పూర్ణాంకం procfs_get_pid_max(uint64_t *ret);
పూర్ణాంకం procfs_get_దారాలు_max(uint64_t *ret);

పూర్ణాంకం procfs_tasks_సమితి_హద్దు(uint64_t limit);
పూర్ణాంకం procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_గమ్యం_nsec(uint64_t ticks);
పూర్ణాంకం procfs_cpu_get_usage(nsec_t *ret);

రకనిర్వచనం నిర్మాణం ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

పూర్ణాంకం procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

పూర్ణాంకం procfs_జ్ఞాపకం_get(uint64_t *ret_మొత్తం, uint64_t *ret_వాడిన);
స్థిర అంతఃవరుస పూర్ణాంకం procfs_జ్ఞాపకం_get_వాడిన(uint64_t *ret) {
        తిరిగి procfs_జ్ఞాపకం_get(ఖాళీ, ret);
}

పూర్ణాంకం convert_meminfo_విలువ_గమ్యం_uint64_బైట్లు(స్థిరరాశి అక్షరం *s, uint64_t *ret);

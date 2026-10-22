/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

पूर्णाङ्क procfs_get_pid_max(uint64_t *ret);
पूर्णाङ्क procfs_get_तन्तवः_max(uint64_t *ret);

पूर्णाङ्क procfs_tasks_समुच्चयः_सीमा(uint64_t limit);
पूर्णाङ्क procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_गन्तव्यम्_nsec(uint64_t ticks);
पूर्णाङ्क procfs_cpu_get_usage(nsec_t *ret);

प्रकारनिर्देश संरचना ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

पूर्णाङ्क procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

पूर्णाङ्क procfs_स्मृतिः_get(uint64_t *ret_योग, uint64_t *ret_प्रयुक्तम्);
स्थिर अन्तःस्थ पूर्णाङ्क procfs_स्मृतिः_get_प्रयुक्तम्(uint64_t *ret) {
        प्रत्यागम procfs_स्मृतिः_get(रिक्त, ret);
}

पूर्णाङ्क convert_meminfo_मानम्_गन्तव्यम्_uint64_अष्टकानि(ध्रुव वर्ण *s, uint64_t *ret);

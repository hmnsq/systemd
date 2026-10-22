/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "procfs-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "time-util.h"

पूर्णाङ्क procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ वर्ण *मूल्य = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = read_one_पंक्ति_file("/proc/sys/kernel/pid_max", &मूल्य);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम safe_atou64(मूल्य, ret);
}

पूर्णाङ्क procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ वर्ण *मूल्य = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = read_one_पंक्ति_file("/proc/sys/kernel/threads-max", &मूल्य);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम safe_atou64(मूल्य, ret);
}

पूर्णाङ्क procfs_tasks_set_limit(uint64_t limit) {
        वर्ण buffer[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        पूर्णाङ्क r;

        यदि (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                प्रत्यागम -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        यदि (r < 0)
                प्रत्यागम r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        यदि (limit > pid_max-1) {
                xsprintf(buffer, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        xsprintf(buffer, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
        यदि (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                यदि (procfs_get_threads_max(&threads_max) < 0)
                        प्रत्यागम r; /* return original error */

                यदि (MIN(pid_max - 1, threads_max) != limit)
                        प्रत्यागम r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        प्रत्यागम 0;
}

पूर्णाङ्क procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ वर्ण *मूल्य = रिक्त;
        ध्रुव वर्ण *p, *nr;
        size_t n;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = read_one_पंक्ति_file("/proc/loadavg", &मूल्य);
        यदि (r < 0)
                प्रत्यागम r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(मूल्य, '/');
        यदि (!p)
                प्रत्यागम -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_safe(p, n);

        प्रत्यागम safe_atou64(nr, ret);
}

स्थिर uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        यावत् (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        प्रत्यागम a;
}

nsec_t procfs_ticks_to_nsec(uint64_t ticks) {
        uint64_t ticks_per_second = sysconf_clock_ticks_cached(),
                gcd = calc_gcd64(NSEC_PER_SEC, ticks_per_second);

        /* Reduce NSEC_PER_SEC/ticks_per_second first to avoid overflow of ticks * NSEC_PER_SEC, and split
         * the multiplication into whole and remainder parts, so that the result stays exact.
         *
         * Note that in most cases b == 1 and the second term is trivial. */
        uint64_t a = NSEC_PER_SEC / gcd,
                 b = ticks_per_second / gcd;

        प्रत्यागम ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

पूर्णाङ्क procfs_cpu_get_usage(nsec_t *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        यदि (r < 0)
                प्रत्यागम r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        प्रत्यागम 0;
}

पूर्णाङ्क procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ वर्ण *first_पंक्ति = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = read_one_पंक्ति_file("/proc/stat", &first_पंक्ति);
        यदि (r < 0)
                प्रत्यागम r;

        ध्रुव वर्ण *p = first_word(first_पंक्ति, "cpu");
        यदि (!p)
                प्रत्यागम -EINVAL;

        /* All eight fields exist on any kernel we support: iowait was added in 2.5.41, steal in 2.6.11.
         * "guest" and "guest_nice" are not returned, the kernel already includes them in "user" and "nice".
         * Nevertheless, this file might be synthesized slopilly, so let's not require the last three fields.
         * We don't care about them in most cases anyway. */
        ProcfsCpuTicks t = {};
        r = sscanf(p, "%"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64,
                   &t.user,
                   &t.nice,
                   &t.system,
                   &t.idle,
                   &t.iowait,
                   &t.irq,
                   &t.softirq,
                   &t.steal);
        यदि (r < 5)
                प्रत्यागम -EINVAL;
        यदि (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        प्रत्यागम 0;
}

पूर्णाङ्क convert_meminfo_मूल्य_to_uint64_bytes(ध्रुव वर्ण *s, uint64_t *ret) {
        _cleanup_free_ वर्ण *w = रिक्त;
        uint64_t v;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        r = extract_first_word(&s, &w, /* separators= */ रिक्त, /* flags= */ 0);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == 0)
                प्रत्यागम -EINVAL;

        /* Ensure the line ends in "kB" */
        यदि (!streq(s, "kB"))
                प्रत्यागम -EINVAL;

        r = safe_atou64(w, &v);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (v == UINT64_MAX)
                प्रत्यागम -EINVAL;

        यदि (!MUL_ASSIGN_SAFE(&v, U64_KB))
                प्रत्यागम -EOVERFLOW;

        *ret = v;
        प्रत्यागम 0;
}

पूर्णाङ्क procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used) {
        uint64_t mem_total = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ FILE *f = रिक्त;
        पूर्णाङ्क r;

        f = fopen("/proc/meminfo", "re");
        यदि (!f)
                प्रत्यागम -errno;

        प्रति (;;) {
                _cleanup_free_ वर्ण *पंक्ति = रिक्त;
                uint64_t *v;
                वर्ण *p;

                r = read_पंक्ति(f, LONG_पंक्ति_MAX, &पंक्ति);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        प्रत्यागम -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(पंक्ति, "MemTotal:");
                यदि (p)
                        v = &mem_total;
                अन्यथा {
                        p = first_word(पंक्ति, "MemAvailable:");
                        यदि (p)
                                v = &mem_available;
                        अन्यथा
                                अनुवर्त;
                }

                r = convert_meminfo_मूल्य_to_uint64_bytes(p, v);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (mem_total != UINT64_MAX && mem_available != UINT64_MAX)
                        भङ्ग;
        }

        यदि (mem_available > mem_total)
                प्रत्यागम -EINVAL;

        यदि (ret_total)
                *ret_total = mem_total;
        यदि (ret_used)
                *ret_used = mem_total - mem_available;
        प्रत्यागम 0;
}

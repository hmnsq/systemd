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
        _cleanup_free_ वर्ण *मानम् = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = read_one_पङ्क्तिः_file("/proc/sys/kernel/pid_max", &मानम्);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त safe_atou64(मानम्, ret);
}

पूर्णाङ्क procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ वर्ण *मानम् = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = read_one_पङ्क्तिः_file("/proc/sys/kernel/threads-max", &मानम्);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त safe_atou64(मानम्, ret);
}

पूर्णाङ्क procfs_tasks_set_limit(uint64_t limit) {
        वर्ण buffer[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        पूर्णाङ्क r;

        सचे (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                पटिनिवत्त -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        सचे (r < 0)
                पटिनिवत्त r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        सचे (limit > pid_max-1) {
                xsprintf(buffer, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        xsprintf(buffer, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
        सचे (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                सचे (procfs_get_threads_max(&threads_max) < 0)
                        पटिनिवत्त r; /* return original error */

                सचे (MIN(pid_max - 1, threads_max) != limit)
                        पटिनिवत्त r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ वर्ण *मानम् = रिक्त;
        ध्रुव वर्ण *p, *nr;
        size_t n;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = read_one_पङ्क्तिः_file("/proc/loadavg", &मानम्);
        सचे (r < 0)
                पटिनिवत्त r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(मानम्, '/');
        सचे (!p)
                पटिनिवत्त -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_safe(p, n);

        पटिनिवत्त safe_atou64(nr, ret);
}

स्थिर uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        याव (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        पटिनिवत्त a;
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

        पटिनिवत्त ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

पूर्णाङ्क procfs_cpu_get_usage(nsec_t *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        सचे (r < 0)
                पटिनिवत्त r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        पटिनिवत्त 0;
}

पूर्णाङ्क procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ वर्ण *first_पङ्क्तिः = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = read_one_पङ्क्तिः_file("/proc/stat", &first_पङ्क्तिः);
        सचे (r < 0)
                पटिनिवत्त r;

        ध्रुव वर्ण *p = first_word(first_पङ्क्तिः, "cpu");
        सचे (!p)
                पटिनिवत्त -EINVAL;

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
        सचे (r < 5)
                पटिनिवत्त -EINVAL;
        सचे (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        पटिनिवत्त 0;
}

पूर्णाङ्क convert_meminfo_मानम्_to_uint64_bytes(ध्रुव वर्ण *s, uint64_t *ret) {
        _cleanup_free_ वर्ण *w = रिक्त;
        uint64_t v;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        r = extract_first_word(&s, &w, /* separators= */ रिक्त, /* flags= */ 0);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (r == 0)
                पटिनिवत्त -EINVAL;

        /* Ensure the line ends in "kB" */
        सचे (!streq(s, "kB"))
                पटिनिवत्त -EINVAL;

        r = safe_atou64(w, &v);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (v == UINT64_MAX)
                पटिनिवत्त -EINVAL;

        सचे (!MUL_ASSIGN_SAFE(&v, U64_KB))
                पटिनिवत्त -EOVERFLOW;

        *ret = v;
        पटिनिवत्त 0;
}

पूर्णाङ्क procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used) {
        uint64_t mem_total = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ FILE *f = रिक्त;
        पूर्णाङ्क r;

        f = fopen("/proc/meminfo", "re");
        सचे (!f)
                पटिनिवत्त -errno;

        अत्थाय (;;) {
                _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;
                uint64_t *v;
                वर्ण *p;

                r = read_पङ्क्तिः(f, LONG_पङ्क्तिः_MAX, &पङ्क्तिः);
                सचे (r < 0)
                        पटिनिवत्त r;
                सचे (r == 0)
                        पटिनिवत्त -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(पङ्क्तिः, "MemTotal:");
                सचे (p)
                        v = &mem_total;
                अञ्ञथा {
                        p = first_word(पङ्क्तिः, "MemAvailable:");
                        सचे (p)
                                v = &mem_available;
                        अञ्ञथा
                                अनुवर्त;
                }

                r = convert_meminfo_मानम्_to_uint64_bytes(p, v);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (mem_total != UINT64_MAX && mem_available != UINT64_MAX)
                        भङ्ग;
        }

        सचे (mem_available > mem_total)
                पटिनिवत्त -EINVAL;

        सचे (ret_total)
                *ret_total = mem_total;
        सचे (ret_used)
                *ret_used = mem_total - mem_available;
        पटिनिवत्त 0;
}

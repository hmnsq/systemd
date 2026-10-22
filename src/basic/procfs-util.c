/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "procfs-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"

ಪೂರ್ಣಾಙ್ಕ procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ ವರ್ಣ *ಮೌಲ್ಯ = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = read_one_ಸಾಲು_file("/proc/sys/kernel/pid_max", &ಮೌಲ್ಯ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ safe_atou64(ಮೌಲ್ಯ, ret);
}

ಪೂರ್ಣಾಙ್ಕ procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ ವರ್ಣ *ಮೌಲ್ಯ = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = read_one_ಸಾಲು_file("/proc/sys/kernel/threads-max", &ಮೌಲ್ಯ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ safe_atou64(ಮೌಲ್ಯ, ret);
}

ಪೂರ್ಣಾಙ್ಕ procfs_tasks_set_limit(uint64_t limit) {
        ವರ್ಣ buffer[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        ಯದಿ (limit > pid_max-1) {
                xsprintf(buffer, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        xsprintf(buffer, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
        ಯದಿ (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                ಯದಿ (procfs_get_threads_max(&threads_max) < 0)
                        ಪ್ರತ್ಯಾಗಮ r; /* return original error */

                ಯದಿ (MIN(pid_max - 1, threads_max) != limit)
                        ಪ್ರತ್ಯಾಗಮ r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ ವರ್ಣ *ಮೌಲ್ಯ = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *p, *nr;
        size_t n;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = read_one_ಸಾಲು_file("/proc/loadavg", &ಮೌಲ್ಯ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(ಮೌಲ್ಯ, '/');
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_safe(p, n);

        ಪ್ರತ್ಯಾಗಮ safe_atou64(nr, ret);
}

ಸ್ಥಿರ uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        ಯಾವತ್ (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        ಪ್ರತ್ಯಾಗಮ a;
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

        ಪ್ರತ್ಯಾಗಮ ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

ಪೂರ್ಣಾಙ್ಕ procfs_cpu_get_usage(nsec_t *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ ವರ್ಣ *first_ಸಾಲು = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = read_one_ಸಾಲು_file("/proc/stat", &first_ಸಾಲು);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಧ್ರುವ ವರ್ಣ *p = first_word(first_ಸಾಲು, "cpu");
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

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
        ಯದಿ (r < 5)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ convert_meminfo_ಮೌಲ್ಯ_to_uint64_bytes(ಧ್ರುವ ವರ್ಣ *s, uint64_t *ret) {
        _cleanup_free_ ವರ್ಣ *w = ರಿಕ್ತ;
        uint64_t v;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = extract_first_word(&s, &w, /* separators= */ ರಿಕ್ತ, /* flags= */ 0);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* Ensure the line ends in "kB" */
        ಯದಿ (!streq(s, "kB"))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = safe_atou64(w, &v);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (v == UINT64_MAX)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (!MUL_ASSIGN_SAFE(&v, U64_KB))
                ಪ್ರತ್ಯಾಗಮ -EOVERFLOW;

        *ret = v;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used) {
        uint64_t mem_total = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        f = fopen("/proc/meminfo", "re");
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
                uint64_t *v;
                ವರ್ಣ *p;

                r = read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(ಸಾಲು, "MemTotal:");
                ಯದಿ (p)
                        v = &mem_total;
                ಅನ್ಯಥಾ {
                        p = first_word(ಸಾಲು, "MemAvailable:");
                        ಯದಿ (p)
                                v = &mem_available;
                        ಅನ್ಯಥಾ
                                ಅನುವರ್ತ;
                }

                r = convert_meminfo_ಮೌಲ್ಯ_to_uint64_bytes(p, v);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (mem_total != UINT64_MAX && mem_available != UINT64_MAX)
                        ಭಙ್ಗ;
        }

        ಯದಿ (mem_available > mem_total)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (ret_total)
                *ret_total = mem_total;
        ಯದಿ (ret_used)
                *ret_used = mem_total - mem_available;
        ಪ್ರತ್ಯಾಗಮ 0;
}

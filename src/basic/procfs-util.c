/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "procfs-util.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "time-util.h"

পূর্ণসংখ্যা procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ অক্ষর *মান = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = read_one_সারি_file("/proc/sys/kernel/pid_max", &মান);
        যদি (r < 0)
                ফেরত r;

        ফেরত safe_atou64(মান, ret);
}

পূর্ণসংখ্যা procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ অক্ষর *মান = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = read_one_সারি_file("/proc/sys/kernel/threads-max", &মান);
        যদি (r < 0)
                ফেরত r;

        ফেরত safe_atou64(মান, ret);
}

পূর্ণসংখ্যা procfs_tasks_set_limit(uint64_t limit) {
        অক্ষর বাফার[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        পূর্ণসংখ্যা r;

        যদি (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                ফেরত -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        যদি (r < 0)
                ফেরত r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        যদি (limit > pid_max-1) {
                xsprintf(বাফার, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", বাফার, WRITE_STRING_FILE_DISABLE_বাফার);
                যদি (r < 0)
                        ফেরত r;
        }

        xsprintf(বাফার, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", বাফার, WRITE_STRING_FILE_DISABLE_বাফার);
        যদি (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                যদি (procfs_get_threads_max(&threads_max) < 0)
                        ফেরত r; /* return original error */

                যদি (MIN(pid_max - 1, threads_max) != limit)
                        ফেরত r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        ফেরত 0;
}

পূর্ণসংখ্যা procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ অক্ষর *মান = ফাঁকা;
        ধ্রুবক অক্ষর *p, *nr;
        size_t n;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = read_one_সারি_file("/proc/loadavg", &মান);
        যদি (r < 0)
                ফেরত r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(মান, '/');
        যদি (!p)
                ফেরত -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_safe(p, n);

        ফেরত safe_atou64(nr, ret);
}

স্থির uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        যখন (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        ফেরত a;
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

        ফেরত ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

পূর্ণসংখ্যা procfs_cpu_get_usage(nsec_t *ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        যদি (r < 0)
                ফেরত r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        ফেরত 0;
}

পূর্ণসংখ্যা procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ অক্ষর *first_সারি = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = read_one_সারি_file("/proc/stat", &first_সারি);
        যদি (r < 0)
                ফেরত r;

        ধ্রুবক অক্ষর *p = first_word(first_সারি, "cpu");
        যদি (!p)
                ফেরত -EINVAL;

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
        যদি (r < 5)
                ফেরত -EINVAL;
        যদি (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        ফেরত 0;
}

পূর্ণসংখ্যা convert_meminfo_মান_to_uint64_bytes(ধ্রুবক অক্ষর *s, uint64_t *ret) {
        _cleanup_free_ অক্ষর *w = ফাঁকা;
        uint64_t v;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret);

        r = extract_first_word(&s, &w, /* separators= */ ফাঁকা, /* flags= */ 0);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0)
                ফেরত -EINVAL;

        /* Ensure the line ends in "kB" */
        যদি (!streq(s, "kB"))
                ফেরত -EINVAL;

        r = safe_atou64(w, &v);
        যদি (r < 0)
                ফেরত r;
        যদি (v == UINT64_MAX)
                ফেরত -EINVAL;

        যদি (!MUL_ASSIGN_SAFE(&v, U64_KB))
                ফেরত -EOVERFLOW;

        *ret = v;
        ফেরত 0;
}

পূর্ণসংখ্যা procfs_memory_get(uint64_t *ret_যোগ, uint64_t *ret_used) {
        uint64_t mem_যোগ = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ FILE *f = ফাঁকা;
        পূর্ণসংখ্যা r;

        f = fopen("/proc/meminfo", "re");
        যদি (!f)
                ফেরত -errno;

        জন্য (;;) {
                _cleanup_free_ অক্ষর *সারি = ফাঁকা;
                uint64_t *v;
                অক্ষর *p;

                r = read_সারি(f, LONG_সারি_MAX, &সারি);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        ফেরত -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(সারি, "MemTotal:");
                যদি (p)
                        v = &mem_যোগ;
                অন্যথা {
                        p = first_word(সারি, "MemAvailable:");
                        যদি (p)
                                v = &mem_available;
                        অন্যথা
                                চালু;
                }

                r = convert_meminfo_মান_to_uint64_bytes(p, v);
                যদি (r < 0)
                        ফেরত r;

                যদি (mem_যোগ != UINT64_MAX && mem_available != UINT64_MAX)
                        বিরতি;
        }

        যদি (mem_available > mem_যোগ)
                ফেরত -EINVAL;

        যদি (ret_যোগ)
                *ret_যোগ = mem_যোগ;
        যদি (ret_used)
                *ret_used = mem_যোগ - mem_available;
        ফেরত 0;
}

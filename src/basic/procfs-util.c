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

পূর্ণাঙ্ক procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ বর্ণ *মান = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = read_one_সারি_file("/proc/sys/kernel/pid_max", &মান);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম safe_atou64(মান, ret);
}

পূর্ণাঙ্ক procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ বর্ণ *মান = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = read_one_সারি_file("/proc/sys/kernel/threads-max", &মান);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম safe_atou64(মান, ret);
}

পূর্ণাঙ্ক procfs_tasks_set_limit(uint64_t limit) {
        বর্ণ buffer[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        পূর্ণাঙ্ক r;

        যদি (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                প্রত্যাগম -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        যদি (r < 0)
                প্রত্যাগম r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        যদি (limit > pid_max-1) {
                xsprintf(buffer, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        xsprintf(buffer, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
        যদি (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                যদি (procfs_get_threads_max(&threads_max) < 0)
                        প্রত্যাগম r; /* return original error */

                যদি (MIN(pid_max - 1, threads_max) != limit)
                        প্রত্যাগম r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ বর্ণ *মান = রিক্ত;
        ধ্রুব বর্ণ *p, *nr;
        size_t n;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = read_one_সারি_file("/proc/loadavg", &মান);
        যদি (r < 0)
                প্রত্যাগম r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(মান, '/');
        যদি (!p)
                প্রত্যাগম -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_safe(p, n);

        প্রত্যাগম safe_atou64(nr, ret);
}

স্থির uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        যাবত্ (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        প্রত্যাগম a;
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

        প্রত্যাগম ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

পূর্ণাঙ্ক procfs_cpu_get_usage(nsec_t *ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        যদি (r < 0)
                প্রত্যাগম r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ বর্ণ *first_সারি = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = read_one_সারি_file("/proc/stat", &first_সারি);
        যদি (r < 0)
                প্রত্যাগম r;

        ধ্রুব বর্ণ *p = first_word(first_সারি, "cpu");
        যদি (!p)
                প্রত্যাগম -EINVAL;

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
                প্রত্যাগম -EINVAL;
        যদি (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক convert_meminfo_মান_to_uint64_bytes(ধ্রুব বর্ণ *s, uint64_t *ret) {
        _cleanup_free_ বর্ণ *w = রিক্ত;
        uint64_t v;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret);

        r = extract_first_word(&s, &w, /* separators= */ রিক্ত, /* flags= */ 0);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (r == 0)
                প্রত্যাগম -EINVAL;

        /* Ensure the line ends in "kB" */
        যদি (!streq(s, "kB"))
                প্রত্যাগম -EINVAL;

        r = safe_atou64(w, &v);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (v == UINT64_MAX)
                প্রত্যাগম -EINVAL;

        যদি (!MUL_ASSIGN_SAFE(&v, U64_KB))
                প্রত্যাগম -EOVERFLOW;

        *ret = v;
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used) {
        uint64_t mem_total = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ FILE *f = রিক্ত;
        পূর্ণাঙ্ক r;

        f = fopen("/proc/meminfo", "re");
        যদি (!f)
                প্রত্যাগম -errno;

        প্রতি (;;) {
                _cleanup_free_ বর্ণ *সারি = রিক্ত;
                uint64_t *v;
                বর্ণ *p;

                r = read_সারি(f, LONG_সারি_MAX, &সারি);
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0)
                        প্রত্যাগম -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(সারি, "MemTotal:");
                যদি (p)
                        v = &mem_total;
                অন্যথা {
                        p = first_word(সারি, "MemAvailable:");
                        যদি (p)
                                v = &mem_available;
                        অন্যথা
                                অনুবর্ত;
                }

                r = convert_meminfo_মান_to_uint64_bytes(p, v);
                যদি (r < 0)
                        প্রত্যাগম r;

                যদি (mem_total != UINT64_MAX && mem_available != UINT64_MAX)
                        ভঙ্গ;
        }

        যদি (mem_available > mem_total)
                প্রত্যাগম -EINVAL;

        যদি (ret_total)
                *ret_total = mem_total;
        যদি (ret_used)
                *ret_used = mem_total - mem_available;
        প্রত্যাগম 0;
}

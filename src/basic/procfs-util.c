/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "procfs-util.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ ଅକ୍ଷର *ମୂଲ୍ୟ = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = read_one_ଧାଡ଼ି_file("/proc/sys/kernel/pid_max", &ମୂଲ୍ୟ);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ safe_atou64(ମୂଲ୍ୟ, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ ଅକ୍ଷର *ମୂଲ୍ୟ = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = read_one_ଧାଡ଼ି_file("/proc/sys/kernel/threads-max", &ମୂଲ୍ୟ);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ safe_atou64(ମୂଲ୍ୟ, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_tasks_set_limit(uint64_t limit) {
        ଅକ୍ଷର buffer[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                ଫେରନ୍ତୁ -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        ଯଦି (limit > pid_max-1) {
                xsprintf(buffer, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        xsprintf(buffer, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
        ଯଦି (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                ଯଦି (procfs_get_threads_max(&threads_max) < 0)
                        ଫେରନ୍ତୁ r; /* return original error */

                ଯଦି (MIN(pid_max - 1, threads_max) != limit)
                        ଫେରନ୍ତୁ r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ ଅକ୍ଷର *ମୂଲ୍ୟ = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *p, *nr;
        size_t n;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = read_one_ଧାଡ଼ି_file("/proc/loadavg", &ମୂଲ୍ୟ);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(ମୂଲ୍ୟ, '/');
        ଯଦି (!p)
                ଫେରନ୍ତୁ -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_safe(p, n);

        ଫେରନ୍ତୁ safe_atou64(nr, ret);
}

ସ୍ଥିର uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        ଯେତେବେଳେ (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        ଫେରନ୍ତୁ a;
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

        ଫେରନ୍ତୁ ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_cpu_get_usage(nsec_t *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ ଅକ୍ଷର *first_ଧାଡ଼ି = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = read_one_ଧାଡ଼ି_file("/proc/stat", &first_ଧାଡ଼ି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଅଚଳ ଅକ୍ଷର *p = first_word(first_ଧାଡ଼ି, "cpu");
        ଯଦି (!p)
                ଫେରନ୍ତୁ -EINVAL;

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
        ଯଦି (r < 5)
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା convert_meminfo_ମୂଲ୍ୟ_to_uint64_bytes(ଅଚଳ ଅକ୍ଷର *s, uint64_t *ret) {
        _cleanup_free_ ଅକ୍ଷର *w = ଖାଲି;
        uint64_t v;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(ret);

        r = extract_first_word(&s, &w, /* separators= */ ଖାଲି, /* flags= */ 0);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -EINVAL;

        /* Ensure the line ends in "kB" */
        ଯଦି (!streq(s, "kB"))
                ଫେରନ୍ତୁ -EINVAL;

        r = safe_atou64(w, &v);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (v == UINT64_MAX)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (!MUL_ASSIGN_SAFE(&v, U64_KB))
                ଫେରନ୍ତୁ -EOVERFLOW;

        *ret = v;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used) {
        uint64_t mem_total = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ FILE *f = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        f = fopen("/proc/meminfo", "re");
        ଯଦି (!f)
                ଫେରନ୍ତୁ -errno;

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
                uint64_t *v;
                ଅକ୍ଷର *p;

                r = read_ଧାଡ଼ି(f, LONG_ଧାଡ଼ି_MAX, &ଧାଡ଼ି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଫେରନ୍ତୁ -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(ଧାଡ଼ି, "MemTotal:");
                ଯଦି (p)
                        v = &mem_total;
                ଅନ୍ୟଥା {
                        p = first_word(ଧାଡ଼ି, "MemAvailable:");
                        ଯଦି (p)
                                v = &mem_available;
                        ଅନ୍ୟଥା
                                ଜାରି;
                }

                r = convert_meminfo_ମୂଲ୍ୟ_to_uint64_bytes(p, v);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (mem_total != UINT64_MAX && mem_available != UINT64_MAX)
                        ଭାଙ୍ଗ;
        }

        ଯଦି (mem_available > mem_total)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (ret_total)
                *ret_total = mem_total;
        ଯଦି (ret_used)
                *ret_used = mem_total - mem_available;
        ଫେରନ୍ତୁ 0;
}

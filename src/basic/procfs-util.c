/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "extract-word.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "log.h"
#ضمن "parse-util.h"
#ضمن "process-util.h"
#ضمن "procfs-util.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"
#ضمن "time-util.h"

صحيح procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ حرف *value = عدم;
        صحيح r;

        أكد(ret);

        r = read_one_line_file("/proc/sys/kernel/pid_max", &value);
        𐡄𐡍 (r < 0)
                أرجع r;

        أرجع safe_atou64(value, ret);
}

صحيح procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ حرف *value = عدم;
        صحيح r;

        أكد(ret);

        r = read_one_line_file("/proc/sys/kernel/threads-max", &value);
        𐡄𐡍 (r < 0)
                أرجع r;

        أرجع safe_atou64(value, ret);
}

صحيح procfs_tasks_set_limit(uint64_t limit) {
        حرف buffer[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        صحيح r;

        𐡄𐡍 (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                أرجع -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        𐡄𐡍 (r < 0)
                أرجع r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        𐡄𐡍 (limit > pid_max-1) {
                xsprintf(buffer, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
                𐡄𐡍 (r < 0)
                        أرجع r;
        }

        xsprintf(buffer, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
        𐡄𐡍 (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                𐡄𐡍 (procfs_get_threads_max(&threads_max) < 0)
                        أرجع r; /* return original error */

                𐡄𐡍 (MIN(pid_max - 1, threads_max) != limit)
                        أرجع r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        أرجع 0;
}

صحيح procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ حرف *value = عدم;
        ثابت حرف *p, *nr;
        size_t n;
        صحيح r;

        أكد(ret);

        r = read_one_line_file("/proc/loadavg", &value);
        𐡄𐡍 (r < 0)
                أرجع r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(value, '/');
        𐡄𐡍 (!p)
                أرجع -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_safe(p, n);

        أرجع safe_atou64(nr, ret);
}

ساكن uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        طالما (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        أرجع a;
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

        أرجع ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

صحيح procfs_cpu_get_usage(nsec_t *ret) {
        صحيح r;

        أكد(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        𐡄𐡍 (r < 0)
                أرجع r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        أرجع 0;
}

صحيح procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ حرف *first_line = عدم;
        صحيح r;

        أكد(ret);

        r = read_one_line_file("/proc/stat", &first_line);
        𐡄𐡍 (r < 0)
                أرجع r;

        ثابت حرف *p = first_word(first_line, "cpu");
        𐡄𐡍 (!p)
                أرجع -EINVAL;

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
        𐡄𐡍 (r < 5)
                أرجع -EINVAL;
        𐡄𐡍 (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        أرجع 0;
}

صحيح convert_meminfo_value_to_uint64_bytes(ثابت حرف *s, uint64_t *ret) {
        _cleanup_free_ حرف *w = عدم;
        uint64_t v;
        صحيح r;

        أكد(s);
        أكد(ret);

        r = extract_first_word(&s, &w, /* separators= */ عدم, /* flags= */ 0);
        𐡄𐡍 (r < 0)
                أرجع r;
        𐡄𐡍 (r == 0)
                أرجع -EINVAL;

        /* Ensure the line ends in "kB" */
        𐡄𐡍 (!streq(s, "kB"))
                أرجع -EINVAL;

        r = safe_atou64(w, &v);
        𐡄𐡍 (r < 0)
                أرجع r;
        𐡄𐡍 (v == UINT64_MAX)
                أرجع -EINVAL;

        𐡄𐡍 (!MUL_ASSIGN_SAFE(&v, U64_KB))
                أرجع -EOVERFLOW;

        *ret = v;
        أرجع 0;
}

صحيح procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used) {
        uint64_t mem_total = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        f = fopen("/proc/meminfo", "re");
        𐡄𐡍 (!f)
                أرجع -errno;

        لكل (;;) {
                _cleanup_free_ حرف *line = عدم;
                uint64_t *v;
                حرف *p;

                r = read_line(f, LONG_LINE_MAX, &line);
                𐡄𐡍 (r < 0)
                        أرجع r;
                𐡄𐡍 (r == 0)
                        أرجع -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(line, "MemTotal:");
                𐡄𐡍 (p)
                        v = &mem_total;
                وإلا {
                        p = first_word(line, "MemAvailable:");
                        𐡄𐡍 (p)
                                v = &mem_available;
                        وإلا
                                استمر;
                }

                r = convert_meminfo_value_to_uint64_bytes(p, v);
                𐡄𐡍 (r < 0)
                        أرجع r;

                𐡄𐡍 (mem_total != UINT64_MAX && mem_available != UINT64_MAX)
                        اكسر;
        }

        𐡄𐡍 (mem_available > mem_total)
                أرجع -EINVAL;

        𐡄𐡍 (ret_total)
                *ret_total = mem_total;
        𐡄𐡍 (ret_used)
                *ret_used = mem_total - mem_available;
        أرجع 0;
}

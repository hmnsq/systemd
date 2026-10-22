/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "extract-word.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "log.h"
#شامل "parse-util.h"
#شامل "process-util.h"
#شامل "procfs-util.h"
#شامل "stdio-util.h"
#شامل "string-util.h"
#شامل "time-util.h"

صحیح procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ نویسه *مقدار = پوچ;
        صحیح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/sys/kernel/pid_max", &مقدار);
        اگر (r < 0)
                بازگشت r;

        بازگشت safe_atou64(مقدار, ret);
}

صحیح procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ نویسه *مقدار = پوچ;
        صحیح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/sys/kernel/threads-max", &مقدار);
        اگر (r < 0)
                بازگشت r;

        بازگشت safe_atou64(مقدار, ret);
}

صحیح procfs_tasks_set_limit(uint64_t limit) {
        نویسه buffer[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        صحیح r;

        اگر (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                بازگشت -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        اگر (r < 0)
                بازگشت r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        اگر (limit > pid_max-1) {
                xsprintf(buffer, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
                اگر (r < 0)
                        بازگشت r;
        }

        xsprintf(buffer, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
        اگر (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                اگر (procfs_get_threads_max(&threads_max) < 0)
                        بازگشت r; /* return original error */

                اگر (MIN(pid_max - 1, threads_max) != limit)
                        بازگشت r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        بازگشت 0;
}

صحیح procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ نویسه *مقدار = پوچ;
        ثابت نویسه *p, *nr;
        size_t n;
        صحیح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/loadavg", &مقدار);
        اگر (r < 0)
                بازگشت r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(مقدار, '/');
        اگر (!p)
                بازگشت -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_safe(p, n);

        بازگشت safe_atou64(nr, ret);
}

ایستا uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        وقتی (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        بازگشت a;
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

        بازگشت ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

صحیح procfs_cpu_get_usage(nsec_t *ret) {
        صحیح r;

        أكد(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        اگر (r < 0)
                بازگشت r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        بازگشت 0;
}

صحیح procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ نویسه *first_سطر = پوچ;
        صحیح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/stat", &first_سطر);
        اگر (r < 0)
                بازگشت r;

        ثابت نویسه *p = first_word(first_سطر, "cpu");
        اگر (!p)
                بازگشت -EINVAL;

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
        اگر (r < 5)
                بازگشت -EINVAL;
        اگر (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        بازگشت 0;
}

صحیح convert_meminfo_مقدار_to_uint64_bytes(ثابت نویسه *s, uint64_t *ret) {
        _cleanup_free_ نویسه *w = پوچ;
        uint64_t v;
        صحیح r;

        أكد(s);
        أكد(ret);

        r = extract_first_word(&s, &w, /* separators= */ پوچ, /* flags= */ 0);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -EINVAL;

        /* Ensure the line ends in "kB" */
        اگر (!streq(s, "kB"))
                بازگشت -EINVAL;

        r = safe_atou64(w, &v);
        اگر (r < 0)
                بازگشت r;
        اگر (v == UINT64_MAX)
                بازگشت -EINVAL;

        اگر (!MUL_ASSIGN_SAFE(&v, U64_KB))
                بازگشت -EOVERFLOW;

        *ret = v;
        بازگشت 0;
}

صحیح procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used) {
        uint64_t mem_total = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ FILE *f = پوچ;
        صحیح r;

        f = fopen("/proc/meminfo", "re");
        اگر (!f)
                بازگشت -errno;

        برای (;;) {
                _cleanup_free_ نویسه *سطر = پوچ;
                uint64_t *v;
                نویسه *p;

                r = read_سطر(f, LONG_سطر_MAX, &سطر);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بازگشت -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(سطر, "MemTotal:");
                اگر (p)
                        v = &mem_total;
                وگرنه {
                        p = first_word(سطر, "MemAvailable:");
                        اگر (p)
                                v = &mem_available;
                        وگرنه
                                ادامه;
                }

                r = convert_meminfo_مقدار_to_uint64_bytes(p, v);
                اگر (r < 0)
                        بازگشت r;

                اگر (mem_total != UINT64_MAX && mem_available != UINT64_MAX)
                        بشکن;
        }

        اگر (mem_available > mem_total)
                بازگشت -EINVAL;

        اگر (ret_total)
                *ret_total = mem_total;
        اگر (ret_used)
                *ret_used = mem_total - mem_available;
        بازگشت 0;
}

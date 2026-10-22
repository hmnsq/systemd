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
        _cleanup_free_ حرف *قدر = تہی;
        صحیح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/sys/kernel/pid_max", &قدر);
        اگر (r < 0)
                واپس r;

        واپس محفوظ_atou64(قدر, ret);
}

صحیح procfs_get_دھاگے_max(uint64_t *ret) {
        _cleanup_free_ حرف *قدر = تہی;
        صحیح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/sys/kernel/threads-max", &قدر);
        اگر (r < 0)
                واپس r;

        واپس محفوظ_atou64(قدر, ret);
}

صحیح procfs_tasks_سیٹ_حد_بندی(uint64_t limit) {
        حرف بفر[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        صحیح r;

        اگر (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                واپس -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        اگر (r < 0)
                واپس r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        اگر (limit > pid_max-1) {
                xsprintf(بفر, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", بفر, WRITE_STRING_FILE_DISABLE_بفر);
                اگر (r < 0)
                        واپس r;
        }

        xsprintf(بفر, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", بفر, WRITE_STRING_FILE_DISABLE_بفر);
        اگر (r < 0) {
                uint64_t دھاگے_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                اگر (procfs_get_دھاگے_max(&دھاگے_max) < 0)
                        واپس r; /* return original error */

                اگر (MIN(pid_max - 1, دھاگے_max) != limit)
                        واپس r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        واپس 0;
}

صحیح procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ حرف *قدر = تہی;
        ثابت حرف *p, *nr;
        size_t n;
        صحیح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/loadavg", &قدر);
        اگر (r < 0)
                واپس r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(قدر, '/');
        اگر (!p)
                واپس -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_محفوظ(p, n);

        واپس محفوظ_atou64(nr, ret);
}

ساکن uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        جب (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        واپس a;
}

nsec_t procfs_ticks_مقصد_nsec(uint64_t ticks) {
        uint64_t ticks_فی_دوسرا = sysconf_clock_ticks_cached(),
                عاد_اعظم = calc_gcd64(NSEC_فی_SEC, ticks_فی_دوسرا);

        /* Reduce NSEC_PER_SEC/ticks_per_second first to avoid overflow of ticks * NSEC_PER_SEC, and split
         * the multiplication into whole and remainder parts, so that the result stays exact.
         *
         * Note that in most cases b == 1 and the second term is trivial. */
        uint64_t a = NSEC_فی_SEC / عاد_اعظم,
                 b = ticks_فی_دوسرا / عاد_اعظم;

        واپس ticks / b * a + DIV_باری_اوپر(ticks % b * a, b);
}

صحیح procfs_cpu_get_usage(nsec_t *ret) {
        صحیح r;

        أكد(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        اگر (r < 0)
                واپس r;

        *ret = procfs_ticks_مقصد_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        واپس 0;
}

صحیح procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ حرف *پہلا_سطر = تہی;
        صحیح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/stat", &پہلا_سطر);
        اگر (r < 0)
                واپس r;

        ثابت حرف *p = پہلا_لفظ(پہلا_سطر, "cpu");
        اگر (!p)
                واپس -EINVAL;

        /* All eight fields exist on any kernel we support: iowait was added in 2.5.41, steal in 2.6.11.
         * "guest" and "guest_nice" are not returned, the kernel already includes them in "user" and "nice".
         * Nevertheless, this file might be synthesized slopilly, so let's not require the last three fields.
         * We don't care about them in most cases anyway. */
        ProcfsCpuTicks t = {};
        r = لڑی_سے_پڑھو(p, "%"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64,
                   &t.user,
                   &t.nice,
                   &t.system,
                   &t.idle,
                   &t.iowait,
                   &t.irq,
                   &t.softirq,
                   &t.steal);
        اگر (r < 5)
                واپس -EINVAL;
        اگر (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        واپس 0;
}

صحیح convert_meminfo_قدر_مقصد_uint64_بائٹیں(ثابت حرف *s, uint64_t *ret) {
        _cleanup_free_ حرف *w = تہی;
        uint64_t v;
        صحیح r;

        أكد(s);
        أكد(ret);

        r = نکالو_پہلا_لفظ(&s, &w, /* separators= */ تہی, /* flags= */ 0);
        اگر (r < 0)
                واپس r;
        اگر (r == 0)
                واپس -EINVAL;

        /* Ensure the line ends in "kB" */
        اگر (!streq(s, "kB"))
                واپس -EINVAL;

        r = محفوظ_atou64(w, &v);
        اگر (r < 0)
                واپس r;
        اگر (v == UINT64_MAX)
                واپس -EINVAL;

        اگر (!MUL_تفویض_محفوظ(&v, U64_KB))
                واپس -EOVERFLOW;

        *ret = v;
        واپس 0;
}

صحیح procfs_حافظہ_get(uint64_t *ret_کل, uint64_t *ret_مستعمل) {
        uint64_t mem_کل = UINT64_MAX, mem_دستیاب = UINT64_MAX;
        _cleanup_fclose_ FILE *f = تہی;
        صحیح r;

        f = fopen("/proc/meminfo", "re");
        اگر (!f)
                واپس -errno;

        برائے (;;) {
                _cleanup_free_ حرف *سطر = تہی;
                uint64_t *v;
                حرف *p;

                r = read_سطر(f, LONG_سطر_MAX, &سطر);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        واپس -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = پہلا_لفظ(سطر, "MemTotal:");
                اگر (p)
                        v = &mem_کل;
                ورنہ {
                        p = پہلا_لفظ(سطر, "MemAvailable:");
                        اگر (p)
                                v = &mem_دستیاب;
                        ورنہ
                                جاری;
                }

                r = convert_meminfo_قدر_مقصد_uint64_بائٹیں(p, v);
                اگر (r < 0)
                        واپس r;

                اگر (mem_کل != UINT64_MAX && mem_دستیاب != UINT64_MAX)
                        توڑو;
        }

        اگر (mem_دستیاب > mem_کل)
                واپس -EINVAL;

        اگر (ret_کل)
                *ret_کل = mem_کل;
        اگر (ret_مستعمل)
                *ret_مستعمل = mem_کل - mem_دستیاب;
        واپس 0;
}

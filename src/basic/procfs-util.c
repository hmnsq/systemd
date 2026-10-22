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
        _cleanup_free_ حرف *قدر = عدم;
        صحيح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/sys/kernel/pid_max", &قدر);
        إذا (r < 0)
                أرجع r;

        أرجع محفوظ_atou64(قدر, ret);
}

صحيح procfs_get_دھاگے_max(uint64_t *ret) {
        _cleanup_free_ حرف *قدر = عدم;
        صحيح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/sys/kernel/threads-max", &قدر);
        إذا (r < 0)
                أرجع r;

        أرجع محفوظ_atou64(قدر, ret);
}

صحيح procfs_tasks_سیٹ_حد_بندی(uint64_t limit) {
        حرف بفر[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        صحيح r;

        إذا (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                أرجع -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        إذا (r < 0)
                أرجع r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        إذا (limit > pid_max-1) {
                xsprintf(بفر, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", بفر, WRITE_STRING_FILE_DISABLE_بفر);
                إذا (r < 0)
                        أرجع r;
        }

        xsprintf(بفر, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", بفر, WRITE_STRING_FILE_DISABLE_بفر);
        إذا (r < 0) {
                uint64_t دھاگے_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                إذا (procfs_get_دھاگے_max(&دھاگے_max) < 0)
                        أرجع r; /* return original error */

                إذا (MIN(pid_max - 1, دھاگے_max) != limit)
                        أرجع r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        أرجع 0;
}

صحيح procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ حرف *قدر = عدم;
        ثابت حرف *p, *nr;
        size_t n;
        صحيح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/loadavg", &قدر);
        إذا (r < 0)
                أرجع r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(قدر, '/');
        إذا (!p)
                أرجع -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_محفوظ(p, n);

        أرجع محفوظ_atou64(nr, ret);
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

nsec_t procfs_ticks_منزل_nsec(uint64_t ticks) {
        uint64_t ticks_فی_دوجا = sysconf_clock_ticks_cached(),
                عاد_اعظم = calc_gcd64(NSEC_فی_SEC, ticks_فی_دوجا);

        /* Reduce NSEC_PER_SEC/ticks_per_second first to avoid overflow of ticks * NSEC_PER_SEC, and split
         * the multiplication into whole and remainder parts, so that the result stays exact.
         *
         * Note that in most cases b == 1 and the second term is trivial. */
        uint64_t a = NSEC_فی_SEC / عاد_اعظم,
                 b = ticks_فی_دوجا / عاد_اعظم;

        أرجع ticks / b * a + DIV_گیڑا_اتے(ticks % b * a, b);
}

صحيح procfs_cpu_get_usage(nsec_t *ret) {
        صحيح r;

        أكد(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        إذا (r < 0)
                أرجع r;

        *ret = procfs_ticks_منزل_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        أرجع 0;
}

صحيح procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ حرف *پہلا_سطر = عدم;
        صحيح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/stat", &پہلا_سطر);
        إذا (r < 0)
                أرجع r;

        ثابت حرف *p = پہلا_لفظ(پہلا_سطر, "cpu");
        إذا (!p)
                أرجع -EINVAL;

        /* All eight fields exist on any kernel we support: iowait was added in 2.5.41, steal in 2.6.11.
         * "guest" and "guest_nice" are not returned, the kernel already includes them in "user" and "nice".
         * Nevertheless, this file might be synthesized slopilly, so let's not require the last three fields.
         * We don't care about them in most cases anyway. */
        ProcfsCpuTicks t = {};
        r = لڑی_توں_پڑھو(p, "%"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64,
                   &t.user,
                   &t.nice,
                   &t.system,
                   &t.idle,
                   &t.iowait,
                   &t.irq,
                   &t.softirq,
                   &t.steal);
        إذا (r < 5)
                أرجع -EINVAL;
        إذا (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        أرجع 0;
}

صحيح convert_meminfo_قدر_منزل_uint64_بائٹاں(ثابت حرف *s, uint64_t *ret) {
        _cleanup_free_ حرف *w = عدم;
        uint64_t v;
        صحيح r;

        أكد(s);
        أكد(ret);

        r = کڈھو_پہلا_لفظ(&s, &w, /* separators= */ عدم, /* flags= */ 0);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -EINVAL;

        /* Ensure the line ends in "kB" */
        إذا (!streq(s, "kB"))
                أرجع -EINVAL;

        r = محفوظ_atou64(w, &v);
        إذا (r < 0)
                أرجع r;
        إذا (v == UINT64_MAX)
                أرجع -EINVAL;

        إذا (!MUL_سونپو_محفوظ(&v, U64_KB))
                أرجع -EOVERFLOW;

        *ret = v;
        أرجع 0;
}

صحيح procfs_حافظہ_get(uint64_t *ret_کل, uint64_t *ret_ورتیا) {
        uint64_t mem_کل = UINT64_MAX, mem_دستیاب = UINT64_MAX;
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        f = fopen("/proc/meminfo", "re");
        إذا (!f)
                أرجع -errno;

        لكل (;;) {
                _cleanup_free_ حرف *سطر = عدم;
                uint64_t *v;
                حرف *p;

                r = read_سطر(f, LONG_سطر_MAX, &سطر);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        أرجع -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = پہلا_لفظ(سطر, "MemTotal:");
                إذا (p)
                        v = &mem_کل;
                وإلا {
                        p = پہلا_لفظ(سطر, "MemAvailable:");
                        إذا (p)
                                v = &mem_دستیاب;
                        وإلا
                                استمر;
                }

                r = convert_meminfo_قدر_منزل_uint64_بائٹاں(p, v);
                إذا (r < 0)
                        أرجع r;

                إذا (mem_کل != UINT64_MAX && mem_دستیاب != UINT64_MAX)
                        اكسر;
        }

        إذا (mem_دستیاب > mem_کل)
                أرجع -EINVAL;

        إذا (ret_کل)
                *ret_کل = mem_کل;
        إذا (ret_ورتیا)
                *ret_ورتیا = mem_کل - mem_دستیاب;
        أرجع 0;
}

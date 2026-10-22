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
        _cleanup_free_ حرف *مقدار = عدم;
        صحيح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/sys/kernel/pid_max", &مقدار);
        إذا (r < 0)
                أرجع r;

        أرجع امن_atou64(مقدار, ret);
}

صحيح procfs_get_نخها_max(uint64_t *ret) {
        _cleanup_free_ حرف *مقدار = عدم;
        صحيح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/sys/kernel/threads-max", &مقدار);
        إذا (r < 0)
                أرجع r;

        أرجع امن_atou64(مقدار, ret);
}

صحيح procfs_tasks_مجموعه_حد(uint64_t limit) {
        حرف میانگیر[DECIMAL_STR_MAX(uint64_t)+1];
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
                xsprintf(میانگیر, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", میانگیر, WRITE_STRING_FILE_DISABLE_میانگیر);
                إذا (r < 0)
                        أرجع r;
        }

        xsprintf(میانگیر, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", میانگیر, WRITE_STRING_FILE_DISABLE_میانگیر);
        إذا (r < 0) {
                uint64_t نخها_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                إذا (procfs_get_نخها_max(&نخها_max) < 0)
                        أرجع r; /* return original error */

                إذا (MIN(pid_max - 1, نخها_max) != limit)
                        أرجع r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        أرجع 0;
}

صحيح procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ حرف *مقدار = عدم;
        ثابت حرف *p, *nr;
        size_t n;
        صحيح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/loadavg", &مقدار);
        إذا (r < 0)
                أرجع r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(مقدار, '/');
        إذا (!p)
                أرجع -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_امن(p, n);

        أرجع امن_atou64(nr, ret);
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

nsec_t procfs_ticks_مقصد_nsec(uint64_t ticks) {
        uint64_t ticks_در_هر_دومی = sysconf_clock_ticks_cached(),
                بزرگترین_مقسوم_علیه = calc_gcd64(NSEC_در_هر_SEC, ticks_در_هر_دومی);

        /* Reduce NSEC_PER_SEC/ticks_per_second first to avoid overflow of ticks * NSEC_PER_SEC, and split
         * the multiplication into whole and remainder parts, so that the result stays exact.
         *
         * Note that in most cases b == 1 and the second term is trivial. */
        uint64_t a = NSEC_در_هر_SEC / بزرگترین_مقسوم_علیه,
                 b = ticks_در_هر_دومی / بزرگترین_مقسوم_علیه;

        أرجع ticks / b * a + DIV_دور_به_بالا(ticks % b * a, b);
}

صحيح procfs_cpu_get_usage(nsec_t *ret) {
        صحيح r;

        أكد(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        إذا (r < 0)
                أرجع r;

        *ret = procfs_ticks_مقصد_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        أرجع 0;
}

صحيح procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ حرف *اولی_سطر = عدم;
        صحيح r;

        أكد(ret);

        r = read_one_سطر_file("/proc/stat", &اولی_سطر);
        إذا (r < 0)
                أرجع r;

        ثابت حرف *p = اولی_کلمه(اولی_سطر, "cpu");
        إذا (!p)
                أرجع -EINVAL;

        /* All eight fields exist on any kernel we support: iowait was added in 2.5.41, steal in 2.6.11.
         * "guest" and "guest_nice" are not returned, the kernel already includes them in "user" and "nice".
         * Nevertheless, this file might be synthesized slopilly, so let's not require the last three fields.
         * We don't care about them in most cases anyway. */
        ProcfsCpuTicks t = {};
        r = از_رشته_بخوان(p, "%"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64,
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

صحيح convert_meminfo_مقدار_مقصد_uint64_بایتها(ثابت حرف *s, uint64_t *ret) {
        _cleanup_free_ حرف *w = عدم;
        uint64_t v;
        صحيح r;

        أكد(s);
        أكد(ret);

        r = استخراج_اولی_کلمه(&s, &w, /* separators= */ عدم, /* flags= */ 0);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -EINVAL;

        /* Ensure the line ends in "kB" */
        إذا (!streq(s, "kB"))
                أرجع -EINVAL;

        r = امن_atou64(w, &v);
        إذا (r < 0)
                أرجع r;
        إذا (v == UINT64_MAX)
                أرجع -EINVAL;

        إذا (!MUL_تخصیص_بده_امن(&v, U64_KB))
                أرجع -EOVERFLOW;

        *ret = v;
        أرجع 0;
}

صحيح procfs_حافظه_get(uint64_t *ret_مجموع, uint64_t *ret_استفاده_شده) {
        uint64_t mem_مجموع = UINT64_MAX, mem_موجود = UINT64_MAX;
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

                p = اولی_کلمه(سطر, "MemTotal:");
                إذا (p)
                        v = &mem_مجموع;
                وإلا {
                        p = اولی_کلمه(سطر, "MemAvailable:");
                        إذا (p)
                                v = &mem_موجود;
                        وإلا
                                استمر;
                }

                r = convert_meminfo_مقدار_مقصد_uint64_بایتها(p, v);
                إذا (r < 0)
                        أرجع r;

                إذا (mem_مجموع != UINT64_MAX && mem_موجود != UINT64_MAX)
                        اكسر;
        }

        إذا (mem_موجود > mem_مجموع)
                أرجع -EINVAL;

        إذا (ret_مجموع)
                *ret_مجموع = mem_مجموع;
        إذا (ret_استفاده_شده)
                *ret_استفاده_شده = mem_مجموع - mem_موجود;
        أرجع 0;
}

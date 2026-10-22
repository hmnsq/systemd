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
        _cleanup_free_ حرف *ارزښت = عدم;
        صحيح r;

        أكد(ret);

        r = read_one_کرښه_file("/proc/sys/kernel/pid_max", &ارزښت);
        إذا (r < 0)
                أرجع r;

        أرجع خوندي_atou64(ارزښت, ret);
}

صحيح procfs_get_تارونه_max(uint64_t *ret) {
        _cleanup_free_ حرف *ارزښت = عدم;
        صحيح r;

        أكد(ret);

        r = read_one_کرښه_file("/proc/sys/kernel/threads-max", &ارزښت);
        إذا (r < 0)
                أرجع r;

        أرجع خوندي_atou64(ارزښت, ret);
}

صحيح procfs_tasks_سېټ_حد(uint64_t limit) {
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
                uint64_t تارونه_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                إذا (procfs_get_تارونه_max(&تارونه_max) < 0)
                        أرجع r; /* return original error */

                إذا (MIN(pid_max - 1, تارونه_max) != limit)
                        أرجع r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        أرجع 0;
}

صحيح procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ حرف *ارزښت = عدم;
        ثابت حرف *p, *nr;
        size_t n;
        صحيح r;

        أكد(ret);

        r = read_one_کرښه_file("/proc/loadavg", &ارزښت);
        إذا (r < 0)
                أرجع r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(ارزښت, '/');
        إذا (!p)
                أرجع -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_خوندي(p, n);

        أرجع خوندي_atou64(nr, ret);
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
        uint64_t ticks_فی_دوهم = sysconf_clock_ticks_cached(),
                لوی_ګډ_وېشونکی = calc_gcd64(NSEC_فی_SEC, ticks_فی_دوهم);

        /* Reduce NSEC_PER_SEC/ticks_per_second first to avoid overflow of ticks * NSEC_PER_SEC, and split
         * the multiplication into whole and remainder parts, so that the result stays exact.
         *
         * Note that in most cases b == 1 and the second term is trivial. */
        uint64_t a = NSEC_فی_SEC / لوی_ګډ_وېشونکی,
                 b = ticks_فی_دوهم / لوی_ګډ_وېشونکی;

        أرجع ticks / b * a + DIV_پړاو_پورته(ticks % b * a, b);
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
        _cleanup_free_ حرف *لومړی_کرښه = عدم;
        صحيح r;

        أكد(ret);

        r = read_one_کرښه_file("/proc/stat", &لومړی_کرښه);
        إذا (r < 0)
                أرجع r;

        ثابت حرف *p = لومړی_کلمه(لومړی_کرښه, "cpu");
        إذا (!p)
                أرجع -EINVAL;

        /* All eight fields exist on any kernel we support: iowait was added in 2.5.41, steal in 2.6.11.
         * "guest" and "guest_nice" are not returned, the kernel already includes them in "user" and "nice".
         * Nevertheless, this file might be synthesized slopilly, so let's not require the last three fields.
         * We don't care about them in most cases anyway. */
        ProcfsCpuTicks t = {};
        r = له_مزي_ولوله(p, "%"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64,
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

صحيح convert_meminfo_ارزښت_منزل_uint64_بایټونه(ثابت حرف *s, uint64_t *ret) {
        _cleanup_free_ حرف *w = عدم;
        uint64_t v;
        صحيح r;

        أكد(s);
        أكد(ret);

        r = ایستل_لومړی_کلمه(&s, &w, /* separators= */ عدم, /* flags= */ 0);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -EINVAL;

        /* Ensure the line ends in "kB" */
        إذا (!streq(s, "kB"))
                أرجع -EINVAL;

        r = خوندي_atou64(w, &v);
        إذا (r < 0)
                أرجع r;
        إذا (v == UINT64_MAX)
                أرجع -EINVAL;

        إذا (!MUL_ټاکل_خوندي(&v, U64_KB))
                أرجع -EOVERFLOW;

        *ret = v;
        أرجع 0;
}

صحيح procfs_حافظه_get(uint64_t *ret_ټول, uint64_t *ret_کارول_شوی) {
        uint64_t mem_ټول = UINT64_MAX, mem_شته = UINT64_MAX;
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        f = fopen("/proc/meminfo", "re");
        إذا (!f)
                أرجع -errno;

        لكل (;;) {
                _cleanup_free_ حرف *کرښه = عدم;
                uint64_t *v;
                حرف *p;

                r = read_کرښه(f, LONG_کرښه_MAX, &کرښه);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        أرجع -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = لومړی_کلمه(کرښه, "MemTotal:");
                إذا (p)
                        v = &mem_ټول;
                وإلا {
                        p = لومړی_کلمه(کرښه, "MemAvailable:");
                        إذا (p)
                                v = &mem_شته;
                        وإلا
                                استمر;
                }

                r = convert_meminfo_ارزښت_منزل_uint64_بایټونه(p, v);
                إذا (r < 0)
                        أرجع r;

                إذا (mem_ټول != UINT64_MAX && mem_شته != UINT64_MAX)
                        اكسر;
        }

        إذا (mem_شته > mem_ټول)
                أرجع -EINVAL;

        إذا (ret_ټول)
                *ret_ټول = mem_ټول;
        إذا (ret_کارول_شوی)
                *ret_کارول_شوی = mem_ټول - mem_شته;
        أرجع 0;
}

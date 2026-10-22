/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "procfs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"

ਪੂਰਨ procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ ਅੱਖਰ *ਮੁੱਲ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = read_one_ਸਤਰ_file("/proc/sys/kernel/pid_max", &ਮੁੱਲ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou64(ਮੁੱਲ, ret);
}

ਪੂਰਨ procfs_get_ਧਾਗੇ_max(uint64_t *ret) {
        _cleanup_free_ ਅੱਖਰ *ਮੁੱਲ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = read_one_ਸਤਰ_file("/proc/sys/kernel/threads-max", &ਮੁੱਲ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou64(ਮੁੱਲ, ret);
}

ਪੂਰਨ procfs_tasks_ਸੈੱਟ_ਹੱਦਬੰਦੀ(uint64_t limit) {
        ਅੱਖਰ ਬਫਰ[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        ਪੂਰਨ r;

        ਜੇ (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                ਵਾਪਸ -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        ਜੇ (limit > pid_max-1) {
                xsprintf(ਬਫਰ, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", ਬਫਰ, WRITE_STRING_FILE_DISABLE_ਬਫਰ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        xsprintf(ਬਫਰ, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", ਬਫਰ, WRITE_STRING_FILE_DISABLE_ਬਫਰ);
        ਜੇ (r < 0) {
                uint64_t ਧਾਗੇ_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                ਜੇ (procfs_get_ਧਾਗੇ_max(&ਧਾਗੇ_max) < 0)
                        ਵਾਪਸ r; /* return original error */

                ਜੇ (MIN(pid_max - 1, ਧਾਗੇ_max) != limit)
                        ਵਾਪਸ r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ ਅੱਖਰ *ਮੁੱਲ = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *p, *nr;
        size_t n;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = read_one_ਸਤਰ_file("/proc/loadavg", &ਮੁੱਲ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(ਮੁੱਲ, '/');
        ਜੇ (!p)
                ਵਾਪਸ -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_ਸੁਰੱਖਿਅਤ(p, n);

        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou64(nr, ret);
}

ਸਥਿਰ uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        ਜਦੋਂ (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        ਵਾਪਸ a;
}

nsec_t procfs_ticks_ਮੰਜ਼ਿਲ_nsec(uint64_t ticks) {
        uint64_t ticks_ਪ੍ਰਤੀ_ਦੂਜਾ = sysconf_clock_ticks_cached(),
                ਮਹੱਤਮ_ਸਾਂਝਾ_ਭਾਜਕ = calc_gcd64(NSEC_ਪ੍ਰਤੀ_SEC, ticks_ਪ੍ਰਤੀ_ਦੂਜਾ);

        /* Reduce NSEC_PER_SEC/ticks_per_second first to avoid overflow of ticks * NSEC_PER_SEC, and split
         * the multiplication into whole and remainder parts, so that the result stays exact.
         *
         * Note that in most cases b == 1 and the second term is trivial. */
        uint64_t a = NSEC_ਪ੍ਰਤੀ_SEC / ਮਹੱਤਮ_ਸਾਂਝਾ_ਭਾਜਕ,
                 b = ticks_ਪ੍ਰਤੀ_ਦੂਜਾ / ਮਹੱਤਮ_ਸਾਂਝਾ_ਭਾਜਕ;

        ਵਾਪਸ ticks / b * a + DIV_ਗੇੜਾ_ਉੱਪਰ(ticks % b * a, b);
}

ਪੂਰਨ procfs_cpu_get_usage(nsec_t *ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = procfs_ticks_ਮੰਜ਼ਿਲ_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        ਵਾਪਸ 0;
}

ਪੂਰਨ procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ ਅੱਖਰ *ਪਹਿਲਾ_ਸਤਰ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = read_one_ਸਤਰ_file("/proc/stat", &ਪਹਿਲਾ_ਸਤਰ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਅਚਲ ਅੱਖਰ *p = ਪਹਿਲਾ_ਸ਼ਬਦ(ਪਹਿਲਾ_ਸਤਰ, "cpu");
        ਜੇ (!p)
                ਵਾਪਸ -EINVAL;

        /* All eight fields exist on any kernel we support: iowait was added in 2.5.41, steal in 2.6.11.
         * "guest" and "guest_nice" are not returned, the kernel already includes them in "user" and "nice".
         * Nevertheless, this file might be synthesized slopilly, so let's not require the last three fields.
         * We don't care about them in most cases anyway. */
        ProcfsCpuTicks t = {};
        r = ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(p, "%"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64,
                   &t.user,
                   &t.nice,
                   &t.system,
                   &t.idle,
                   &t.iowait,
                   &t.irq,
                   &t.softirq,
                   &t.steal);
        ਜੇ (r < 5)
                ਵਾਪਸ -EINVAL;
        ਜੇ (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        ਵਾਪਸ 0;
}

ਪੂਰਨ convert_meminfo_ਮੁੱਲ_ਮੰਜ਼ਿਲ_uint64_ਬਾਈਟਾਂ(ਅਚਲ ਅੱਖਰ *s, uint64_t *ret) {
        _cleanup_free_ ਅੱਖਰ *w = ਨਿਰਾ;
        uint64_t v;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&s, &w, /* separators= */ ਨਿਰਾ, /* flags= */ 0);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -EINVAL;

        /* Ensure the line ends in "kB" */
        ਜੇ (!streq(s, "kB"))
                ਵਾਪਸ -EINVAL;

        r = ਸੁਰੱਖਿਅਤ_atou64(w, &v);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (v == UINT64_MAX)
                ਵਾਪਸ -EINVAL;

        ਜੇ (!MUL_ਸੌਂਪੋ_ਸੁਰੱਖਿਅਤ(&v, U64_KB))
                ਵਾਪਸ -EOVERFLOW;

        *ret = v;
        ਵਾਪਸ 0;
}

ਪੂਰਨ procfs_ਸਮ੍ਰਿਤੀ_get(uint64_t *ret_ਕੁੱਲ, uint64_t *ret_ਵਰਤਿਆ) {
        uint64_t mem_ਕੁੱਲ = UINT64_MAX, mem_ਉਪਲਬਧ = UINT64_MAX;
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        ਪੂਰਨ r;

        f = fopen("/proc/meminfo", "re");
        ਜੇ (!f)
                ਵਾਪਸ -errno;

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
                uint64_t *v;
                ਅੱਖਰ *p;

                r = read_ਸਤਰ(f, LONG_ਸਤਰ_MAX, &ਸਤਰ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਵਾਪਸ -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = ਪਹਿਲਾ_ਸ਼ਬਦ(ਸਤਰ, "MemTotal:");
                ਜੇ (p)
                        v = &mem_ਕੁੱਲ;
                ਨਹੀਂਤਾਂ {
                        p = ਪਹਿਲਾ_ਸ਼ਬਦ(ਸਤਰ, "MemAvailable:");
                        ਜੇ (p)
                                v = &mem_ਉਪਲਬਧ;
                        ਨਹੀਂਤਾਂ
                                ਜਾਰੀ;
                }

                r = convert_meminfo_ਮੁੱਲ_ਮੰਜ਼ਿਲ_uint64_ਬਾਈਟਾਂ(p, v);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (mem_ਕੁੱਲ != UINT64_MAX && mem_ਉਪਲਬਧ != UINT64_MAX)
                        ਰੋਕੋ;
        }

        ਜੇ (mem_ਉਪਲਬਧ > mem_ਕੁੱਲ)
                ਵਾਪਸ -EINVAL;

        ਜੇ (ret_ਕੁੱਲ)
                *ret_ਕੁੱਲ = mem_ਕੁੱਲ;
        ਜੇ (ret_ਵਰਤਿਆ)
                *ret_ਵਰਤਿਆ = mem_ਕੁੱਲ - mem_ਉਪਲਬਧ;
        ਵਾਪਸ 0;
}

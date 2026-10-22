/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "procfs-util.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "time-util.h"

పూర్ణాంకం procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ అక్షరం *విలువ = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = read_one_పంక్తి_file("/proc/sys/kernel/pid_max", &విలువ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి సురక్షిత_atou64(విలువ, ret);
}

పూర్ణాంకం procfs_get_దారాలు_max(uint64_t *ret) {
        _cleanup_free_ అక్షరం *విలువ = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = read_one_పంక్తి_file("/proc/sys/kernel/threads-max", &విలువ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి సురక్షిత_atou64(విలువ, ret);
}

పూర్ణాంకం procfs_tasks_సమితి_హద్దు(uint64_t limit) {
        అక్షరం బఫరు[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        పూర్ణాంకం r;

        ఒకవేళ (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                తిరిగి -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        ఒకవేళ (limit > pid_max-1) {
                xsprintf(బఫరు, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", బఫరు, WRITE_STRING_FILE_DISABLE_బఫరు);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        xsprintf(బఫరు, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", బఫరు, WRITE_STRING_FILE_DISABLE_బఫరు);
        ఒకవేళ (r < 0) {
                uint64_t దారాలు_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                ఒకవేళ (procfs_get_దారాలు_max(&దారాలు_max) < 0)
                        తిరిగి r; /* return original error */

                ఒకవేళ (MIN(pid_max - 1, దారాలు_max) != limit)
                        తిరిగి r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        తిరిగి 0;
}

పూర్ణాంకం procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ అక్షరం *విలువ = ఖాళీ;
        స్థిరరాశి అక్షరం *p, *nr;
        size_t n;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = read_one_పంక్తి_file("/proc/loadavg", &విలువ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(విలువ, '/');
        ఒకవేళ (!p)
                తిరిగి -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_సురక్షిత(p, n);

        తిరిగి సురక్షిత_atou64(nr, ret);
}

స్థిర uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        అయితే (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        తిరిగి a;
}

nsec_t procfs_ticks_గమ్యం_nsec(uint64_t ticks) {
        uint64_t ticks_చొప్పున_రెండవ = sysconf_clock_ticks_cached(),
                గరిష్ఠ_సామాన్య_భాజకం = calc_gcd64(NSEC_చొప్పున_SEC, ticks_చొప్పున_రెండవ);

        /* Reduce NSEC_PER_SEC/ticks_per_second first to avoid overflow of ticks * NSEC_PER_SEC, and split
         * the multiplication into whole and remainder parts, so that the result stays exact.
         *
         * Note that in most cases b == 1 and the second term is trivial. */
        uint64_t a = NSEC_చొప్పున_SEC / గరిష్ఠ_సామాన్య_భాజకం,
                 b = ticks_చొప్పున_రెండవ / గరిష్ఠ_సామాన్య_భాజకం;

        తిరిగి ticks / b * a + DIV_విడత_పైకి(ticks % b * a, b);
}

పూర్ణాంకం procfs_cpu_get_usage(nsec_t *ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = procfs_ticks_గమ్యం_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        తిరిగి 0;
}

పూర్ణాంకం procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ అక్షరం *మొదటి_పంక్తి = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = read_one_పంక్తి_file("/proc/stat", &మొదటి_పంక్తి);
        ఒకవేళ (r < 0)
                తిరిగి r;

        స్థిరరాశి అక్షరం *p = మొదటి_మాట(మొదటి_పంక్తి, "cpu");
        ఒకవేళ (!p)
                తిరిగి -EINVAL;

        /* All eight fields exist on any kernel we support: iowait was added in 2.5.41, steal in 2.6.11.
         * "guest" and "guest_nice" are not returned, the kernel already includes them in "user" and "nice".
         * Nevertheless, this file might be synthesized slopilly, so let's not require the last three fields.
         * We don't care about them in most cases anyway. */
        ProcfsCpuTicks t = {};
        r = అక్షరశ్రేణి_చదువు(p, "%"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64,
                   &t.user,
                   &t.nice,
                   &t.system,
                   &t.idle,
                   &t.iowait,
                   &t.irq,
                   &t.softirq,
                   &t.steal);
        ఒకవేళ (r < 5)
                తిరిగి -EINVAL;
        ఒకవేళ (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        తిరిగి 0;
}

పూర్ణాంకం convert_meminfo_విలువ_గమ్యం_uint64_బైట్లు(స్థిరరాశి అక్షరం *s, uint64_t *ret) {
        _cleanup_free_ అక్షరం *w = ఖాళీ;
        uint64_t v;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(ret);

        r = వెలికితీయి_మొదటి_మాట(&s, &w, /* separators= */ ఖాళీ, /* flags= */ 0);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -EINVAL;

        /* Ensure the line ends in "kB" */
        ఒకవేళ (!streq(s, "kB"))
                తిరిగి -EINVAL;

        r = సురక్షిత_atou64(w, &v);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (v == UINT64_MAX)
                తిరిగి -EINVAL;

        ఒకవేళ (!MUL_కేటాయించు_సురక్షిత(&v, U64_KB))
                తిరిగి -EOVERFLOW;

        *ret = v;
        తిరిగి 0;
}

పూర్ణాంకం procfs_జ్ఞాపకం_get(uint64_t *ret_మొత్తం, uint64_t *ret_వాడిన) {
        uint64_t mem_మొత్తం = UINT64_MAX, mem_అందుబాటు = UINT64_MAX;
        _cleanup_fclose_ FILE *f = ఖాళీ;
        పూర్ణాంకం r;

        f = fopen("/proc/meminfo", "re");
        ఒకవేళ (!f)
                తిరిగి -errno;

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
                uint64_t *v;
                అక్షరం *p;

                r = read_పంక్తి(f, LONG_పంక్తి_MAX, &పంక్తి);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        తిరిగి -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = మొదటి_మాట(పంక్తి, "MemTotal:");
                ఒకవేళ (p)
                        v = &mem_మొత్తం;
                లేకపోతే {
                        p = మొదటి_మాట(పంక్తి, "MemAvailable:");
                        ఒకవేళ (p)
                                v = &mem_అందుబాటు;
                        లేకపోతే
                                కొనసాగు;
                }

                r = convert_meminfo_విలువ_గమ్యం_uint64_బైట్లు(p, v);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (mem_మొత్తం != UINT64_MAX && mem_అందుబాటు != UINT64_MAX)
                        ఆపు;
        }

        ఒకవేళ (mem_అందుబాటు > mem_మొత్తం)
                తిరిగి -EINVAL;

        ఒకవేళ (ret_మొత్తం)
                *ret_మొత్తం = mem_మొత్తం;
        ఒకవేళ (ret_వాడిన)
                *ret_వాడిన = mem_మొత్తం - mem_అందుబాటు;
        తిరిగి 0;
}

/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "procfs-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"

ಪೂರ್ಣಾಂಕ procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಮೌಲ್ಯ = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = read_one_ಸಾಲು_file("/proc/sys/kernel/pid_max", &ಮೌಲ್ಯ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_atou64(ಮೌಲ್ಯ, ret);
}

ಪೂರ್ಣಾಂಕ procfs_get_ಎಳೆಗಳು_max(uint64_t *ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಮೌಲ್ಯ = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = read_one_ಸಾಲು_file("/proc/sys/kernel/threads-max", &ಮೌಲ್ಯ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_atou64(ಮೌಲ್ಯ, ret);
}

ಪೂರ್ಣಾಂಕ procfs_tasks_ಗಣ_ಎಲ್ಲೆ(uint64_t limit) {
        ಅಕ್ಷರ ಬಫರ್[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        ಆದರೆ (limit > pid_max-1) {
                xsprintf(ಬಫರ್, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", ಬಫರ್, WRITE_STRING_FILE_DISABLE_ಬಫರ್);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        xsprintf(ಬಫರ್, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", ಬಫರ್, WRITE_STRING_FILE_DISABLE_ಬಫರ್);
        ಆದರೆ (r < 0) {
                uint64_t ಎಳೆಗಳು_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                ಆದರೆ (procfs_get_ಎಳೆಗಳು_max(&ಎಳೆಗಳು_max) < 0)
                        ಹಿಂತಿರುಗಿ r; /* return original error */

                ಆದರೆ (MIN(pid_max - 1, ಎಳೆಗಳು_max) != limit)
                        ಹಿಂತಿರುಗಿ r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಮೌಲ್ಯ = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, *nr;
        size_t n;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = read_one_ಸಾಲು_file("/proc/loadavg", &ಮೌಲ್ಯ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(ಮೌಲ್ಯ, '/');
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_ಸುರಕ್ಷಿತ(p, n);

        ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_atou64(nr, ret);
}

ಸ್ಥಿರ uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        ಆಗ (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        ಹಿಂತಿರುಗಿ a;
}

nsec_t procfs_ticks_ಗಮ್ಯ_nsec(uint64_t ticks) {
        uint64_t ticks_ಪ್ರತಿ_ಎರಡನೇ = sysconf_clock_ticks_cached(),
                ಮಹತ್ತಮ_ಸಾಮಾನ್ಯ_ಅಪವರ್ತನ = calc_gcd64(NSEC_ಪ್ರತಿ_SEC, ticks_ಪ್ರತಿ_ಎರಡನೇ);

        /* Reduce NSEC_PER_SEC/ticks_per_second first to avoid overflow of ticks * NSEC_PER_SEC, and split
         * the multiplication into whole and remainder parts, so that the result stays exact.
         *
         * Note that in most cases b == 1 and the second term is trivial. */
        uint64_t a = NSEC_ಪ್ರತಿ_SEC / ಮಹತ್ತಮ_ಸಾಮಾನ್ಯ_ಅಪವರ್ತನ,
                 b = ticks_ಪ್ರತಿ_ಎರಡನೇ / ಮಹತ್ತಮ_ಸಾಮಾನ್ಯ_ಅಪವರ್ತನ;

        ಹಿಂತಿರುಗಿ ticks / b * a + DIV_ಸುತ್ತು_ಮೇಲೆ(ticks % b * a, b);
}

ಪೂರ್ಣಾಂಕ procfs_cpu_get_usage(nsec_t *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = procfs_ticks_ಗಮ್ಯ_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಮೊದಲ_ಸಾಲು = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = read_one_ಸಾಲು_file("/proc/stat", &ಮೊದಲ_ಸಾಲು);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಮೊದಲ_ಶಬ್ದ(ಮೊದಲ_ಸಾಲು, "cpu");
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* All eight fields exist on any kernel we support: iowait was added in 2.5.41, steal in 2.6.11.
         * "guest" and "guest_nice" are not returned, the kernel already includes them in "user" and "nice".
         * Nevertheless, this file might be synthesized slopilly, so let's not require the last three fields.
         * We don't care about them in most cases anyway. */
        ProcfsCpuTicks t = {};
        r = ಅಕ್ಷರಸರಣಿ_ಓದು(p, "%"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64,
                   &t.user,
                   &t.nice,
                   &t.system,
                   &t.idle,
                   &t.iowait,
                   &t.irq,
                   &t.softirq,
                   &t.steal);
        ಆದರೆ (r < 5)
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ convert_meminfo_ಮೌಲ್ಯ_ಗಮ್ಯ_uint64_ಬೈಟುಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uint64_t *ret) {
        _cleanup_free_ ಅಕ್ಷರ *w = ಖಾಲಿ;
        uint64_t v;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&s, &w, /* separators= */ ಖಾಲಿ, /* flags= */ 0);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* Ensure the line ends in "kB" */
        ಆದರೆ (!streq(s, "kB"))
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = ಸುರಕ್ಷಿತ_atou64(w, &v);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (v == UINT64_MAX)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (!MUL_ನಿಯೋಜಿಸು_ಸುರಕ್ಷಿತ(&v, U64_KB))
                ಹಿಂತಿರುಗಿ -EOVERFLOW;

        *ret = v;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ procfs_ಸ್ಮರಣೆ_get(uint64_t *ret_ಒಟ್ಟು, uint64_t *ret_ಬಳಸಿದ) {
        uint64_t mem_ಒಟ್ಟು = UINT64_MAX, mem_ಲಭ್ಯ = UINT64_MAX;
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        f = fopen("/proc/meminfo", "re");
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ -errno;

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
                uint64_t *v;
                ಅಕ್ಷರ *p;

                r = read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ಹಿಂತಿರುಗಿ -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = ಮೊದಲ_ಶಬ್ದ(ಸಾಲು, "MemTotal:");
                ಆದರೆ (p)
                        v = &mem_ಒಟ್ಟು;
                ಇಲ್ಲದಿದ್ದರೆ {
                        p = ಮೊದಲ_ಶಬ್ದ(ಸಾಲು, "MemAvailable:");
                        ಆದರೆ (p)
                                v = &mem_ಲಭ್ಯ;
                        ಇಲ್ಲದಿದ್ದರೆ
                                ಮುಂದುವರಿ;
                }

                r = convert_meminfo_ಮೌಲ್ಯ_ಗಮ್ಯ_uint64_ಬೈಟುಗಳು(p, v);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (mem_ಒಟ್ಟು != UINT64_MAX && mem_ಲಭ್ಯ != UINT64_MAX)
                        ನಿಲ್ಲಿಸು;
        }

        ಆದರೆ (mem_ಲಭ್ಯ > mem_ಒಟ್ಟು)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ret_ಒಟ್ಟು)
                *ret_ಒಟ್ಟು = mem_ಒಟ್ಟು;
        ಆದರೆ (ret_ಬಳಸಿದ)
                *ret_ಬಳಸಿದ = mem_ಒಟ್ಟು - mem_ಲಭ್ಯ;
        ಹಿಂತಿರುಗಿ 0;
}

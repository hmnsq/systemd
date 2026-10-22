/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "procfs-util.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "time-util.h"

പൂർണ്ണസംഖ്യ procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ അക്ഷരം *മൂല്യം = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = read_one_വരി_file("/proc/sys/kernel/pid_max", &മൂല്യം);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ safe_atou64(മൂല്യം, ret);
}

പൂർണ്ണസംഖ്യ procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ അക്ഷരം *മൂല്യം = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = read_one_വരി_file("/proc/sys/kernel/threads-max", &മൂല്യം);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ safe_atou64(മൂല്യം, ret);
}

പൂർണ്ണസംഖ്യ procfs_tasks_set_limit(uint64_t limit) {
        അക്ഷരം buffer[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                തിരികെ -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        എങ്കിൽ (limit > pid_max-1) {
                xsprintf(buffer, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        xsprintf(buffer, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
        എങ്കിൽ (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                എങ്കിൽ (procfs_get_threads_max(&threads_max) < 0)
                        തിരികെ r; /* return original error */

                എങ്കിൽ (MIN(pid_max - 1, threads_max) != limit)
                        തിരികെ r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ അക്ഷരം *മൂല്യം = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *p, *nr;
        size_t n;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = read_one_വരി_file("/proc/loadavg", &മൂല്യം);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(മൂല്യം, '/');
        എങ്കിൽ (!p)
                തിരികെ -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_safe(p, n);

        തിരികെ safe_atou64(nr, ret);
}

സ്ഥിതം uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        ആകുമ്പോൾ (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        തിരികെ a;
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

        തിരികെ ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

പൂർണ്ണസംഖ്യ procfs_cpu_get_usage(nsec_t *ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ അക്ഷരം *first_വരി = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = read_one_വരി_file("/proc/stat", &first_വരി);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        സ്ഥിരം അക്ഷരം *p = first_word(first_വരി, "cpu");
        എങ്കിൽ (!p)
                തിരികെ -EINVAL;

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
        എങ്കിൽ (r < 5)
                തിരികെ -EINVAL;
        എങ്കിൽ (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ convert_meminfo_മൂല്യം_to_uint64_bytes(സ്ഥിരം അക്ഷരം *s, uint64_t *ret) {
        _cleanup_free_ അക്ഷരം *w = ശൂന്യ;
        uint64_t v;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(ret);

        r = extract_first_word(&s, &w, /* separators= */ ശൂന്യ, /* flags= */ 0);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -EINVAL;

        /* Ensure the line ends in "kB" */
        എങ്കിൽ (!streq(s, "kB"))
                തിരികെ -EINVAL;

        r = safe_atou64(w, &v);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (v == UINT64_MAX)
                തിരികെ -EINVAL;

        എങ്കിൽ (!MUL_ASSIGN_SAFE(&v, U64_KB))
                തിരികെ -EOVERFLOW;

        *ret = v;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used) {
        uint64_t mem_total = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        f = fopen("/proc/meminfo", "re");
        എങ്കിൽ (!f)
                തിരികെ -errno;

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;
                uint64_t *v;
                അക്ഷരം *p;

                r = read_വരി(f, LONG_വരി_MAX, &വരി);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        തിരികെ -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(വരി, "MemTotal:");
                എങ്കിൽ (p)
                        v = &mem_total;
                അല്ലെങ്കിൽ {
                        p = first_word(വരി, "MemAvailable:");
                        എങ്കിൽ (p)
                                v = &mem_available;
                        അല്ലെങ്കിൽ
                                തുടരുക;
                }

                r = convert_meminfo_മൂല്യം_to_uint64_bytes(p, v);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (mem_total != UINT64_MAX && mem_available != UINT64_MAX)
                        നിർത്തുക;
        }

        എങ്കിൽ (mem_available > mem_total)
                തിരികെ -EINVAL;

        എങ്കിൽ (ret_total)
                *ret_total = mem_total;
        എങ്കിൽ (ret_used)
                *ret_used = mem_total - mem_available;
        തിരികെ 0;
}

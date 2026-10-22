/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "alloc-util.h"
#include "extract-word.h"
#include "fd-util.h"
#include "fileio.h"
#include "log.h"
#include "parse-util.h"
#include "process-util.h"
#include "procfs-util.h"
#include "stdio-util.h"
#include "string-util.h"
#include "time-util.h"

số_nguyên procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ char *value = NULL;
        số_nguyên r;

        assert(ret);

        r = read_one_line_file("/proc/sys/kernel/pid_max", &value);
        nếu (r < 0)
                trả_về r;

        trả_về safe_atou64(value, ret);
}

số_nguyên procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ char *value = NULL;
        số_nguyên r;

        assert(ret);

        r = read_one_line_file("/proc/sys/kernel/threads-max", &value);
        nếu (r < 0)
                trả_về r;

        trả_về safe_atou64(value, ret);
}

số_nguyên procfs_tasks_set_limit(uint64_t limit) {
        char buffer[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        số_nguyên r;

        nếu (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                trả_về -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        nếu (r < 0)
                trả_về r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        nếu (limit > pid_max-1) {
                xsprintf(buffer, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
                nếu (r < 0)
                        trả_về r;
        }

        xsprintf(buffer, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
        nếu (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                nếu (procfs_get_threads_max(&threads_max) < 0)
                        trả_về r; /* return original error */

                nếu (MIN(pid_max - 1, threads_max) != limit)
                        trả_về r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        trả_về 0;
}

số_nguyên procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ char *value = NULL;
        const char *p, *nr;
        size_t n;
        số_nguyên r;

        assert(ret);

        r = read_one_line_file("/proc/loadavg", &value);
        nếu (r < 0)
                trả_về r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(value, '/');
        nếu (!p)
                trả_về -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_safe(p, n);

        trả_về safe_atou64(nr, ret);
}

static uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        trong_khi (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        trả_về a;
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

        trả_về ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

số_nguyên procfs_cpu_get_usage(nsec_t *ret) {
        số_nguyên r;

        assert(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        nếu (r < 0)
                trả_về r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        trả_về 0;
}

số_nguyên procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ char *first_line = NULL;
        số_nguyên r;

        assert(ret);

        r = read_one_line_file("/proc/stat", &first_line);
        nếu (r < 0)
                trả_về r;

        const char *p = first_word(first_line, "cpu");
        nếu (!p)
                trả_về -EINVAL;

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
        nếu (r < 5)
                trả_về -EINVAL;
        nếu (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        trả_về 0;
}

số_nguyên convert_meminfo_value_to_uint64_bytes(const char *s, uint64_t *ret) {
        _cleanup_free_ char *w = NULL;
        uint64_t v;
        số_nguyên r;

        assert(s);
        assert(ret);

        r = extract_first_word(&s, &w, /* separators= */ NULL, /* flags= */ 0);
        nếu (r < 0)
                trả_về r;
        nếu (r == 0)
                trả_về -EINVAL;

        /* Ensure the line ends in "kB" */
        nếu (!streq(s, "kB"))
                trả_về -EINVAL;

        r = safe_atou64(w, &v);
        nếu (r < 0)
                trả_về r;
        nếu (v == UINT64_MAX)
                trả_về -EINVAL;

        nếu (!MUL_ASSIGN_SAFE(&v, U64_KB))
                trả_về -EOVERFLOW;

        *ret = v;
        trả_về 0;
}

số_nguyên procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used) {
        uint64_t mem_total = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ FILE *f = NULL;
        số_nguyên r;

        f = fopen("/proc/meminfo", "re");
        nếu (!f)
                trả_về -errno;

        cho (;;) {
                _cleanup_free_ char *line = NULL;
                uint64_t *v;
                char *p;

                r = read_line(f, LONG_LINE_MAX, &line);
                nếu (r < 0)
                        trả_về r;
                nếu (r == 0)
                        trả_về -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(line, "MemTotal:");
                nếu (p)
                        v = &mem_total;
                không_thì {
                        p = first_word(line, "MemAvailable:");
                        nếu (p)
                                v = &mem_available;
                        không_thì
                                continue;
                }

                r = convert_meminfo_value_to_uint64_bytes(p, v);
                nếu (r < 0)
                        trả_về r;

                nếu (mem_total != UINT64_MAX && mem_available != UINT64_MAX)
                        break;
        }

        nếu (mem_available > mem_total)
                trả_về -EINVAL;

        nếu (ret_total)
                *ret_total = mem_total;
        nếu (ret_used)
                *ret_used = mem_total - mem_available;
        trả_về 0;
}

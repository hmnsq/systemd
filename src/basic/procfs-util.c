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

ціле procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ char *value = NULL;
        ціле r;

        assert(ret);

        r = read_one_line_file("/proc/sys/kernel/pid_max", &value);
        якщо (r < 0)
                повернути r;

        повернути safe_atou64(value, ret);
}

ціле procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ char *value = NULL;
        ціле r;

        assert(ret);

        r = read_one_line_file("/proc/sys/kernel/threads-max", &value);
        якщо (r < 0)
                повернути r;

        повернути safe_atou64(value, ret);
}

ціле procfs_tasks_set_limit(uint64_t limit) {
        char buffer[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        ціле r;

        якщо (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                повернути -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        якщо (r < 0)
                повернути r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        якщо (limit > pid_max-1) {
                xsprintf(buffer, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
                якщо (r < 0)
                        повернути r;
        }

        xsprintf(buffer, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
        якщо (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                якщо (procfs_get_threads_max(&threads_max) < 0)
                        повернути r; /* return original error */

                якщо (MIN(pid_max - 1, threads_max) != limit)
                        повернути r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        повернути 0;
}

ціле procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ char *value = NULL;
        const char *p, *nr;
        size_t n;
        ціле r;

        assert(ret);

        r = read_one_line_file("/proc/loadavg", &value);
        якщо (r < 0)
                повернути r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(value, '/');
        якщо (!p)
                повернути -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_safe(p, n);

        повернути safe_atou64(nr, ret);
}

static uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        поки (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        повернути a;
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

        повернути ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

ціле procfs_cpu_get_usage(nsec_t *ret) {
        ціле r;

        assert(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        якщо (r < 0)
                повернути r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        повернути 0;
}

ціле procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ char *first_line = NULL;
        ціле r;

        assert(ret);

        r = read_one_line_file("/proc/stat", &first_line);
        якщо (r < 0)
                повернути r;

        const char *p = first_word(first_line, "cpu");
        якщо (!p)
                повернути -EINVAL;

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
        якщо (r < 5)
                повернути -EINVAL;
        якщо (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        повернути 0;
}

ціле convert_meminfo_value_to_uint64_bytes(const char *s, uint64_t *ret) {
        _cleanup_free_ char *w = NULL;
        uint64_t v;
        ціле r;

        assert(s);
        assert(ret);

        r = extract_first_word(&s, &w, /* separators= */ NULL, /* flags= */ 0);
        якщо (r < 0)
                повернути r;
        якщо (r == 0)
                повернути -EINVAL;

        /* Ensure the line ends in "kB" */
        якщо (!streq(s, "kB"))
                повернути -EINVAL;

        r = safe_atou64(w, &v);
        якщо (r < 0)
                повернути r;
        якщо (v == UINT64_MAX)
                повернути -EINVAL;

        якщо (!MUL_ASSIGN_SAFE(&v, U64_KB))
                повернути -EOVERFLOW;

        *ret = v;
        повернути 0;
}

ціле procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used) {
        uint64_t mem_total = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ FILE *f = NULL;
        ціле r;

        f = fopen("/proc/meminfo", "re");
        якщо (!f)
                повернути -errno;

        для (;;) {
                _cleanup_free_ char *line = NULL;
                uint64_t *v;
                char *p;

                r = read_line(f, LONG_LINE_MAX, &line);
                якщо (r < 0)
                        повернути r;
                якщо (r == 0)
                        повернути -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(line, "MemTotal:");
                якщо (p)
                        v = &mem_total;
                інакше {
                        p = first_word(line, "MemAvailable:");
                        якщо (p)
                                v = &mem_available;
                        інакше
                                continue;
                }

                r = convert_meminfo_value_to_uint64_bytes(p, v);
                якщо (r < 0)
                        повернути r;

                якщо (mem_total != UINT64_MAX && mem_available != UINT64_MAX)
                        break;
        }

        якщо (mem_available > mem_total)
                повернути -EINVAL;

        якщо (ret_total)
                *ret_total = mem_total;
        якщо (ret_used)
                *ret_used = mem_total - mem_available;
        повернути 0;
}

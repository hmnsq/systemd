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

ကိန်းပြည့် procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ var_na *value = NULL;
        ကိန်းပြည့် r;

        assert(ret);

        r = read_one_line_file("/proc/sys/kernel/pid_max", &value);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး safe_atou64(value, ret);
}

ကိန်းပြည့် procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ var_na *value = NULL;
        ကိန်းပြည့် r;

        assert(ret);

        r = read_one_line_file("/proc/sys/kernel/threads-max", &value);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး safe_atou64(value, ret);
}

ကိန်းပြည့် procfs_tasks_set_limit(uint64_t limit) {
        var_na buffer[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        ကိန်းပြည့် r;

        အကယ်၍ (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                ပြန်ပေး -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        အကယ်၍ (limit > pid_max-1) {
                xsprintf(buffer, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        xsprintf(buffer, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
        အကယ်၍ (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                အကယ်၍ (procfs_get_threads_max(&threads_max) < 0)
                        ပြန်ပေး r; /* return original error */

                အကယ်၍ (MIN(pid_max - 1, threads_max) != limit)
                        ပြန်ပေး r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        ပြန်ပေး 0;
}

ကိန်းပြည့် procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ var_na *value = NULL;
        const var_na *p, *nr;
        size_t n;
        ကိန်းပြည့် r;

        assert(ret);

        r = read_one_line_file("/proc/loadavg", &value);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(value, '/');
        အကယ်၍ (!p)
                ပြန်ပေး -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_safe(p, n);

        ပြန်ပေး safe_atou64(nr, ret);
}

static uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        နေစဉ် (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        ပြန်ပေး a;
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

        ပြန်ပေး ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

ကိန်းပြည့် procfs_cpu_get_usage(nsec_t *ret) {
        ကိန်းပြည့် r;

        assert(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        ပြန်ပေး 0;
}

ကိန်းပြည့် procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ var_na *first_line = NULL;
        ကိန်းပြည့် r;

        assert(ret);

        r = read_one_line_file("/proc/stat", &first_line);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        const var_na *p = first_word(first_line, "cpu");
        အကယ်၍ (!p)
                ပြန်ပေး -EINVAL;

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
        အကယ်၍ (r < 5)
                ပြန်ပေး -EINVAL;
        အကယ်၍ (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        ပြန်ပေး 0;
}

ကိန်းပြည့် convert_meminfo_value_to_uint64_bytes(const var_na *s, uint64_t *ret) {
        _cleanup_free_ var_na *w = NULL;
        uint64_t v;
        ကိန်းပြည့် r;

        assert(s);
        assert(ret);

        r = extract_first_word(&s, &w, /* separators= */ NULL, /* flags= */ 0);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (r == 0)
                ပြန်ပေး -EINVAL;

        /* Ensure the line ends in "kB" */
        အကယ်၍ (!streq(s, "kB"))
                ပြန်ပေး -EINVAL;

        r = safe_atou64(w, &v);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (v == UINT64_MAX)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (!MUL_ASSIGN_SAFE(&v, U64_KB))
                ပြန်ပေး -EOVERFLOW;

        *ret = v;
        ပြန်ပေး 0;
}

ကိန်းပြည့် procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used) {
        uint64_t mem_total = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ FILE *f = NULL;
        ကိန်းပြည့် r;

        f = fopen("/proc/meminfo", "re");
        အကယ်၍ (!f)
                ပြန်ပေး -errno;

        အတွက် (;;) {
                _cleanup_free_ var_na *line = NULL;
                uint64_t *v;
                var_na *p;

                r = read_line(f, LONG_LINE_MAX, &line);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                အကယ်၍ (r == 0)
                        ပြန်ပေး -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(line, "MemTotal:");
                အကယ်၍ (p)
                        v = &mem_total;
                မဟုတ်ပါက {
                        p = first_word(line, "MemAvailable:");
                        အကယ်၍ (p)
                                v = &mem_available;
                        မဟုတ်ပါက
                                continue;
                }

                r = convert_meminfo_value_to_uint64_bytes(p, v);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (mem_total != UINT64_MAX && mem_available != UINT64_MAX)
                        bha_nga;
        }

        အကယ်၍ (mem_available > mem_total)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (ret_total)
                *ret_total = mem_total;
        အကယ်၍ (ret_used)
                *ret_used = mem_total - mem_available;
        ပြန်ပေး 0;
}

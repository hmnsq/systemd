/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "procfs-util.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "time-util.h"

முழு procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ எழுத்து *மதிப்பு = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = read_one_வரி_file("/proc/sys/kernel/pid_max", &மதிப்பு);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு safe_atou64(மதிப்பு, ret);
}

முழு procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ எழுத்து *மதிப்பு = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = read_one_வரி_file("/proc/sys/kernel/threads-max", &மதிப்பு);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு safe_atou64(மதிப்பு, ret);
}

முழு procfs_tasks_set_limit(uint64_t limit) {
        எழுத்து buffer[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        முழு r;

        எனில் (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                திரும்பு -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        எனில் (r < 0)
                திரும்பு r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        எனில் (limit > pid_max-1) {
                xsprintf(buffer, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
                எனில் (r < 0)
                        திரும்பு r;
        }

        xsprintf(buffer, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
        எனில் (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                எனில் (procfs_get_threads_max(&threads_max) < 0)
                        திரும்பு r; /* return original error */

                எனில் (MIN(pid_max - 1, threads_max) != limit)
                        திரும்பு r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        திரும்பு 0;
}

முழு procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ எழுத்து *மதிப்பு = வெற்று;
        மாறிலி எழுத்து *p, *nr;
        size_t n;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = read_one_வரி_file("/proc/loadavg", &மதிப்பு);
        எனில் (r < 0)
                திரும்பு r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(மதிப்பு, '/');
        எனில் (!p)
                திரும்பு -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_safe(p, n);

        திரும்பு safe_atou64(nr, ret);
}

நிலையான uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        போது (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        திரும்பு a;
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

        திரும்பு ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

முழு procfs_cpu_get_usage(nsec_t *ret) {
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        எனில் (r < 0)
                திரும்பு r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        திரும்பு 0;
}

முழு procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ எழுத்து *first_வரி = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = read_one_வரி_file("/proc/stat", &first_வரி);
        எனில் (r < 0)
                திரும்பு r;

        மாறிலி எழுத்து *p = first_word(first_வரி, "cpu");
        எனில் (!p)
                திரும்பு -EINVAL;

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
        எனில் (r < 5)
                திரும்பு -EINVAL;
        எனில் (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        திரும்பு 0;
}

முழு convert_meminfo_மதிப்பு_to_uint64_bytes(மாறிலி எழுத்து *s, uint64_t *ret) {
        _cleanup_free_ எழுத்து *w = வெற்று;
        uint64_t v;
        முழு r;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(ret);

        r = extract_first_word(&s, &w, /* separators= */ வெற்று, /* flags= */ 0);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -EINVAL;

        /* Ensure the line ends in "kB" */
        எனில் (!streq(s, "kB"))
                திரும்பு -EINVAL;

        r = safe_atou64(w, &v);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (v == UINT64_MAX)
                திரும்பு -EINVAL;

        எனில் (!MUL_ASSIGN_SAFE(&v, U64_KB))
                திரும்பு -EOVERFLOW;

        *ret = v;
        திரும்பு 0;
}

முழு procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used) {
        uint64_t mem_total = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ FILE *f = வெற்று;
        முழு r;

        f = fopen("/proc/meminfo", "re");
        எனில் (!f)
                திரும்பு -errno;

        க்கு (;;) {
                _cleanup_free_ எழுத்து *வரி = வெற்று;
                uint64_t *v;
                எழுத்து *p;

                r = read_வரி(f, LONG_வரி_MAX, &வரி);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        திரும்பு -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(வரி, "MemTotal:");
                எனில் (p)
                        v = &mem_total;
                இல்லையெனில் {
                        p = first_word(வரி, "MemAvailable:");
                        எனில் (p)
                                v = &mem_available;
                        இல்லையெனில்
                                தொடர்;
                }

                r = convert_meminfo_மதிப்பு_to_uint64_bytes(p, v);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (mem_total != UINT64_MAX && mem_available != UINT64_MAX)
                        நிறுத்து;
        }

        எனில் (mem_available > mem_total)
                திரும்பு -EINVAL;

        எனில் (ret_total)
                *ret_total = mem_total;
        எனில் (ret_used)
                *ret_used = mem_total - mem_available;
        திரும்பு 0;
}

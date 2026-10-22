/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "procfs-util.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "time-util.h"

પૂર્ણાંક procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ અક્ષર *કિંમત = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = read_one_લીટી_file("/proc/sys/kernel/pid_max", &કિંમત);
        જો (r < 0)
                પાછા r;

        પાછા safe_atou64(કિંમત, ret);
}

પૂર્ણાંક procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ અક્ષર *કિંમત = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = read_one_લીટી_file("/proc/sys/kernel/threads-max", &કિંમત);
        જો (r < 0)
                પાછા r;

        પાછા safe_atou64(કિંમત, ret);
}

પૂર્ણાંક procfs_tasks_set_limit(uint64_t limit) {
        અક્ષર buffer[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        પૂર્ણાંક r;

        જો (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                પાછા -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        જો (r < 0)
                પાછા r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        જો (limit > pid_max-1) {
                xsprintf(buffer, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
                જો (r < 0)
                        પાછા r;
        }

        xsprintf(buffer, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", buffer, WRITE_STRING_FILE_DISABLE_BUFFER);
        જો (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                જો (procfs_get_threads_max(&threads_max) < 0)
                        પાછા r; /* return original error */

                જો (MIN(pid_max - 1, threads_max) != limit)
                        પાછા r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        પાછા 0;
}

પૂર્ણાંક procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ અક્ષર *કિંમત = ખાલી;
        અચલ અક્ષર *p, *nr;
        size_t n;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = read_one_લીટી_file("/proc/loadavg", &કિંમત);
        જો (r < 0)
                પાછા r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = strchr(કિંમત, '/');
        જો (!p)
                પાછા -EINVAL;

        p++;
        n = strspn(p, DIGITS);
        nr = strndupa_safe(p, n);

        પાછા safe_atou64(nr, ret);
}

સ્થિર uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        જ્યારે (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        પાછા a;
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

        પાછા ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

પૂર્ણાંક procfs_cpu_get_usage(nsec_t *ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        જો (r < 0)
                પાછા r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.system + ticks.irq + ticks.softirq);
        પાછા 0;
}

પૂર્ણાંક procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ અક્ષર *first_લીટી = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = read_one_લીટી_file("/proc/stat", &first_લીટી);
        જો (r < 0)
                પાછા r;

        અચલ અક્ષર *p = first_word(first_લીટી, "cpu");
        જો (!p)
                પાછા -EINVAL;

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
        જો (r < 5)
                પાછા -EINVAL;
        જો (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        પાછા 0;
}

પૂર્ણાંક convert_meminfo_કિંમત_to_uint64_bytes(અચલ અક્ષર *s, uint64_t *ret) {
        _cleanup_free_ અક્ષર *w = ખાલી;
        uint64_t v;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(ret);

        r = extract_first_word(&s, &w, /* separators= */ ખાલી, /* flags= */ 0);
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -EINVAL;

        /* Ensure the line ends in "kB" */
        જો (!streq(s, "kB"))
                પાછા -EINVAL;

        r = safe_atou64(w, &v);
        જો (r < 0)
                પાછા r;
        જો (v == UINT64_MAX)
                પાછા -EINVAL;

        જો (!MUL_ASSIGN_SAFE(&v, U64_KB))
                પાછા -EOVERFLOW;

        *ret = v;
        પાછા 0;
}

પૂર્ણાંક procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used) {
        uint64_t mem_total = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ FILE *f = ખાલી;
        પૂર્ણાંક r;

        f = fopen("/proc/meminfo", "re");
        જો (!f)
                પાછા -errno;

        માટે (;;) {
                _cleanup_free_ અક્ષર *લીટી = ખાલી;
                uint64_t *v;
                અક્ષર *p;

                r = read_લીટી(f, LONG_લીટી_MAX, &લીટી);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        પાછા -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(લીટી, "MemTotal:");
                જો (p)
                        v = &mem_total;
                નહીંતર {
                        p = first_word(લીટી, "MemAvailable:");
                        જો (p)
                                v = &mem_available;
                        નહીંતર
                                ચાલુ;
                }

                r = convert_meminfo_કિંમત_to_uint64_bytes(p, v);
                જો (r < 0)
                        પાછા r;

                જો (mem_total != UINT64_MAX && mem_available != UINT64_MAX)
                        તોડો;
        }

        જો (mem_available > mem_total)
                પાછા -EINVAL;

        જો (ret_total)
                *ret_total = mem_total;
        જો (ret_used)
                *ret_used = mem_total - mem_available;
        પાછા 0;
}

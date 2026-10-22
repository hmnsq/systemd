/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "extract-word.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "log.h"
#समावेश "parse-util.h"
#समावेश "process-util.h"
#समावेश "procfs-util.h"
#समावेश "stdio-util.h"
#समावेश "string-util.h"
#समावेश "time-util.h"

पूर्णांक procfs_get_pid_max(uint64_t *ret) {
        _cleanup_free_ अक्षर *मूल्य = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        r = read_one_पंक्ति_file("/proc/sys/kernel/pid_max", &मूल्य);
        अगर (r < 0)
                वापस r;

        वापस safe_atou64(मूल्य, ret);
}

पूर्णांक procfs_get_threads_max(uint64_t *ret) {
        _cleanup_free_ अक्षर *मूल्य = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        r = read_one_पंक्ति_file("/proc/sys/kernel/threads-max", &मूल्य);
        अगर (r < 0)
                वापस r;

        वापस safe_atou64(मूल्य, ret);
}

पूर्णांक procfs_tasks_set_limit(uint64_t limit) {
        अक्षर बफर[DECIMAL_STR_MAX(uint64_t)+1];
        uint64_t pid_max;
        पूर्णांक r;

        अगर (limit == 0) /* This makes no sense, we are userspace and hence count as tasks too, and we want to live,
                         * hence the limit conceptually has to be above 0. Also, most likely if anyone asks for a zero
                         * limit they probably mean "no limit", hence let's better refuse this to avoid
                         * confusion. */
                वापस -EINVAL;

        /* The Linux kernel doesn't allow this value to go below 20, hence don't allow this either, higher values than
         * TASKS_MAX are not accepted by the pid_max sysctl. We'll treat anything this high as "unbounded" and hence
         * set it to the maximum. */
        limit = CLAMP(limit, 20U, TASKS_MAX);

        r = procfs_get_pid_max(&pid_max);
        अगर (r < 0)
                वापस r;

        /* As pid_max is about the numeric pid_t range we'll bump it if necessary, but only ever increase it, never
         * decrease it, as threads-max is the much more relevant sysctl. */
        अगर (limit > pid_max-1) {
                xsprintf(बफर, "%" PRIu64, limit+1); /* Add one, since PID 0 is not a valid PID */
                r = write_string_file("/proc/sys/kernel/pid_max", बफर, WRITE_STRING_FILE_DISABLE_बफर);
                अगर (r < 0)
                        वापस r;
        }

        xsprintf(बफर, "%" PRIu64, limit);
        r = write_string_file("/proc/sys/kernel/threads-max", बफर, WRITE_STRING_FILE_DISABLE_बफर);
        अगर (r < 0) {
                uint64_t threads_max;

                /* Hmm, we couldn't write this? If so, maybe it was already set properly? In that case let's not
                 * generate an error */

                अगर (procfs_get_threads_max(&threads_max) < 0)
                        वापस r; /* return original error */

                अगर (MIN(pid_max - 1, threads_max) != limit)
                        वापस r; /* return original error */

                /* Yay! Value set already matches what we were trying to set, hence consider this a success. */
        }

        वापस 0;
}

पूर्णांक procfs_tasks_get_current(uint64_t *ret) {
        _cleanup_free_ अक्षर *मूल्य = शून्य;
        स्थिर अक्षर *p, *nr;
        माप_प्रकार n;
        पूर्णांक r;

        निश्चित(ret);

        r = read_one_पंक्ति_file("/proc/loadavg", &मूल्य);
        अगर (r < 0)
                वापस r;

        /* Look for the second part of the fourth field, which is separated by a slash from the first part. None of the
         * earlier fields use a slash, hence let's use this to find the right spot. */
        p = म_अक्षर(मूल्य, '/');
        अगर (!p)
                वापस -EINVAL;

        p++;
        n = म_अखोज(p, DIGITS);
        nr = strndupa_safe(p, n);

        वापस safe_atou64(nr, ret);
}

अटल uint64_t calc_gcd64(uint64_t a, uint64_t b) {

        जबतक (b > 0) {
                uint64_t t;

                t = a % b;

                a = b;
                b = t;
        }

        वापस a;
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

        वापस ticks / b * a + DIV_ROUND_UP(ticks % b * a, b);
}

पूर्णांक procfs_cpu_get_usage(nsec_t *ret) {
        पूर्णांक r;

        निश्चित(ret);

        ProcfsCpuTicks ticks;
        r = procfs_cpu_get_ticks(&ticks);
        अगर (r < 0)
                वापस r;

        *ret = procfs_ticks_to_nsec(ticks.user + ticks.nice + ticks.प्रणाली + ticks.irq + ticks.softirq);
        वापस 0;
}

पूर्णांक procfs_cpu_get_ticks(ProcfsCpuTicks *ret) {
        _cleanup_free_ अक्षर *first_पंक्ति = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        r = read_one_पंक्ति_file("/proc/stat", &first_पंक्ति);
        अगर (r < 0)
                वापस r;

        स्थिर अक्षर *p = first_word(first_पंक्ति, "cpu");
        अगर (!p)
                वापस -EINVAL;

        /* All eight fields exist on any kernel we support: iowait was added in 2.5.41, steal in 2.6.11.
         * "guest" and "guest_nice" are not returned, the kernel already includes them in "user" and "nice".
         * Nevertheless, this file might be synthesized slopilly, so let's not require the last three fields.
         * We don't care about them in most cases anyway. */
        ProcfsCpuTicks t = {};
        r = माला_पूछो(p, "%"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64" %"SCNu64,
                   &t.user,
                   &t.nice,
                   &t.प्रणाली,
                   &t.idle,
                   &t.iowait,
                   &t.irq,
                   &t.softirq,
                   &t.steal);
        अगर (r < 5)
                वापस -EINVAL;
        अगर (r < 8)
                log_once(LOG_INFO, "Short read (%d fields) from /proc/stat, continuing.", r);

        *ret = t;
        वापस 0;
}

पूर्णांक convert_meminfo_मूल्य_to_uint64_bytes(स्थिर अक्षर *s, uint64_t *ret) {
        _cleanup_free_ अक्षर *w = शून्य;
        uint64_t v;
        पूर्णांक r;

        निश्चित(s);
        निश्चित(ret);

        r = extract_first_word(&s, &w, /* separators= */ शून्य, /* flags= */ 0);
        अगर (r < 0)
                वापस r;
        अगर (r == 0)
                वापस -EINVAL;

        /* Ensure the line ends in "kB" */
        अगर (!streq(s, "kB"))
                वापस -EINVAL;

        r = safe_atou64(w, &v);
        अगर (r < 0)
                वापस r;
        अगर (v == UINT64_MAX)
                वापस -EINVAL;

        अगर (!MUL_ASSIGN_SAFE(&v, U64_KB))
                वापस -EOVERFLOW;

        *ret = v;
        वापस 0;
}

पूर्णांक procfs_memory_get(uint64_t *ret_योग, uint64_t *ret_used) {
        uint64_t mem_योग = UINT64_MAX, mem_available = UINT64_MAX;
        _cleanup_fclose_ खाता *f = शून्य;
        पूर्णांक r;

        f = ख_खोलो("/proc/meminfo", "re");
        अगर (!f)
                वापस -त्रुटि_सं;

        क्रम (;;) {
                _cleanup_free_ अक्षर *पंक्ति = शून्य;
                uint64_t *v;
                अक्षर *p;

                r = read_पंक्ति(f, LONG_पंक्ति_MAX, &पंक्ति);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        वापस -EINVAL; /* EOF: Couldn't find one or both fields? */

                p = first_word(पंक्ति, "MemTotal:");
                अगर (p)
                        v = &mem_योग;
                अन्यथा {
                        p = first_word(पंक्ति, "MemAvailable:");
                        अगर (p)
                                v = &mem_available;
                        अन्यथा
                                जारी;
                }

                r = convert_meminfo_मूल्य_to_uint64_bytes(p, v);
                अगर (r < 0)
                        वापस r;

                अगर (mem_योग != UINT64_MAX && mem_available != UINT64_MAX)
                        अवरोध;
        }

        अगर (mem_available > mem_योग)
                वापस -EINVAL;

        अगर (ret_योग)
                *ret_योग = mem_योग;
        अगर (ret_used)
                *ret_used = mem_योग - mem_available;
        वापस 0;
}

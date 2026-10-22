/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "cgroup-util.h"
#شامل "limits-util.h"
#شامل "log.h"
#شامل "memory-util.h"
#شامل "parse-util.h"
#شامل "process-util.h"
#شامل "procfs-util.h"

uint64_t physical_حافظہ(خالی) {
        _cleanup_free_ حرف *جڑ = تہی, *قدر = تہی;
        uint64_t mem, lim;
        size_t ps;
        طویل sc;
        صحیح r;

        /* We return this as uint64_t in case we are running as 32-bit process on a 64-bit kernel with huge amounts of
         * memory.
         *
         * In order to support containers nicely that have a configured memory limit we'll take the minimum of the
         * physically reported amount of memory and the limit configured for the root cgroup, if there is any. */

        sc = sysconf(_SC_PHYS_PAGES);
        أكد(sc > 0);

        ps = صفحہ_سائز();
        /* Physical page count times page size cannot realistically overflow uint64_t,
         * but use MUL_SAFE to make this obvious to static analyzers. */
        assert_se(MUL_محفوظ(&mem, (uint64_t) sc, (uint64_t) ps));

        r = cg_get_جڑ_راستہ(&جڑ);
        اگر (r < 0) {
                log_debug_errno(r, "Failed to determine root cgroup, ignoring cgroup memory limit: %m");
                واپس mem;
        }

        r = cg_get_attribute(جڑ, "memory.max", &قدر);
        اگر (r == -ENOENT) /* Field does not exist on the system's top-level cgroup, hence don't
                           * complain. (Note that it might exist on our own root though, if we live
                           * in a cgroup namespace, hence check anyway instead of not even
                           * trying.) */
                واپس mem;
        اگر (r < 0) {
                log_debug_errno(r, "Failed to read memory.max cgroup attribute, ignoring cgroup memory limit: %m");
                واپس mem;
        }

        اگر (streq(قدر, "max"))
                واپس mem;

        r = محفوظ_atou64(قدر, &lim);
        اگر (r < 0) {
                log_debug_errno(r, "Failed to parse cgroup memory limit '%s', ignoring: %m", قدر);
                واپس mem;
        }
        اگر (lim == UINT64_MAX)
                واپس mem;

        /* Make sure the limit is a multiple of our own page size */
        lim /= ps;
        lim *= ps;

        واپس MIN(mem, lim);
}

uint64_t physical_حافظہ_scale(uint64_t v, uint64_t max) {
        uint64_t p, m, ps;

        /* Shortcut two special cases */
        اگر (v == 0)
                واپس 0;
        اگر (v == max)
                واپس physical_حافظہ();

        أكد(max > 0);

        /* Returns the physical memory size, multiplied by v divided by max. Returns UINT64_MAX on overflow. On success
         * the result is a multiple of the page size (rounds down). */

        ps = صفحہ_سائز();
        أكد(ps > 0);

        p = physical_حافظہ() / ps;
        أكد(p > 0);

        اگر (v > UINT64_MAX / p)
                واپس UINT64_MAX;

        m = p * v;
        m /= max;

        اگر (m > UINT64_MAX / ps)
                واپس UINT64_MAX;

        واپس m * ps;
}

uint64_t system_tasks_max(خالی) {
        uint64_t a = TASKS_MAX, b = TASKS_MAX, c = TASKS_MAX;
        _cleanup_free_ حرف *جڑ = تہی;
        صحیح r;

        /* Determine the maximum number of tasks that may run on this system. We check three sources to
         * determine this limit:
         *
         * a) kernel.threads-max sysctl: the maximum number of tasks (threads) the kernel allows.
         *
         *    This puts a direct limit on the number of concurrent tasks.
         *
         * b) kernel.pid_max sysctl: the maximum PID value.
         *
         *    This limits the numeric range PIDs can take, and thus indirectly also limits the number of
         *    concurrent threads. It's primarily a compatibility concept: some crappy old code used a signed
         *    16-bit type for PIDs, hence the kernel provides a way to ensure the PIDs never go beyond
         *    INT16_MAX by default.
         *
         *    Also note the weird definition: PIDs assigned will be kept below this value, which means
         *    the number of tasks that can be created is one lower, as PID 0 is not a valid process ID.
         *
         * c) pids.max on the root cgroup: the kernel's configured maximum number of tasks.
         *
         * and then pick the smallest of the three.
         *
         * By default pid_max is set to much lower values than threads-max, hence the limit people come into
         * contact with first, as it's the lowest boundary they need to bump when they want higher number of
         * processes.
         */

        r = procfs_get_دھاگے_max(&a);
        اگر (r < 0)
                log_debug_errno(r, "Failed to read kernel.threads-max, ignoring: %m");

        r = procfs_get_pid_max(&b);
        اگر (r < 0)
                log_debug_errno(r, "Failed to read kernel.pid_max, ignoring: %m");
        ورنہ اگر (b > 0)
                /* Subtract one from pid_max, since PID 0 is not a valid PID */
                b--;

        r = cg_get_جڑ_راستہ(&جڑ);
        اگر (r < 0)
                log_debug_errno(r, "Failed to determine cgroup root path, ignoring: %m");
        ورنہ {
                /* We'll have the "pids.max" attribute on the our root cgroup only if we are in a
                 * CLONE_NEWCGROUP namespace. On the top-level namespace this attribute is missing, hence
                 * suppress any message about that */
                r = cg_get_attribute_as_uint64(جڑ, "pids.max", &c);
                اگر (r < 0 && r != -ENODATA)
                        log_debug_errno(r, "Failed to read pids.max attribute of root cgroup, ignoring: %m");
        }

        واپس MIN3(a, b, c);
}

uint64_t system_tasks_max_scale(uint64_t v, uint64_t max) {
        uint64_t t, m;

        /* Shortcut two special cases */
        اگر (v == 0)
                واپس 0;
        اگر (v == max)
                واپس system_tasks_max();

        أكد(max > 0);

        /* Multiply the system's task value by the fraction v/max. Hence, if max==100 this calculates percentages
         * relative to the system's maximum number of tasks. Returns UINT64_MAX on overflow. */

        t = system_tasks_max();
        أكد(t > 0);

        اگر (v > UINT64_MAX / t) /* overflow? */
                واپس UINT64_MAX;

        m = t * v;
        واپس m / max;
}

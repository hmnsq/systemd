/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "cgroup-util.h"
#ضمن "limits-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "parse-util.h"
#ضمن "process-util.h"
#ضمن "procfs-util.h"

uint64_t physical_memory(فراغ) {
        _cleanup_free_ حرف *اکر = عدم, *نيلاي = عدم;
        uint64_t mem, lim;
        size_t ps;
        long sc;
        صحيح r;

        /* We return this as uint64_t in case we are running as 32-bit process on a 64-bit kernel with huge amounts of
         * memory.
         *
         * In order to support containers nicely that have a configured memory limit we'll take the minimum of the
         * physically reported amount of memory and the limit configured for the root cgroup, if there is any. */

        sc = sysconf(_SC_PHYS_PAGES);
        أكد(sc > 0);

        ps = page_size();
        /* Physical page count times page size cannot realistically overflow uint64_t,
         * but use MUL_SAFE to make this obvious to static analyzers. */
        assert_se(MUL_SAFE(&mem, (uint64_t) sc, (uint64_t) ps));

        r = cg_get_اکر_path(&اکر);
        إذا (r < 0) {
                log_debug_errno(r, "Failed to determine root cgroup, ignoring cgroup memory limit: %m");
                أرجع mem;
        }

        r = cg_get_attribute(اکر, "memory.max", &نيلاي);
        إذا (r == -ENOENT) /* Field does not exist on the system's top-level cgroup, hence don't
                           * complain. (Note that it might exist on our own root though, if we live
                           * in a cgroup namespace, hence check anyway instead of not even
                           * trying.) */
                أرجع mem;
        إذا (r < 0) {
                log_debug_errno(r, "Failed to read memory.max cgroup attribute, ignoring cgroup memory limit: %m");
                أرجع mem;
        }

        إذا (streq(نيلاي, "max"))
                أرجع mem;

        r = safe_atou64(نيلاي, &lim);
        إذا (r < 0) {
                log_debug_errno(r, "Failed to parse cgroup memory limit '%s', ignoring: %m", نيلاي);
                أرجع mem;
        }
        إذا (lim == UINT64_MAX)
                أرجع mem;

        /* Make sure the limit is a multiple of our own page size */
        lim /= ps;
        lim *= ps;

        أرجع MIN(mem, lim);
}

uint64_t physical_memory_scale(uint64_t v, uint64_t max) {
        uint64_t p, m, ps;

        /* Shortcut two special cases */
        إذا (v == 0)
                أرجع 0;
        إذا (v == max)
                أرجع physical_memory();

        أكد(max > 0);

        /* Returns the physical memory size, multiplied by v divided by max. Returns UINT64_MAX on overflow. On success
         * the result is a multiple of the page size (rounds down). */

        ps = page_size();
        أكد(ps > 0);

        p = physical_memory() / ps;
        أكد(p > 0);

        إذا (v > UINT64_MAX / p)
                أرجع UINT64_MAX;

        m = p * v;
        m /= max;

        إذا (m > UINT64_MAX / ps)
                أرجع UINT64_MAX;

        أرجع m * ps;
}

uint64_t system_tasks_max(فراغ) {
        uint64_t a = TASKS_MAX, b = TASKS_MAX, c = TASKS_MAX;
        _cleanup_free_ حرف *اکر = عدم;
        صحيح r;

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

        r = procfs_get_threads_max(&a);
        إذا (r < 0)
                log_debug_errno(r, "Failed to read kernel.threads-max, ignoring: %m");

        r = procfs_get_pid_max(&b);
        إذا (r < 0)
                log_debug_errno(r, "Failed to read kernel.pid_max, ignoring: %m");
        وإلا إذا (b > 0)
                /* Subtract one from pid_max, since PID 0 is not a valid PID */
                b--;

        r = cg_get_اکر_path(&اکر);
        إذا (r < 0)
                log_debug_errno(r, "Failed to determine cgroup root path, ignoring: %m");
        وإلا {
                /* We'll have the "pids.max" attribute on the our root cgroup only if we are in a
                 * CLONE_NEWCGROUP namespace. On the top-level namespace this attribute is missing, hence
                 * suppress any message about that */
                r = cg_get_attribute_as_uint64(اکر, "pids.max", &c);
                إذا (r < 0 && r != -ENODATA)
                        log_debug_errno(r, "Failed to read pids.max attribute of root cgroup, ignoring: %m");
        }

        أرجع MIN3(a, b, c);
}

uint64_t system_tasks_max_scale(uint64_t v, uint64_t max) {
        uint64_t t, m;

        /* Shortcut two special cases */
        إذا (v == 0)
                أرجع 0;
        إذا (v == max)
                أرجع system_tasks_max();

        أكد(max > 0);

        /* Multiply the system's task value by the fraction v/max. Hence, if max==100 this calculates percentages
         * relative to the system's maximum number of tasks. Returns UINT64_MAX on overflow. */

        t = system_tasks_max();
        أكد(t > 0);

        إذا (v > UINT64_MAX / t) /* overflow? */
                أرجع UINT64_MAX;

        m = t * v;
        أرجع m / max;
}

/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "cgroup-util.h"
#അന്തര്ഭാവ "limits-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "procfs-util.h"

uint64_t physical_memory(ശൂന്യം) {
        _cleanup_free_ അക്ഷരം *വേര് = ശൂന്യ, *മൂല്യം = ശൂന്യ;
        uint64_t mem, lim;
        size_t ps;
        long sc;
        പൂർണ്ണസംഖ്യ r;

        /* We return this as uint64_t in case we are running as 32-bit process on a 64-bit kernel with huge amounts of
         * memory.
         *
         * In order to support containers nicely that have a configured memory limit we'll take the minimum of the
         * physically reported amount of memory and the limit configured for the root cgroup, if there is any. */

        sc = sysconf(_SC_PHYS_PAGES);
        പ്രതിജ്ഞാ(sc > 0);

        ps = page_size();
        /* Physical page count times page size cannot realistically overflow uint64_t,
         * but use MUL_SAFE to make this obvious to static analyzers. */
        assert_se(MUL_SAFE(&mem, (uint64_t) sc, (uint64_t) ps));

        r = cg_get_വേര്_path(&വേര്);
        എങ്കിൽ (r < 0) {
                log_debug_errno(r, "Failed to determine root cgroup, ignoring cgroup memory limit: %m");
                തിരികെ mem;
        }

        r = cg_get_attribute(വേര്, "memory.max", &മൂല്യം);
        എങ്കിൽ (r == -ENOENT) /* Field does not exist on the system's top-level cgroup, hence don't
                           * complain. (Note that it might exist on our own root though, if we live
                           * in a cgroup namespace, hence check anyway instead of not even
                           * trying.) */
                തിരികെ mem;
        എങ്കിൽ (r < 0) {
                log_debug_errno(r, "Failed to read memory.max cgroup attribute, ignoring cgroup memory limit: %m");
                തിരികെ mem;
        }

        എങ്കിൽ (streq(മൂല്യം, "max"))
                തിരികെ mem;

        r = safe_atou64(മൂല്യം, &lim);
        എങ്കിൽ (r < 0) {
                log_debug_errno(r, "Failed to parse cgroup memory limit '%s', ignoring: %m", മൂല്യം);
                തിരികെ mem;
        }
        എങ്കിൽ (lim == UINT64_MAX)
                തിരികെ mem;

        /* Make sure the limit is a multiple of our own page size */
        lim /= ps;
        lim *= ps;

        തിരികെ MIN(mem, lim);
}

uint64_t physical_memory_scale(uint64_t v, uint64_t max) {
        uint64_t p, m, ps;

        /* Shortcut two special cases */
        എങ്കിൽ (v == 0)
                തിരികെ 0;
        എങ്കിൽ (v == max)
                തിരികെ physical_memory();

        പ്രതിജ്ഞാ(max > 0);

        /* Returns the physical memory size, multiplied by v divided by max. Returns UINT64_MAX on overflow. On success
         * the result is a multiple of the page size (rounds down). */

        ps = page_size();
        പ്രതിജ്ഞാ(ps > 0);

        p = physical_memory() / ps;
        പ്രതിജ്ഞാ(p > 0);

        എങ്കിൽ (v > UINT64_MAX / p)
                തിരികെ UINT64_MAX;

        m = p * v;
        m /= max;

        എങ്കിൽ (m > UINT64_MAX / ps)
                തിരികെ UINT64_MAX;

        തിരികെ m * ps;
}

uint64_t system_tasks_max(ശൂന്യം) {
        uint64_t a = TASKS_MAX, b = TASKS_MAX, c = TASKS_MAX;
        _cleanup_free_ അക്ഷരം *വേര് = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

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
        എങ്കിൽ (r < 0)
                log_debug_errno(r, "Failed to read kernel.threads-max, ignoring: %m");

        r = procfs_get_pid_max(&b);
        എങ്കിൽ (r < 0)
                log_debug_errno(r, "Failed to read kernel.pid_max, ignoring: %m");
        അല്ലെങ്കിൽ എങ്കിൽ (b > 0)
                /* Subtract one from pid_max, since PID 0 is not a valid PID */
                b--;

        r = cg_get_വേര്_path(&വേര്);
        എങ്കിൽ (r < 0)
                log_debug_errno(r, "Failed to determine cgroup root path, ignoring: %m");
        അല്ലെങ്കിൽ {
                /* We'll have the "pids.max" attribute on the our root cgroup only if we are in a
                 * CLONE_NEWCGROUP namespace. On the top-level namespace this attribute is missing, hence
                 * suppress any message about that */
                r = cg_get_attribute_as_uint64(വേര്, "pids.max", &c);
                എങ്കിൽ (r < 0 && r != -ENODATA)
                        log_debug_errno(r, "Failed to read pids.max attribute of root cgroup, ignoring: %m");
        }

        തിരികെ MIN3(a, b, c);
}

uint64_t system_tasks_max_scale(uint64_t v, uint64_t max) {
        uint64_t t, m;

        /* Shortcut two special cases */
        എങ്കിൽ (v == 0)
                തിരികെ 0;
        എങ്കിൽ (v == max)
                തിരികെ system_tasks_max();

        പ്രതിജ്ഞാ(max > 0);

        /* Multiply the system's task value by the fraction v/max. Hence, if max==100 this calculates percentages
         * relative to the system's maximum number of tasks. Returns UINT64_MAX on overflow. */

        t = system_tasks_max();
        പ്രതിജ്ഞാ(t > 0);

        എങ്കിൽ (v > UINT64_MAX / t) /* overflow? */
                തിരികെ UINT64_MAX;

        m = t * v;
        തിരികെ m / max;
}

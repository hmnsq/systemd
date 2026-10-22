/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <sched.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <sys/mman.h>
#অন্তর্ভাব <sys/prctl.h> /* IWYU pragma: keep */

#অন্তর্ভাব "argv-util.h"
#অন্তর্ভাব "capability-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "string-util.h"

পূর্ণসংখ্যা saved_argc = 0;
অক্ষর **saved_argv = ফাঁকা;

শূন্য save_argc_argv(পূর্ণসংখ্যা argc, অক্ষর **argv) {
        /* Protect against CVE-2021-4034 style attacks */
        assert_se(argc > 0);
        assert_se(argv);
        assert_se(argv[0]);

        saved_argc = argc;
        saved_argv = argv;
}

bool invoked_as(অক্ষর *argv[], ধ্রুবক অক্ষর *token) {
        ধ্রুবক অক্ষর *progname = empty_to_null(secure_getenv("SYSTEMD_INVOKED_AS"));

        যদি (!progname && argv)
                progname = argv[0];

        যদি (isempty(progname) || isempty(token))
                ফেরত false;

        ফেরত strstr(last_path_component(progname), token);
}

bool invoked_by_systemd(শূন্য) {
        স্থির পূর্ণসংখ্যা cached = -1;
        পূর্ণসংখ্যা r;

        যদি (cached >= 0)
                ফেরত cached;

        /* If the process is directly executed by PID1 (e.g. ExecStart= or generator), systemd-importd,
         * or systemd-homed, then $SYSTEMD_EXEC_PID= is set, and read the command line. */
        ধ্রুবক অক্ষর *e = getenv("SYSTEMD_EXEC_PID");
        যদি (!e)
                ফেরত (cached = false);

        যদি (streq(e, "*"))
                /* For testing. */
                ফেরত (cached = true);

        pid_t p;
        r = parse_pid(e, &p);
        যদি (r < 0) {
                /* We know that systemd sets the variable correctly. Something else must have set it. */
                log_debug_errno(r, "Failed to parse \"SYSTEMD_EXEC_PID=%s\", ignoring: %m", e);
                ফেরত (cached = false);
        }

        cached = getpid_cached() == p;
        ফেরত cached;
}

স্থির পূর্ণসংখ্যা update_argv(ধ্রুবক অক্ষর name[], size_t l) {
        স্থির পূর্ণসংখ্যা can_do = -1;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(l < SIZE_MAX);

        যদি (can_do == 0)
                ফেরত 0;
        can_do = false; /* We'll set it to true only if the whole process works */

        /* Calling prctl() with PR_SET_MM_ARG_{START,END} requires CAP_SYS_RESOURCE so let's use this as quick bypass
         * check, to avoid calling mmap() should PR_SET_MM_ARG_{START,END} fail with EPERM later on anyway. */
        r = have_effective_cap(CAP_SYS_RESOURCE);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to check if we have enough privileges: %m");
        যদি (r == 0)
                ফেরত log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Skipping PR_SET_MM, as we don't have privileges.");

        স্থির size_t mm_আকার = 0;
        স্থির অক্ষর *mm = ফাঁকা;

        যদি (mm_আকার < l+1) {
                size_t nn_আকার;
                অক্ষর *nn;

                nn_আকার = PAGE_ALIGN(l+1);
                যদি (nn_আকার >= SIZE_MAX)
                        ফেরত log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The requested argument is too long.");

                nn = mmap(ফাঁকা, nn_আকার, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
                যদি (nn == MAP_FAILED)
                        ফেরত log_debug_errno(errno, "mmap() failed: %m");

                strncpy(nn, name, nn_আকার);

                /* Now, let's tell the kernel about this new memory */
                r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_START, (unsigned long) nn, 0, 0);
                যদি (ERRNO_IS_NEG_PRIVILEGE(r))
                        ফেরত log_debug_errno(r, "PR_SET_MM_ARG_START failed: %m");
                যদি (r < 0) {
                        /* HACK: prctl() API is kind of dumb on this point.  The existing end address may already be
                         * below the desired start address, in which case the kernel may have kicked this back due
                         * to a range-check failure (see linux/kernel/sys.c:validate_prctl_map() to see this in
                         * action).  The proper solution would be to have a prctl() API that could set both start+end
                         * simultaneously, or at least let us query the existing address to anticipate this condition
                         * and respond accordingly.  For now, we can only guess at the cause of this failure and try
                         * a workaround--which will briefly expand the arg space to something potentially huge before
                         * resizing it to what we want. */
                        log_debug_errno(r, "PR_SET_MM_ARG_START failed, attempting PR_SET_MM_ARG_END hack: %m");

                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (unsigned long) nn + l + 1, 0, 0);
                        যদি (r < 0) {
                                log_debug_errno(r, "PR_SET_MM_ARG_END hack failed, proceeding without: %m");
                                munmap_safe(nn, nn_আকার);
                                ফেরত r;
                        }

                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_START, (unsigned long) nn, 0, 0);
                        যদি (r < 0)
                                ফেরত log_debug_errno(r, "PR_SET_MM_ARG_START still failed, proceeding without: %m");
                } অন্যথা {
                        /* And update the end pointer to the new end, too. If this fails, we don't really know what
                         * to do, it's pretty unlikely that we can rollback, hence we'll just accept the failure,
                         * and continue. */
                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (unsigned long) nn + l + 1, 0, 0);
                        যদি (r < 0)
                                log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
                }

                munmap_safe(mm, mm_আকার);

                mm = nn;
                mm_আকার = nn_আকার;
        } অন্যথা {
                strncpy(mm, name, mm_আকার);

                /* Update the end pointer, continuing regardless of any failure. */
                r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (unsigned long) mm + l + 1, 0, 0);
                যদি (r < 0)
                        log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
        }

        can_do = true;
        ফেরত 0;
}

পূর্ণসংখ্যা rename_process_full(ধ্রুবক অক্ষর *comm, ধ্রুবক অক্ষর *invocation) {
        bool truncated = false;
        পূর্ণসংখ্যা r;

        /* This is a like a poor man's setproctitle(). It changes the comm field by the name specified by
         * 'comm', and changes argv[0] and the glibc's internally used names of the process
         * (program_invocation_name and program_invocation_short_name) by the name specified by 'invocation'.
         * For the first one a limit of 16 chars applies; to the second one in many cases one of 10 (i.e.
         * length of "/sbin/init") — however if we have CAP_SYS_RESOURCES it is unbounded; to the third one
         * 7 (i.e. the length of "systemd". If you pass a longer string it will likely be truncated.
         *
         * Returns 0 if a name was set but truncated, > 0 if it was set but not truncated. */

        যদি (isempty(comm))
                ফেরত -EINVAL; /* let's not confuse users unnecessarily with an empty name */

        যদি (!is_main_thread())
                ফেরত -EPERM; /* Let's not allow setting the process name from other threads than the main one, as we
                                * cache things without locking, and we make assumptions that PR_SET_NAME sets the
                                * process name that isn't correct on any other threads */

        size_t l = strlen(comm);

        /* First step, change the comm field. The main thread's comm is identical to the process comm. This means we
         * can use PR_SET_NAME, which sets the thread name for the calling thread. */
        r = proc_set_comm(comm);
        যদি (r < 0)
                log_debug_errno(r, "PR_SET_NAME failed: %m");
        যদি (l >= TASK_COMM_LEN) /* Linux userspace process names can be 15 chars at max */
                truncated = true;

        /* If nothing specified, fall back to comm. */
        যদি (isempty(invocation))
                invocation = comm;

        l = strlen(invocation);

        /* Second step, change glibc's ID of the process name. */
        যদি (program_invocation_name) {
                size_t k;

                k = strlen(program_invocation_name);
                strncpy(program_invocation_name, invocation, k);
                যদি (l > k)
                        truncated = true;

                /* Also update the short name. */
                অক্ষর *p = strrchr(program_invocation_name, '/');
                program_invocation_short_name = p ? p + 1 : program_invocation_name;
        }

        /* Third step, completely replace the argv[] array the kernel maintains for us. This requires privileges, but
         * has the advantage that the argv[] array is exactly what we want it to be, and not filled up with zeros at
         * the end. This is the best option for changing /proc/self/cmdline. */
        (শূন্য) update_argv(invocation, l);

        /* Fourth step: in all cases we'll also update the original argv[], so that our own code gets it right too if
         * it still looks here */
        যদি (saved_argc > 0) {
                যদি (saved_argv[0]) {
                        size_t k;

                        k = strlen(saved_argv[0]);
                        strncpy(saved_argv[0], invocation, k);
                        যদি (l > k)
                                truncated = true;
                }

                জন্য (পূর্ণসংখ্যা i = 1; i < saved_argc; i++) {
                        যদি (!saved_argv[i])
                                বিরতি;

                        memzero(saved_argv[i], strlen(saved_argv[i]));
                }
        }

        ফেরত !truncated;
}

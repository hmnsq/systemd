/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <sched.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <sys/mman.h>
#ಅನ್ತರ್ಭಾವ <sys/prctl.h> /* IWYU pragma: keep */

#ಅನ್ತರ್ಭಾವ "argv-util.h"
#ಅನ್ತರ್ಭಾವ "capability-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಪೂರ್ಣಾಙ್ಕ saved_argc = 0;
ವರ್ಣ **saved_argv = ರಿಕ್ತ;

ಶೂನ್ಯ save_argc_argv(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv) {
        /* Protect against CVE-2021-4034 style attacks */
        assert_se(argc > 0);
        assert_se(argv);
        assert_se(argv[0]);

        saved_argc = argc;
        saved_argv = argv;
}

bool invoked_as(ವರ್ಣ *argv[], ಧ್ರುವ ವರ್ಣ *token) {
        ಧ್ರುವ ವರ್ಣ *progname = empty_to_null(secure_getenv("SYSTEMD_INVOKED_AS"));

        ಯದಿ (!progname && argv)
                progname = argv[0];

        ಯದಿ (isempty(progname) || isempty(token))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ strstr(last_path_component(progname), token);
}

bool invoked_by_systemd(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cached = -1;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (cached >= 0)
                ಪ್ರತ್ಯಾಗಮ cached;

        /* If the process is directly executed by PID1 (e.g. ExecStart= or generator), systemd-importd,
         * or systemd-homed, then $SYSTEMD_EXEC_PID= is set, and read the command line. */
        ಧ್ರುವ ವರ್ಣ *e = getenv("SYSTEMD_EXEC_PID");
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ (cached = false);

        ಯದಿ (streq(e, "*"))
                /* For testing. */
                ಪ್ರತ್ಯಾಗಮ (cached = true);

        pid_t p;
        r = parse_pid(e, &p);
        ಯದಿ (r < 0) {
                /* We know that systemd sets the variable correctly. Something else must have set it. */
                log_debug_errno(r, "Failed to parse \"SYSTEMD_EXEC_PID=%s\", ignoring: %m", e);
                ಪ್ರತ್ಯಾಗಮ (cached = false);
        }

        cached = getpid_cached() == p;
        ಪ್ರತ್ಯಾಗಮ cached;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ update_argv(ಧ್ರುವ ವರ್ಣ name[], size_t l) {
        ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ can_do = -1;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(l < SIZE_MAX);

        ಯದಿ (can_do == 0)
                ಪ್ರತ್ಯಾಗಮ 0;
        can_do = false; /* We'll set it to true only if the whole process works */

        /* Calling prctl() with PR_SET_MM_ARG_{START,END} requires CAP_SYS_RESOURCE so let's use this as quick bypass
         * check, to avoid calling mmap() should PR_SET_MM_ARG_{START,END} fail with EPERM later on anyway. */
        r = have_effective_cap(CAP_SYS_RESOURCE);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to check if we have enough privileges: %m");
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Skipping PR_SET_MM, as we don't have privileges.");

        ಸ್ಥಿರ size_t mm_size = 0;
        ಸ್ಥಿರ ವರ್ಣ *mm = ರಿಕ್ತ;

        ಯದಿ (mm_size < l+1) {
                size_t nn_size;
                ವರ್ಣ *nn;

                nn_size = PAGE_ALIGN(l+1);
                ಯದಿ (nn_size >= SIZE_MAX)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The requested argument is too long.");

                nn = mmap(ರಿಕ್ತ, nn_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
                ಯದಿ (nn == MAP_FAILED)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "mmap() failed: %m");

                strncpy(nn, name, nn_size);

                /* Now, let's tell the kernel about this new memory */
                r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_START, (unsigned long) nn, 0, 0);
                ಯದಿ (ERRNO_IS_NEG_PRIVILEGE(r))
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "PR_SET_MM_ARG_START failed: %m");
                ಯದಿ (r < 0) {
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
                        ಯದಿ (r < 0) {
                                log_debug_errno(r, "PR_SET_MM_ARG_END hack failed, proceeding without: %m");
                                munmap_safe(nn, nn_size);
                                ಪ್ರತ್ಯಾಗಮ r;
                        }

                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_START, (unsigned long) nn, 0, 0);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "PR_SET_MM_ARG_START still failed, proceeding without: %m");
                } ಅನ್ಯಥಾ {
                        /* And update the end pointer to the new end, too. If this fails, we don't really know what
                         * to do, it's pretty unlikely that we can rollback, hence we'll just accept the failure,
                         * and continue. */
                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (unsigned long) nn + l + 1, 0, 0);
                        ಯದಿ (r < 0)
                                log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
                }

                munmap_safe(mm, mm_size);

                mm = nn;
                mm_size = nn_size;
        } ಅನ್ಯಥಾ {
                strncpy(mm, name, mm_size);

                /* Update the end pointer, continuing regardless of any failure. */
                r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (unsigned long) mm + l + 1, 0, 0);
                ಯದಿ (r < 0)
                        log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
        }

        can_do = true;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ rename_process_full(ಧ್ರುವ ವರ್ಣ *comm, ಧ್ರುವ ವರ್ಣ *invocation) {
        bool truncated = false;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* This is a like a poor man's setproctitle(). It changes the comm field by the name specified by
         * 'comm', and changes argv[0] and the glibc's internally used names of the process
         * (program_invocation_name and program_invocation_short_name) by the name specified by 'invocation'.
         * For the first one a limit of 16 chars applies; to the second one in many cases one of 10 (i.e.
         * length of "/sbin/init") — however if we have CAP_SYS_RESOURCES it is unbounded; to the third one
         * 7 (i.e. the length of "systemd". If you pass a longer string it will likely be truncated.
         *
         * Returns 0 if a name was set but truncated, > 0 if it was set but not truncated. */

        ಯದಿ (isempty(comm))
                ಪ್ರತ್ಯಾಗಮ -EINVAL; /* let's not confuse users unnecessarily with an empty name */

        ಯದಿ (!is_main_thread())
                ಪ್ರತ್ಯಾಗಮ -EPERM; /* Let's not allow setting the process name from other threads than the main one, as we
                                * cache things without locking, and we make assumptions that PR_SET_NAME sets the
                                * process name that isn't correct on any other threads */

        size_t l = strlen(comm);

        /* First step, change the comm field. The main thread's comm is identical to the process comm. This means we
         * can use PR_SET_NAME, which sets the thread name for the calling thread. */
        r = proc_set_comm(comm);
        ಯದಿ (r < 0)
                log_debug_errno(r, "PR_SET_NAME failed: %m");
        ಯದಿ (l >= TASK_COMM_LEN) /* Linux userspace process names can be 15 chars at max */
                truncated = true;

        /* If nothing specified, fall back to comm. */
        ಯದಿ (isempty(invocation))
                invocation = comm;

        l = strlen(invocation);

        /* Second step, change glibc's ID of the process name. */
        ಯದಿ (program_invocation_name) {
                size_t k;

                k = strlen(program_invocation_name);
                strncpy(program_invocation_name, invocation, k);
                ಯದಿ (l > k)
                        truncated = true;

                /* Also update the short name. */
                ವರ್ಣ *p = strrchr(program_invocation_name, '/');
                program_invocation_short_name = p ? p + 1 : program_invocation_name;
        }

        /* Third step, completely replace the argv[] array the kernel maintains for us. This requires privileges, but
         * has the advantage that the argv[] array is exactly what we want it to be, and not filled up with zeros at
         * the end. This is the best option for changing /proc/self/cmdline. */
        (ಶೂನ್ಯ) update_argv(invocation, l);

        /* Fourth step: in all cases we'll also update the original argv[], so that our own code gets it right too if
         * it still looks here */
        ಯದಿ (saved_argc > 0) {
                ಯದಿ (saved_argv[0]) {
                        size_t k;

                        k = strlen(saved_argv[0]);
                        strncpy(saved_argv[0], invocation, k);
                        ಯದಿ (l > k)
                                truncated = true;
                }

                ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < saved_argc; i++) {
                        ಯದಿ (!saved_argv[i])
                                ಭಙ್ಗ;

                        memzero(saved_argv[i], strlen(saved_argv[i]));
                }
        }

        ಪ್ರತ್ಯಾಗಮ !truncated;
}

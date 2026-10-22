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

ಪೂರ್ಣಾಂಕ saved_argc = 0;
ಅಕ್ಷರ **saved_argv = ಖಾಲಿ;

ಶೂನ್ಯ save_argc_argv(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv) {
        /* Protect against CVE-2021-4034 style attacks */
        assert_se(argc > 0);
        assert_se(argv);
        assert_se(argv[0]);

        saved_argc = argc;
        saved_argv = argv;
}

bool invoked_as(ಅಕ್ಷರ *argv[], ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *token) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *progname = ಖಾಲಿಗಳು_ಗಮ್ಯ_null(secure_getenv("SYSTEMD_INVOKED_AS"));

        ಆದರೆ (!progname && argv)
                progname = argv[0];

        ಆದರೆ (isempty(progname) || isempty(token))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(ಕೊನೆಯ_ಮಾರ್ಗ_component(progname), token);
}

bool invoked_by_systemd(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cached = -1;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (cached >= 0)
                ಹಿಂತಿರುಗಿ cached;

        /* If the process is directly executed by PID1 (e.g. ExecStart= or generator), systemd-importd,
         * or systemd-homed, then $SYSTEMD_EXEC_PID= is set, and read the command line. */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = getenv("SYSTEMD_EXEC_PID");
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ (cached = false);

        ಆದರೆ (streq(e, "*"))
                /* For testing. */
                ಹಿಂತಿರುಗಿ (cached = true);

        pid_t p;
        r = parse_pid(e, &p);
        ಆದರೆ (r < 0) {
                /* We know that systemd sets the variable correctly. Something else must have set it. */
                log_debug_errno(r, "Failed to parse \"SYSTEMD_EXEC_PID=%s\", ignoring: %m", e);
                ಹಿಂತಿರುಗಿ (cached = false);
        }

        cached = getpid_cached() == p;
        ಹಿಂತಿರುಗಿ cached;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ update_argv(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ name[], size_t l) {
        ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ can_do = -1;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(l < SIZE_MAX);

        ಆದರೆ (can_do == 0)
                ಹಿಂತಿರುಗಿ 0;
        can_do = false; /* We'll set it to true only if the whole process works */

        /* Calling prctl() with PR_SET_MM_ARG_{START,END} requires CAP_SYS_RESOURCE so let's use this as quick bypass
         * check, to avoid calling mmap() should PR_SET_MM_ARG_{START,END} fail with EPERM later on anyway. */
        r = have_effective_cap(CAP_SYS_RESOURCE);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to check if we have enough privileges: %m");
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Skipping PR_SET_MM, as we don't have privileges.");

        ಸ್ಥಿರ size_t mm_ಗಾತ್ರ = 0;
        ಸ್ಥಿರ ಅಕ್ಷರ *mm = ಖಾಲಿ;

        ಆದರೆ (mm_ಗಾತ್ರ < l+1) {
                size_t nn_ಗಾತ್ರ;
                ಅಕ್ಷರ *nn;

                nn_ಗಾತ್ರ = ಪುಟ_ALIGN(l+1);
                ಆದರೆ (nn_ಗಾತ್ರ >= SIZE_MAX)
                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The requested argument is too long.");

                nn = mmap(ಖಾಲಿ, nn_ಗಾತ್ರ, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
                ಆದರೆ (nn == MAP_FAILED)
                        ಹಿಂತಿರುಗಿ log_debug_errno(errno, "mmap() failed: %m");

                strncpy(nn, name, nn_ಗಾತ್ರ);

                /* Now, let's tell the kernel about this new memory */
                r = prctl_ಸುರಕ್ಷಿತ(PR_SET_MM, PR_SET_MM_ARG_START, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) nn, 0, 0);
                ಆದರೆ (ERRNO_IS_NEG_PRIVILEGE(r))
                        ಹಿಂತಿರುಗಿ log_debug_errno(r, "PR_SET_MM_ARG_START failed: %m");
                ಆದರೆ (r < 0) {
                        /* HACK: prctl() API is kind of dumb on this point.  The existing end address may already be
                         * below the desired start address, in which case the kernel may have kicked this back due
                         * to a range-check failure (see linux/kernel/sys.c:validate_prctl_map() to see this in
                         * action).  The proper solution would be to have a prctl() API that could set both start+end
                         * simultaneously, or at least let us query the existing address to anticipate this condition
                         * and respond accordingly.  For now, we can only guess at the cause of this failure and try
                         * a workaround--which will briefly expand the arg space to something potentially huge before
                         * resizing it to what we want. */
                        log_debug_errno(r, "PR_SET_MM_ARG_START failed, attempting PR_SET_MM_ARG_END hack: %m");

                        r = prctl_ಸುರಕ್ಷಿತ(PR_SET_MM, PR_SET_MM_ARG_END, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) nn + l + 1, 0, 0);
                        ಆದರೆ (r < 0) {
                                log_debug_errno(r, "PR_SET_MM_ARG_END hack failed, proceeding without: %m");
                                munmap_ಸುರಕ್ಷಿತ(nn, nn_ಗಾತ್ರ);
                                ಹಿಂತಿರುಗಿ r;
                        }

                        r = prctl_ಸುರಕ್ಷಿತ(PR_SET_MM, PR_SET_MM_ARG_START, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) nn, 0, 0);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ log_debug_errno(r, "PR_SET_MM_ARG_START still failed, proceeding without: %m");
                } ಇಲ್ಲದಿದ್ದರೆ {
                        /* And update the end pointer to the new end, too. If this fails, we don't really know what
                         * to do, it's pretty unlikely that we can rollback, hence we'll just accept the failure,
                         * and continue. */
                        r = prctl_ಸುರಕ್ಷಿತ(PR_SET_MM, PR_SET_MM_ARG_END, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) nn + l + 1, 0, 0);
                        ಆದರೆ (r < 0)
                                log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
                }

                munmap_ಸುರಕ್ಷಿತ(mm, mm_ಗಾತ್ರ);

                mm = nn;
                mm_ಗಾತ್ರ = nn_ಗಾತ್ರ;
        } ಇಲ್ಲದಿದ್ದರೆ {
                strncpy(mm, name, mm_ಗಾತ್ರ);

                /* Update the end pointer, continuing regardless of any failure. */
                r = prctl_ಸುರಕ್ಷಿತ(PR_SET_MM, PR_SET_MM_ARG_END, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) mm + l + 1, 0, 0);
                ಆದರೆ (r < 0)
                        log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
        }

        can_do = true;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ rename_process_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *comm, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *invocation) {
        bool truncated = false;
        ಪೂರ್ಣಾಂಕ r;

        /* This is a like a poor man's setproctitle(). It changes the comm field by the name specified by
         * 'comm', and changes argv[0] and the glibc's internally used names of the process
         * (program_invocation_name and program_invocation_short_name) by the name specified by 'invocation'.
         * For the first one a limit of 16 chars applies; to the second one in many cases one of 10 (i.e.
         * length of "/sbin/init") — however if we have CAP_SYS_RESOURCES it is unbounded; to the third one
         * 7 (i.e. the length of "systemd". If you pass a longer string it will likely be truncated.
         *
         * Returns 0 if a name was set but truncated, > 0 if it was set but not truncated. */

        ಆದರೆ (isempty(comm))
                ಹಿಂತಿರುಗಿ -EINVAL; /* let's not confuse users unnecessarily with an empty name */

        ಆದರೆ (!is_main_thread())
                ಹಿಂತಿರುಗಿ -EPERM; /* Let's not allow setting the process name from other threads than the main one, as we
                                * cache things without locking, and we make assumptions that PR_SET_NAME sets the
                                * process name that isn't correct on any other threads */

        size_t l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(comm);

        /* First step, change the comm field. The main thread's comm is identical to the process comm. This means we
         * can use PR_SET_NAME, which sets the thread name for the calling thread. */
        r = proc_ಗಣ_comm(comm);
        ಆದರೆ (r < 0)
                log_debug_errno(r, "PR_SET_NAME failed: %m");
        ಆದರೆ (l >= TASK_COMM_LEN) /* Linux userspace process names can be 15 chars at max */
                truncated = true;

        /* If nothing specified, fall back to comm. */
        ಆದರೆ (isempty(invocation))
                invocation = comm;

        l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(invocation);

        /* Second step, change glibc's ID of the process name. */
        ಆದರೆ (ಕಾರ್ಯಕ್ರಮ_invocation_name) {
                size_t k;

                k = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಕಾರ್ಯಕ್ರಮ_invocation_name);
                strncpy(ಕಾರ್ಯಕ್ರಮ_invocation_name, invocation, k);
                ಆದರೆ (l > k)
                        truncated = true;

                /* Also update the short name. */
                ಅಕ್ಷರ *p = strrchr(ಕಾರ್ಯಕ್ರಮ_invocation_name, '/');
                ಕಾರ್ಯಕ್ರಮ_invocation_short_name = p ? p + 1 : ಕಾರ್ಯಕ್ರಮ_invocation_name;
        }

        /* Third step, completely replace the argv[] array the kernel maintains for us. This requires privileges, but
         * has the advantage that the argv[] array is exactly what we want it to be, and not filled up with zeros at
         * the end. This is the best option for changing /proc/self/cmdline. */
        (ಶೂನ್ಯ) update_argv(invocation, l);

        /* Fourth step: in all cases we'll also update the original argv[], so that our own code gets it right too if
         * it still looks here */
        ಆದರೆ (saved_argc > 0) {
                ಆದರೆ (saved_argv[0]) {
                        size_t k;

                        k = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(saved_argv[0]);
                        strncpy(saved_argv[0], invocation, k);
                        ಆದರೆ (l > k)
                                truncated = true;
                }

                ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < saved_argc; i++) {
                        ಆದರೆ (!saved_argv[i])
                                ನಿಲ್ಲಿಸು;

                        memzero(saved_argv[i], ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(saved_argv[i]));
                }
        }

        ಹಿಂತಿರುಗಿ !truncated;
}

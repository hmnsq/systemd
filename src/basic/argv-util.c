/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <sched.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <sys/mman.h>
#అన్తర్భావ <sys/prctl.h> /* IWYU pragma: keep */

#అన్తర్భావ "argv-util.h"
#అన్తర్భావ "capability-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "string-util.h"

పూర్ణాంకం saved_argc = 0;
అక్షరం **saved_argv = ఖాళీ;

శూన్య save_argc_argv(పూర్ణాంకం argc, అక్షరం **argv) {
        /* Protect against CVE-2021-4034 style attacks */
        assert_se(argc > 0);
        assert_se(argv);
        assert_se(argv[0]);

        saved_argc = argc;
        saved_argv = argv;
}

bool invoked_as(అక్షరం *argv[], స్థిరరాశి అక్షరం *token) {
        స్థిరరాశి అక్షరం *progname = ఖాళీలు_గమ్యం_null(secure_getenv("SYSTEMD_INVOKED_AS"));

        ఒకవేళ (!progname && argv)
                progname = argv[0];

        ఒకవేళ (isempty(progname) || isempty(token))
                తిరిగి false;

        తిరిగి అక్షరశ్రేణి_శోధన(చివరి_మార్గం_component(progname), token);
}

bool invoked_by_systemd(శూన్య) {
        స్థిర పూర్ణాంకం cached = -1;
        పూర్ణాంకం r;

        ఒకవేళ (cached >= 0)
                తిరిగి cached;

        /* If the process is directly executed by PID1 (e.g. ExecStart= or generator), systemd-importd,
         * or systemd-homed, then $SYSTEMD_EXEC_PID= is set, and read the command line. */
        స్థిరరాశి అక్షరం *e = getenv("SYSTEMD_EXEC_PID");
        ఒకవేళ (!e)
                తిరిగి (cached = false);

        ఒకవేళ (streq(e, "*"))
                /* For testing. */
                తిరిగి (cached = true);

        pid_t p;
        r = parse_pid(e, &p);
        ఒకవేళ (r < 0) {
                /* We know that systemd sets the variable correctly. Something else must have set it. */
                log_debug_errno(r, "Failed to parse \"SYSTEMD_EXEC_PID=%s\", ignoring: %m", e);
                తిరిగి (cached = false);
        }

        cached = getpid_cached() == p;
        తిరిగి cached;
}

స్థిర పూర్ణాంకం update_argv(స్థిరరాశి అక్షరం name[], size_t l) {
        స్థిర పూర్ణాంకం can_do = -1;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(name);
        ప్రతిజ్ఞా(l < SIZE_MAX);

        ఒకవేళ (can_do == 0)
                తిరిగి 0;
        can_do = false; /* We'll set it to true only if the whole process works */

        /* Calling prctl() with PR_SET_MM_ARG_{START,END} requires CAP_SYS_RESOURCE so let's use this as quick bypass
         * check, to avoid calling mmap() should PR_SET_MM_ARG_{START,END} fail with EPERM later on anyway. */
        r = have_effective_cap(CAP_SYS_RESOURCE);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to check if we have enough privileges: %m");
        ఒకవేళ (r == 0)
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Skipping PR_SET_MM, as we don't have privileges.");

        స్థిర size_t mm_కొలత = 0;
        స్థిర అక్షరం *mm = ఖాళీ;

        ఒకవేళ (mm_కొలత < l+1) {
                size_t nn_కొలత;
                అక్షరం *nn;

                nn_కొలత = పుట_ALIGN(l+1);
                ఒకవేళ (nn_కొలత >= SIZE_MAX)
                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The requested argument is too long.");

                nn = mmap(ఖాళీ, nn_కొలత, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
                ఒకవేళ (nn == MAP_FAILED)
                        తిరిగి log_debug_errno(errno, "mmap() failed: %m");

                strncpy(nn, name, nn_కొలత);

                /* Now, let's tell the kernel about this new memory */
                r = prctl_సురక్షిత(PR_SET_MM, PR_SET_MM_ARG_START, (గుర్తులేని దీర్ఘ) nn, 0, 0);
                ఒకవేళ (ERRNO_IS_NEG_PRIVILEGE(r))
                        తిరిగి log_debug_errno(r, "PR_SET_MM_ARG_START failed: %m");
                ఒకవేళ (r < 0) {
                        /* HACK: prctl() API is kind of dumb on this point.  The existing end address may already be
                         * below the desired start address, in which case the kernel may have kicked this back due
                         * to a range-check failure (see linux/kernel/sys.c:validate_prctl_map() to see this in
                         * action).  The proper solution would be to have a prctl() API that could set both start+end
                         * simultaneously, or at least let us query the existing address to anticipate this condition
                         * and respond accordingly.  For now, we can only guess at the cause of this failure and try
                         * a workaround--which will briefly expand the arg space to something potentially huge before
                         * resizing it to what we want. */
                        log_debug_errno(r, "PR_SET_MM_ARG_START failed, attempting PR_SET_MM_ARG_END hack: %m");

                        r = prctl_సురక్షిత(PR_SET_MM, PR_SET_MM_ARG_END, (గుర్తులేని దీర్ఘ) nn + l + 1, 0, 0);
                        ఒకవేళ (r < 0) {
                                log_debug_errno(r, "PR_SET_MM_ARG_END hack failed, proceeding without: %m");
                                munmap_సురక్షిత(nn, nn_కొలత);
                                తిరిగి r;
                        }

                        r = prctl_సురక్షిత(PR_SET_MM, PR_SET_MM_ARG_START, (గుర్తులేని దీర్ఘ) nn, 0, 0);
                        ఒకవేళ (r < 0)
                                తిరిగి log_debug_errno(r, "PR_SET_MM_ARG_START still failed, proceeding without: %m");
                } లేకపోతే {
                        /* And update the end pointer to the new end, too. If this fails, we don't really know what
                         * to do, it's pretty unlikely that we can rollback, hence we'll just accept the failure,
                         * and continue. */
                        r = prctl_సురక్షిత(PR_SET_MM, PR_SET_MM_ARG_END, (గుర్తులేని దీర్ఘ) nn + l + 1, 0, 0);
                        ఒకవేళ (r < 0)
                                log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
                }

                munmap_సురక్షిత(mm, mm_కొలత);

                mm = nn;
                mm_కొలత = nn_కొలత;
        } లేకపోతే {
                strncpy(mm, name, mm_కొలత);

                /* Update the end pointer, continuing regardless of any failure. */
                r = prctl_సురక్షిత(PR_SET_MM, PR_SET_MM_ARG_END, (గుర్తులేని దీర్ఘ) mm + l + 1, 0, 0);
                ఒకవేళ (r < 0)
                        log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
        }

        can_do = true;
        తిరిగి 0;
}

పూర్ణాంకం rename_process_నిండిన(స్థిరరాశి అక్షరం *comm, స్థిరరాశి అక్షరం *invocation) {
        bool truncated = false;
        పూర్ణాంకం r;

        /* This is a like a poor man's setproctitle(). It changes the comm field by the name specified by
         * 'comm', and changes argv[0] and the glibc's internally used names of the process
         * (program_invocation_name and program_invocation_short_name) by the name specified by 'invocation'.
         * For the first one a limit of 16 chars applies; to the second one in many cases one of 10 (i.e.
         * length of "/sbin/init") — however if we have CAP_SYS_RESOURCES it is unbounded; to the third one
         * 7 (i.e. the length of "systemd". If you pass a longer string it will likely be truncated.
         *
         * Returns 0 if a name was set but truncated, > 0 if it was set but not truncated. */

        ఒకవేళ (isempty(comm))
                తిరిగి -EINVAL; /* let's not confuse users unnecessarily with an empty name */

        ఒకవేళ (!is_main_thread())
                తిరిగి -EPERM; /* Let's not allow setting the process name from other threads than the main one, as we
                                * cache things without locking, and we make assumptions that PR_SET_NAME sets the
                                * process name that isn't correct on any other threads */

        size_t l = అక్షరశ్రేణి_పొడవు(comm);

        /* First step, change the comm field. The main thread's comm is identical to the process comm. This means we
         * can use PR_SET_NAME, which sets the thread name for the calling thread. */
        r = proc_సమితి_comm(comm);
        ఒకవేళ (r < 0)
                log_debug_errno(r, "PR_SET_NAME failed: %m");
        ఒకవేళ (l >= TASK_COMM_LEN) /* Linux userspace process names can be 15 chars at max */
                truncated = true;

        /* If nothing specified, fall back to comm. */
        ఒకవేళ (isempty(invocation))
                invocation = comm;

        l = అక్షరశ్రేణి_పొడవు(invocation);

        /* Second step, change glibc's ID of the process name. */
        ఒకవేళ (కార్యక్రమం_invocation_name) {
                size_t k;

                k = అక్షరశ్రేణి_పొడవు(కార్యక్రమం_invocation_name);
                strncpy(కార్యక్రమం_invocation_name, invocation, k);
                ఒకవేళ (l > k)
                        truncated = true;

                /* Also update the short name. */
                అక్షరం *p = strrchr(కార్యక్రమం_invocation_name, '/');
                కార్యక్రమం_invocation_short_name = p ? p + 1 : కార్యక్రమం_invocation_name;
        }

        /* Third step, completely replace the argv[] array the kernel maintains for us. This requires privileges, but
         * has the advantage that the argv[] array is exactly what we want it to be, and not filled up with zeros at
         * the end. This is the best option for changing /proc/self/cmdline. */
        (శూన్య) update_argv(invocation, l);

        /* Fourth step: in all cases we'll also update the original argv[], so that our own code gets it right too if
         * it still looks here */
        ఒకవేళ (saved_argc > 0) {
                ఒకవేళ (saved_argv[0]) {
                        size_t k;

                        k = అక్షరశ్రేణి_పొడవు(saved_argv[0]);
                        strncpy(saved_argv[0], invocation, k);
                        ఒకవేళ (l > k)
                                truncated = true;
                }

                కొరకు (పూర్ణాంకం i = 1; i < saved_argc; i++) {
                        ఒకవేళ (!saved_argv[i])
                                ఆపు;

                        memzero(saved_argv[i], అక్షరశ్రేణి_పొడవు(saved_argv[i]));
                }
        }

        తిరిగి !truncated;
}

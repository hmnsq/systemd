/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <sched.h>
#شامل <stdlib.h>
#شامل <sys/mman.h>
#شامل <sys/prctl.h> /* IWYU pragma: keep */

#شامل "argv-util.h"
#شامل "capability-util.h"
#شامل "errno-util.h"
#شامل "log.h"
#شامل "memory-util.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "process-util.h"
#شامل "string-util.h"

صحیح saved_argc = 0;
حرف **saved_argv = تہی;

خالی save_argc_argv(صحیح argc, حرف **argv) {
        /* Protect against CVE-2021-4034 style attacks */
        assert_se(argc > 0);
        assert_se(argv);
        assert_se(argv[0]);

        saved_argc = argc;
        saved_argv = argv;
}

bool invoked_as(حرف *argv[], ثابت حرف *token) {
        ثابت حرف *progname = خالی_خانے_مقصد_null(secure_getenv("SYSTEMD_INVOKED_AS"));

        اگر (!progname && argv)
                progname = argv[0];

        اگر (isempty(progname) || isempty(token))
                واپس false;

        واپس لڑی_تلاش(آخری_راستہ_component(progname), token);
}

bool invoked_by_systemd(خالی) {
        ساکن صحیح cached = -1;
        صحیح r;

        اگر (cached >= 0)
                واپس cached;

        /* If the process is directly executed by PID1 (e.g. ExecStart= or generator), systemd-importd,
         * or systemd-homed, then $SYSTEMD_EXEC_PID= is set, and read the command line. */
        ثابت حرف *e = getenv("SYSTEMD_EXEC_PID");
        اگر (!e)
                واپس (cached = false);

        اگر (streq(e, "*"))
                /* For testing. */
                واپس (cached = true);

        pid_t p;
        r = parse_pid(e, &p);
        اگر (r < 0) {
                /* We know that systemd sets the variable correctly. Something else must have set it. */
                log_debug_errno(r, "Failed to parse \"SYSTEMD_EXEC_PID=%s\", ignoring: %m", e);
                واپس (cached = false);
        }

        cached = getpid_cached() == p;
        واپس cached;
}

ساکن صحیح update_argv(ثابت حرف name[], size_t l) {
        ساکن صحیح can_do = -1;
        صحیح r;

        أكد(name);
        أكد(l < SIZE_MAX);

        اگر (can_do == 0)
                واپس 0;
        can_do = false; /* We'll set it to true only if the whole process works */

        /* Calling prctl() with PR_SET_MM_ARG_{START,END} requires CAP_SYS_RESOURCE so let's use this as quick bypass
         * check, to avoid calling mmap() should PR_SET_MM_ARG_{START,END} fail with EPERM later on anyway. */
        r = have_effective_cap(CAP_SYS_RESOURCE);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to check if we have enough privileges: %m");
        اگر (r == 0)
                واپس log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Skipping PR_SET_MM, as we don't have privileges.");

        ساکن size_t mm_سائز = 0;
        ساکن حرف *mm = تہی;

        اگر (mm_سائز < l+1) {
                size_t nn_سائز;
                حرف *nn;

                nn_سائز = صفحہ_ALIGN(l+1);
                اگر (nn_سائز >= SIZE_MAX)
                        واپس log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The requested argument is too long.");

                nn = mmap(تہی, nn_سائز, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
                اگر (nn == MAP_FAILED)
                        واپس log_debug_errno(errno, "mmap() failed: %m");

                strncpy(nn, name, nn_سائز);

                /* Now, let's tell the kernel about this new memory */
                r = prctl_محفوظ(PR_SET_MM, PR_SET_MM_ARG_START, (غیرعلامتی طویل) nn, 0, 0);
                اگر (ERRNO_IS_NEG_PRIVILEGE(r))
                        واپس log_debug_errno(r, "PR_SET_MM_ARG_START failed: %m");
                اگر (r < 0) {
                        /* HACK: prctl() API is kind of dumb on this point.  The existing end address may already be
                         * below the desired start address, in which case the kernel may have kicked this back due
                         * to a range-check failure (see linux/kernel/sys.c:validate_prctl_map() to see this in
                         * action).  The proper solution would be to have a prctl() API that could set both start+end
                         * simultaneously, or at least let us query the existing address to anticipate this condition
                         * and respond accordingly.  For now, we can only guess at the cause of this failure and try
                         * a workaround--which will briefly expand the arg space to something potentially huge before
                         * resizing it to what we want. */
                        log_debug_errno(r, "PR_SET_MM_ARG_START failed, attempting PR_SET_MM_ARG_END hack: %m");

                        r = prctl_محفوظ(PR_SET_MM, PR_SET_MM_ARG_END, (غیرعلامتی طویل) nn + l + 1, 0, 0);
                        اگر (r < 0) {
                                log_debug_errno(r, "PR_SET_MM_ARG_END hack failed, proceeding without: %m");
                                munmap_محفوظ(nn, nn_سائز);
                                واپس r;
                        }

                        r = prctl_محفوظ(PR_SET_MM, PR_SET_MM_ARG_START, (غیرعلامتی طویل) nn, 0, 0);
                        اگر (r < 0)
                                واپس log_debug_errno(r, "PR_SET_MM_ARG_START still failed, proceeding without: %m");
                } ورنہ {
                        /* And update the end pointer to the new end, too. If this fails, we don't really know what
                         * to do, it's pretty unlikely that we can rollback, hence we'll just accept the failure,
                         * and continue. */
                        r = prctl_محفوظ(PR_SET_MM, PR_SET_MM_ARG_END, (غیرعلامتی طویل) nn + l + 1, 0, 0);
                        اگر (r < 0)
                                log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
                }

                munmap_محفوظ(mm, mm_سائز);

                mm = nn;
                mm_سائز = nn_سائز;
        } ورنہ {
                strncpy(mm, name, mm_سائز);

                /* Update the end pointer, continuing regardless of any failure. */
                r = prctl_محفوظ(PR_SET_MM, PR_SET_MM_ARG_END, (غیرعلامتی طویل) mm + l + 1, 0, 0);
                اگر (r < 0)
                        log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
        }

        can_do = true;
        واپس 0;
}

صحیح rename_process_بھرا(ثابت حرف *comm, ثابت حرف *invocation) {
        bool truncated = false;
        صحیح r;

        /* This is a like a poor man's setproctitle(). It changes the comm field by the name specified by
         * 'comm', and changes argv[0] and the glibc's internally used names of the process
         * (program_invocation_name and program_invocation_short_name) by the name specified by 'invocation'.
         * For the first one a limit of 16 chars applies; to the second one in many cases one of 10 (i.e.
         * length of "/sbin/init") — however if we have CAP_SYS_RESOURCES it is unbounded; to the third one
         * 7 (i.e. the length of "systemd". If you pass a longer string it will likely be truncated.
         *
         * Returns 0 if a name was set but truncated, > 0 if it was set but not truncated. */

        اگر (isempty(comm))
                واپس -EINVAL; /* let's not confuse users unnecessarily with an empty name */

        اگر (!is_main_thread())
                واپس -EPERM; /* Let's not allow setting the process name from other threads than the main one, as we
                                * cache things without locking, and we make assumptions that PR_SET_NAME sets the
                                * process name that isn't correct on any other threads */

        size_t l = لڑی_لمبائی(comm);

        /* First step, change the comm field. The main thread's comm is identical to the process comm. This means we
         * can use PR_SET_NAME, which sets the thread name for the calling thread. */
        r = proc_سیٹ_comm(comm);
        اگر (r < 0)
                log_debug_errno(r, "PR_SET_NAME failed: %m");
        اگر (l >= TASK_COMM_LEN) /* Linux userspace process names can be 15 chars at max */
                truncated = true;

        /* If nothing specified, fall back to comm. */
        اگر (isempty(invocation))
                invocation = comm;

        l = لڑی_لمبائی(invocation);

        /* Second step, change glibc's ID of the process name. */
        اگر (پروگرام_invocation_name) {
                size_t k;

                k = لڑی_لمبائی(پروگرام_invocation_name);
                strncpy(پروگرام_invocation_name, invocation, k);
                اگر (l > k)
                        truncated = true;

                /* Also update the short name. */
                حرف *p = strrchr(پروگرام_invocation_name, '/');
                پروگرام_invocation_short_name = p ? p + 1 : پروگرام_invocation_name;
        }

        /* Third step, completely replace the argv[] array the kernel maintains for us. This requires privileges, but
         * has the advantage that the argv[] array is exactly what we want it to be, and not filled up with zeros at
         * the end. This is the best option for changing /proc/self/cmdline. */
        (خالی) update_argv(invocation, l);

        /* Fourth step: in all cases we'll also update the original argv[], so that our own code gets it right too if
         * it still looks here */
        اگر (saved_argc > 0) {
                اگر (saved_argv[0]) {
                        size_t k;

                        k = لڑی_لمبائی(saved_argv[0]);
                        strncpy(saved_argv[0], invocation, k);
                        اگر (l > k)
                                truncated = true;
                }

                برائے (صحیح i = 1; i < saved_argc; i++) {
                        اگر (!saved_argv[i])
                                توڑو;

                        memzero(saved_argv[i], لڑی_لمبائی(saved_argv[i]));
                }
        }

        واپس !truncated;
}

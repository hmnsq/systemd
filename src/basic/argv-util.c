/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <sched.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/mman.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/prctl.h> /* IWYU pragma: keep */

#ਅਨ੍ਤਰ੍ਭਾਵ "argv-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "capability-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਪੂਰਨ saved_argc = 0;
ਅੱਖਰ **saved_argv = ਨਿਰਾ;

ਖਾਲੀ save_argc_argv(ਪੂਰਨ argc, ਅੱਖਰ **argv) {
        /* Protect against CVE-2021-4034 style attacks */
        assert_se(argc > 0);
        assert_se(argv);
        assert_se(argv[0]);

        saved_argc = argc;
        saved_argv = argv;
}

bool invoked_as(ਅੱਖਰ *argv[], ਅਚਲ ਅੱਖਰ *token) {
        ਅਚਲ ਅੱਖਰ *progname = ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_null(secure_getenv("SYSTEMD_INVOKED_AS"));

        ਜੇ (!progname && argv)
                progname = argv[0];

        ਜੇ (isempty(progname) || isempty(token))
                ਵਾਪਸ false;

        ਵਾਪਸ ਲੜੀ_ਖੋਜ(ਆਖਰੀ_ਰਸਤਾ_component(progname), token);
}

bool invoked_by_systemd(ਖਾਲੀ) {
        ਸਥਿਰ ਪੂਰਨ cached = -1;
        ਪੂਰਨ r;

        ਜੇ (cached >= 0)
                ਵਾਪਸ cached;

        /* If the process is directly executed by PID1 (e.g. ExecStart= or generator), systemd-importd,
         * or systemd-homed, then $SYSTEMD_EXEC_PID= is set, and read the command line. */
        ਅਚਲ ਅੱਖਰ *e = getenv("SYSTEMD_EXEC_PID");
        ਜੇ (!e)
                ਵਾਪਸ (cached = false);

        ਜੇ (streq(e, "*"))
                /* For testing. */
                ਵਾਪਸ (cached = true);

        pid_t p;
        r = parse_pid(e, &p);
        ਜੇ (r < 0) {
                /* We know that systemd sets the variable correctly. Something else must have set it. */
                log_debug_errno(r, "Failed to parse \"SYSTEMD_EXEC_PID=%s\", ignoring: %m", e);
                ਵਾਪਸ (cached = false);
        }

        cached = getpid_cached() == p;
        ਵਾਪਸ cached;
}

ਸਥਿਰ ਪੂਰਨ update_argv(ਅਚਲ ਅੱਖਰ name[], size_t l) {
        ਸਥਿਰ ਪੂਰਨ can_do = -1;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);
        ਪ੍ਰਤਿਜ੍ਞਾ(l < SIZE_MAX);

        ਜੇ (can_do == 0)
                ਵਾਪਸ 0;
        can_do = false; /* We'll set it to true only if the whole process works */

        /* Calling prctl() with PR_SET_MM_ARG_{START,END} requires CAP_SYS_RESOURCE so let's use this as quick bypass
         * check, to avoid calling mmap() should PR_SET_MM_ARG_{START,END} fail with EPERM later on anyway. */
        r = have_effective_cap(CAP_SYS_RESOURCE);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to check if we have enough privileges: %m");
        ਜੇ (r == 0)
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Skipping PR_SET_MM, as we don't have privileges.");

        ਸਥਿਰ size_t mm_ਮਾਪ = 0;
        ਸਥਿਰ ਅੱਖਰ *mm = ਨਿਰਾ;

        ਜੇ (mm_ਮਾਪ < l+1) {
                size_t nn_ਮਾਪ;
                ਅੱਖਰ *nn;

                nn_ਮਾਪ = ਪੰਨਾ_ALIGN(l+1);
                ਜੇ (nn_ਮਾਪ >= SIZE_MAX)
                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The requested argument is too long.");

                nn = mmap(ਨਿਰਾ, nn_ਮਾਪ, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
                ਜੇ (nn == MAP_FAILED)
                        ਵਾਪਸ log_debug_errno(errno, "mmap() failed: %m");

                strncpy(nn, name, nn_ਮਾਪ);

                /* Now, let's tell the kernel about this new memory */
                r = prctl_ਸੁਰੱਖਿਅਤ(PR_SET_MM, PR_SET_MM_ARG_START, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) nn, 0, 0);
                ਜੇ (ERRNO_IS_NEG_PRIVILEGE(r))
                        ਵਾਪਸ log_debug_errno(r, "PR_SET_MM_ARG_START failed: %m");
                ਜੇ (r < 0) {
                        /* HACK: prctl() API is kind of dumb on this point.  The existing end address may already be
                         * below the desired start address, in which case the kernel may have kicked this back due
                         * to a range-check failure (see linux/kernel/sys.c:validate_prctl_map() to see this in
                         * action).  The proper solution would be to have a prctl() API that could set both start+end
                         * simultaneously, or at least let us query the existing address to anticipate this condition
                         * and respond accordingly.  For now, we can only guess at the cause of this failure and try
                         * a workaround--which will briefly expand the arg space to something potentially huge before
                         * resizing it to what we want. */
                        log_debug_errno(r, "PR_SET_MM_ARG_START failed, attempting PR_SET_MM_ARG_END hack: %m");

                        r = prctl_ਸੁਰੱਖਿਅਤ(PR_SET_MM, PR_SET_MM_ARG_END, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) nn + l + 1, 0, 0);
                        ਜੇ (r < 0) {
                                log_debug_errno(r, "PR_SET_MM_ARG_END hack failed, proceeding without: %m");
                                munmap_ਸੁਰੱਖਿਅਤ(nn, nn_ਮਾਪ);
                                ਵਾਪਸ r;
                        }

                        r = prctl_ਸੁਰੱਖਿਅਤ(PR_SET_MM, PR_SET_MM_ARG_START, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) nn, 0, 0);
                        ਜੇ (r < 0)
                                ਵਾਪਸ log_debug_errno(r, "PR_SET_MM_ARG_START still failed, proceeding without: %m");
                } ਨਹੀਂਤਾਂ {
                        /* And update the end pointer to the new end, too. If this fails, we don't really know what
                         * to do, it's pretty unlikely that we can rollback, hence we'll just accept the failure,
                         * and continue. */
                        r = prctl_ਸੁਰੱਖਿਅਤ(PR_SET_MM, PR_SET_MM_ARG_END, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) nn + l + 1, 0, 0);
                        ਜੇ (r < 0)
                                log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
                }

                munmap_ਸੁਰੱਖਿਅਤ(mm, mm_ਮਾਪ);

                mm = nn;
                mm_ਮਾਪ = nn_ਮਾਪ;
        } ਨਹੀਂਤਾਂ {
                strncpy(mm, name, mm_ਮਾਪ);

                /* Update the end pointer, continuing regardless of any failure. */
                r = prctl_ਸੁਰੱਖਿਅਤ(PR_SET_MM, PR_SET_MM_ARG_END, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) mm + l + 1, 0, 0);
                ਜੇ (r < 0)
                        log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
        }

        can_do = true;
        ਵਾਪਸ 0;
}

ਪੂਰਨ rename_process_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *comm, ਅਚਲ ਅੱਖਰ *invocation) {
        bool truncated = false;
        ਪੂਰਨ r;

        /* This is a like a poor man's setproctitle(). It changes the comm field by the name specified by
         * 'comm', and changes argv[0] and the glibc's internally used names of the process
         * (program_invocation_name and program_invocation_short_name) by the name specified by 'invocation'.
         * For the first one a limit of 16 chars applies; to the second one in many cases one of 10 (i.e.
         * length of "/sbin/init") — however if we have CAP_SYS_RESOURCES it is unbounded; to the third one
         * 7 (i.e. the length of "systemd". If you pass a longer string it will likely be truncated.
         *
         * Returns 0 if a name was set but truncated, > 0 if it was set but not truncated. */

        ਜੇ (isempty(comm))
                ਵਾਪਸ -EINVAL; /* let's not confuse users unnecessarily with an empty name */

        ਜੇ (!is_main_thread())
                ਵਾਪਸ -EPERM; /* Let's not allow setting the process name from other threads than the main one, as we
                                * cache things without locking, and we make assumptions that PR_SET_NAME sets the
                                * process name that isn't correct on any other threads */

        size_t l = ਲੜੀ_ਲੰਬਾਈ(comm);

        /* First step, change the comm field. The main thread's comm is identical to the process comm. This means we
         * can use PR_SET_NAME, which sets the thread name for the calling thread. */
        r = proc_ਸੈੱਟ_comm(comm);
        ਜੇ (r < 0)
                log_debug_errno(r, "PR_SET_NAME failed: %m");
        ਜੇ (l >= TASK_COMM_LEN) /* Linux userspace process names can be 15 chars at max */
                truncated = true;

        /* If nothing specified, fall back to comm. */
        ਜੇ (isempty(invocation))
                invocation = comm;

        l = ਲੜੀ_ਲੰਬਾਈ(invocation);

        /* Second step, change glibc's ID of the process name. */
        ਜੇ (ਪ੍ਰੋਗਰਾਮ_invocation_name) {
                size_t k;

                k = ਲੜੀ_ਲੰਬਾਈ(ਪ੍ਰੋਗਰਾਮ_invocation_name);
                strncpy(ਪ੍ਰੋਗਰਾਮ_invocation_name, invocation, k);
                ਜੇ (l > k)
                        truncated = true;

                /* Also update the short name. */
                ਅੱਖਰ *p = strrchr(ਪ੍ਰੋਗਰਾਮ_invocation_name, '/');
                ਪ੍ਰੋਗਰਾਮ_invocation_short_name = p ? p + 1 : ਪ੍ਰੋਗਰਾਮ_invocation_name;
        }

        /* Third step, completely replace the argv[] array the kernel maintains for us. This requires privileges, but
         * has the advantage that the argv[] array is exactly what we want it to be, and not filled up with zeros at
         * the end. This is the best option for changing /proc/self/cmdline. */
        (ਖਾਲੀ) update_argv(invocation, l);

        /* Fourth step: in all cases we'll also update the original argv[], so that our own code gets it right too if
         * it still looks here */
        ਜੇ (saved_argc > 0) {
                ਜੇ (saved_argv[0]) {
                        size_t k;

                        k = ਲੜੀ_ਲੰਬਾਈ(saved_argv[0]);
                        strncpy(saved_argv[0], invocation, k);
                        ਜੇ (l > k)
                                truncated = true;
                }

                ਲਈ (ਪੂਰਨ i = 1; i < saved_argc; i++) {
                        ਜੇ (!saved_argv[i])
                                ਰੋਕੋ;

                        memzero(saved_argv[i], ਲੜੀ_ਲੰਬਾਈ(saved_argv[i]));
                }
        }

        ਵਾਪਸ !truncated;
}

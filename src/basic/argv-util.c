/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <sched.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <sys/mman.h>
#അന്തര്ഭാവ <sys/prctl.h> /* IWYU pragma: keep */

#അന്തര്ഭാവ "argv-util.h"
#അന്തര്ഭാവ "capability-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "string-util.h"

പൂർണ്ണസംഖ്യ saved_argc = 0;
അക്ഷരം **saved_argv = ശൂന്യ;

ശൂന്യം save_argc_argv(പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv) {
        /* Protect against CVE-2021-4034 style attacks */
        assert_se(argc > 0);
        assert_se(argv);
        assert_se(argv[0]);

        saved_argc = argc;
        saved_argv = argv;
}

bool invoked_as(അക്ഷരം *argv[], സ്ഥിരം അക്ഷരം *token) {
        സ്ഥിരം അക്ഷരം *progname = empty_to_null(secure_getenv("SYSTEMD_INVOKED_AS"));

        എങ്കിൽ (!progname && argv)
                progname = argv[0];

        എങ്കിൽ (isempty(progname) || isempty(token))
                തിരികെ false;

        തിരികെ strstr(last_path_component(progname), token);
}

bool invoked_by_systemd(ശൂന്യം) {
        സ്ഥിതം പൂർണ്ണസംഖ്യ cached = -1;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (cached >= 0)
                തിരികെ cached;

        /* If the process is directly executed by PID1 (e.g. ExecStart= or generator), systemd-importd,
         * or systemd-homed, then $SYSTEMD_EXEC_PID= is set, and read the command line. */
        സ്ഥിരം അക്ഷരം *e = getenv("SYSTEMD_EXEC_PID");
        എങ്കിൽ (!e)
                തിരികെ (cached = false);

        എങ്കിൽ (streq(e, "*"))
                /* For testing. */
                തിരികെ (cached = true);

        pid_t p;
        r = parse_pid(e, &p);
        എങ്കിൽ (r < 0) {
                /* We know that systemd sets the variable correctly. Something else must have set it. */
                log_debug_errno(r, "Failed to parse \"SYSTEMD_EXEC_PID=%s\", ignoring: %m", e);
                തിരികെ (cached = false);
        }

        cached = getpid_cached() == p;
        തിരികെ cached;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ update_argv(സ്ഥിരം അക്ഷരം name[], size_t l) {
        സ്ഥിതം പൂർണ്ണസംഖ്യ can_do = -1;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(name);
        പ്രതിജ്ഞാ(l < SIZE_MAX);

        എങ്കിൽ (can_do == 0)
                തിരികെ 0;
        can_do = false; /* We'll set it to true only if the whole process works */

        /* Calling prctl() with PR_SET_MM_ARG_{START,END} requires CAP_SYS_RESOURCE so let's use this as quick bypass
         * check, to avoid calling mmap() should PR_SET_MM_ARG_{START,END} fail with EPERM later on anyway. */
        r = have_effective_cap(CAP_SYS_RESOURCE);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to check if we have enough privileges: %m");
        എങ്കിൽ (r == 0)
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Skipping PR_SET_MM, as we don't have privileges.");

        സ്ഥിതം size_t mm_size = 0;
        സ്ഥിതം അക്ഷരം *mm = ശൂന്യ;

        എങ്കിൽ (mm_size < l+1) {
                size_t nn_size;
                അക്ഷരം *nn;

                nn_size = PAGE_ALIGN(l+1);
                എങ്കിൽ (nn_size >= SIZE_MAX)
                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The requested argument is too long.");

                nn = mmap(ശൂന്യ, nn_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
                എങ്കിൽ (nn == MAP_FAILED)
                        തിരികെ log_debug_errno(errno, "mmap() failed: %m");

                strncpy(nn, name, nn_size);

                /* Now, let's tell the kernel about this new memory */
                r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_START, (unsigned long) nn, 0, 0);
                എങ്കിൽ (ERRNO_IS_NEG_PRIVILEGE(r))
                        തിരികെ log_debug_errno(r, "PR_SET_MM_ARG_START failed: %m");
                എങ്കിൽ (r < 0) {
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
                        എങ്കിൽ (r < 0) {
                                log_debug_errno(r, "PR_SET_MM_ARG_END hack failed, proceeding without: %m");
                                munmap_safe(nn, nn_size);
                                തിരികെ r;
                        }

                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_START, (unsigned long) nn, 0, 0);
                        എങ്കിൽ (r < 0)
                                തിരികെ log_debug_errno(r, "PR_SET_MM_ARG_START still failed, proceeding without: %m");
                } അല്ലെങ്കിൽ {
                        /* And update the end pointer to the new end, too. If this fails, we don't really know what
                         * to do, it's pretty unlikely that we can rollback, hence we'll just accept the failure,
                         * and continue. */
                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (unsigned long) nn + l + 1, 0, 0);
                        എങ്കിൽ (r < 0)
                                log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
                }

                munmap_safe(mm, mm_size);

                mm = nn;
                mm_size = nn_size;
        } അല്ലെങ്കിൽ {
                strncpy(mm, name, mm_size);

                /* Update the end pointer, continuing regardless of any failure. */
                r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (unsigned long) mm + l + 1, 0, 0);
                എങ്കിൽ (r < 0)
                        log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
        }

        can_do = true;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ rename_process_full(സ്ഥിരം അക്ഷരം *comm, സ്ഥിരം അക്ഷരം *invocation) {
        bool truncated = false;
        പൂർണ്ണസംഖ്യ r;

        /* This is a like a poor man's setproctitle(). It changes the comm field by the name specified by
         * 'comm', and changes argv[0] and the glibc's internally used names of the process
         * (program_invocation_name and program_invocation_short_name) by the name specified by 'invocation'.
         * For the first one a limit of 16 chars applies; to the second one in many cases one of 10 (i.e.
         * length of "/sbin/init") — however if we have CAP_SYS_RESOURCES it is unbounded; to the third one
         * 7 (i.e. the length of "systemd". If you pass a longer string it will likely be truncated.
         *
         * Returns 0 if a name was set but truncated, > 0 if it was set but not truncated. */

        എങ്കിൽ (isempty(comm))
                തിരികെ -EINVAL; /* let's not confuse users unnecessarily with an empty name */

        എങ്കിൽ (!is_main_thread())
                തിരികെ -EPERM; /* Let's not allow setting the process name from other threads than the main one, as we
                                * cache things without locking, and we make assumptions that PR_SET_NAME sets the
                                * process name that isn't correct on any other threads */

        size_t l = strlen(comm);

        /* First step, change the comm field. The main thread's comm is identical to the process comm. This means we
         * can use PR_SET_NAME, which sets the thread name for the calling thread. */
        r = proc_set_comm(comm);
        എങ്കിൽ (r < 0)
                log_debug_errno(r, "PR_SET_NAME failed: %m");
        എങ്കിൽ (l >= TASK_COMM_LEN) /* Linux userspace process names can be 15 chars at max */
                truncated = true;

        /* If nothing specified, fall back to comm. */
        എങ്കിൽ (isempty(invocation))
                invocation = comm;

        l = strlen(invocation);

        /* Second step, change glibc's ID of the process name. */
        എങ്കിൽ (program_invocation_name) {
                size_t k;

                k = strlen(program_invocation_name);
                strncpy(program_invocation_name, invocation, k);
                എങ്കിൽ (l > k)
                        truncated = true;

                /* Also update the short name. */
                അക്ഷരം *p = strrchr(program_invocation_name, '/');
                program_invocation_short_name = p ? p + 1 : program_invocation_name;
        }

        /* Third step, completely replace the argv[] array the kernel maintains for us. This requires privileges, but
         * has the advantage that the argv[] array is exactly what we want it to be, and not filled up with zeros at
         * the end. This is the best option for changing /proc/self/cmdline. */
        (ശൂന്യം) update_argv(invocation, l);

        /* Fourth step: in all cases we'll also update the original argv[], so that our own code gets it right too if
         * it still looks here */
        എങ്കിൽ (saved_argc > 0) {
                എങ്കിൽ (saved_argv[0]) {
                        size_t k;

                        k = strlen(saved_argv[0]);
                        strncpy(saved_argv[0], invocation, k);
                        എങ്കിൽ (l > k)
                                truncated = true;
                }

                വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < saved_argc; i++) {
                        എങ്കിൽ (!saved_argv[i])
                                നിർത്തുക;

                        memzero(saved_argv[i], strlen(saved_argv[i]));
                }
        }

        തിരികെ !truncated;
}

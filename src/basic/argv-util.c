/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <sched.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <sys/mman.h>
#அந்தர்பாவ <sys/prctl.h> /* IWYU pragma: keep */

#அந்தர்பாவ "argv-util.h"
#அந்தர்பாவ "capability-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "string-util.h"

முழு saved_argc = 0;
எழுத்து **saved_argv = வெற்று;

வெறுமை save_argc_argv(முழு argc, எழுத்து **argv) {
        /* Protect against CVE-2021-4034 style attacks */
        assert_se(argc > 0);
        assert_se(argv);
        assert_se(argv[0]);

        saved_argc = argc;
        saved_argv = argv;
}

bool invoked_as(எழுத்து *argv[], மாறிலி எழுத்து *token) {
        மாறிலி எழுத்து *progname = empty_to_null(secure_getenv("SYSTEMD_INVOKED_AS"));

        எனில் (!progname && argv)
                progname = argv[0];

        எனில் (isempty(progname) || isempty(token))
                திரும்பு false;

        திரும்பு strstr(last_path_component(progname), token);
}

bool invoked_by_systemd(வெறுமை) {
        நிலையான முழு cached = -1;
        முழு r;

        எனில் (cached >= 0)
                திரும்பு cached;

        /* If the process is directly executed by PID1 (e.g. ExecStart= or generator), systemd-importd,
         * or systemd-homed, then $SYSTEMD_EXEC_PID= is set, and read the command line. */
        மாறிலி எழுத்து *e = getenv("SYSTEMD_EXEC_PID");
        எனில் (!e)
                திரும்பு (cached = false);

        எனில் (streq(e, "*"))
                /* For testing. */
                திரும்பு (cached = true);

        pid_t p;
        r = parse_pid(e, &p);
        எனில் (r < 0) {
                /* We know that systemd sets the variable correctly. Something else must have set it. */
                log_debug_errno(r, "Failed to parse \"SYSTEMD_EXEC_PID=%s\", ignoring: %m", e);
                திரும்பு (cached = false);
        }

        cached = getpid_cached() == p;
        திரும்பு cached;
}

நிலையான முழு update_argv(மாறிலி எழுத்து name[], size_t l) {
        நிலையான முழு can_do = -1;
        முழு r;

        ப்ரதிஜ்ஞா(name);
        ப்ரதிஜ்ஞா(l < SIZE_MAX);

        எனில் (can_do == 0)
                திரும்பு 0;
        can_do = false; /* We'll set it to true only if the whole process works */

        /* Calling prctl() with PR_SET_MM_ARG_{START,END} requires CAP_SYS_RESOURCE so let's use this as quick bypass
         * check, to avoid calling mmap() should PR_SET_MM_ARG_{START,END} fail with EPERM later on anyway. */
        r = have_effective_cap(CAP_SYS_RESOURCE);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to check if we have enough privileges: %m");
        எனில் (r == 0)
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Skipping PR_SET_MM, as we don't have privileges.");

        நிலையான size_t mm_size = 0;
        நிலையான எழுத்து *mm = வெற்று;

        எனில் (mm_size < l+1) {
                size_t nn_size;
                எழுத்து *nn;

                nn_size = PAGE_ALIGN(l+1);
                எனில் (nn_size >= SIZE_MAX)
                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The requested argument is too long.");

                nn = mmap(வெற்று, nn_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
                எனில் (nn == MAP_FAILED)
                        திரும்பு log_debug_errno(errno, "mmap() failed: %m");

                strncpy(nn, name, nn_size);

                /* Now, let's tell the kernel about this new memory */
                r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_START, (unsigned long) nn, 0, 0);
                எனில் (ERRNO_IS_NEG_PRIVILEGE(r))
                        திரும்பு log_debug_errno(r, "PR_SET_MM_ARG_START failed: %m");
                எனில் (r < 0) {
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
                        எனில் (r < 0) {
                                log_debug_errno(r, "PR_SET_MM_ARG_END hack failed, proceeding without: %m");
                                munmap_safe(nn, nn_size);
                                திரும்பு r;
                        }

                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_START, (unsigned long) nn, 0, 0);
                        எனில் (r < 0)
                                திரும்பு log_debug_errno(r, "PR_SET_MM_ARG_START still failed, proceeding without: %m");
                } இல்லையெனில் {
                        /* And update the end pointer to the new end, too. If this fails, we don't really know what
                         * to do, it's pretty unlikely that we can rollback, hence we'll just accept the failure,
                         * and continue. */
                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (unsigned long) nn + l + 1, 0, 0);
                        எனில் (r < 0)
                                log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
                }

                munmap_safe(mm, mm_size);

                mm = nn;
                mm_size = nn_size;
        } இல்லையெனில் {
                strncpy(mm, name, mm_size);

                /* Update the end pointer, continuing regardless of any failure. */
                r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (unsigned long) mm + l + 1, 0, 0);
                எனில் (r < 0)
                        log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
        }

        can_do = true;
        திரும்பு 0;
}

முழு rename_process_full(மாறிலி எழுத்து *comm, மாறிலி எழுத்து *invocation) {
        bool truncated = false;
        முழு r;

        /* This is a like a poor man's setproctitle(). It changes the comm field by the name specified by
         * 'comm', and changes argv[0] and the glibc's internally used names of the process
         * (program_invocation_name and program_invocation_short_name) by the name specified by 'invocation'.
         * For the first one a limit of 16 chars applies; to the second one in many cases one of 10 (i.e.
         * length of "/sbin/init") — however if we have CAP_SYS_RESOURCES it is unbounded; to the third one
         * 7 (i.e. the length of "systemd". If you pass a longer string it will likely be truncated.
         *
         * Returns 0 if a name was set but truncated, > 0 if it was set but not truncated. */

        எனில் (isempty(comm))
                திரும்பு -EINVAL; /* let's not confuse users unnecessarily with an empty name */

        எனில் (!is_main_thread())
                திரும்பு -EPERM; /* Let's not allow setting the process name from other threads than the main one, as we
                                * cache things without locking, and we make assumptions that PR_SET_NAME sets the
                                * process name that isn't correct on any other threads */

        size_t l = strlen(comm);

        /* First step, change the comm field. The main thread's comm is identical to the process comm. This means we
         * can use PR_SET_NAME, which sets the thread name for the calling thread. */
        r = proc_set_comm(comm);
        எனில் (r < 0)
                log_debug_errno(r, "PR_SET_NAME failed: %m");
        எனில் (l >= TASK_COMM_LEN) /* Linux userspace process names can be 15 chars at max */
                truncated = true;

        /* If nothing specified, fall back to comm. */
        எனில் (isempty(invocation))
                invocation = comm;

        l = strlen(invocation);

        /* Second step, change glibc's ID of the process name. */
        எனில் (program_invocation_name) {
                size_t k;

                k = strlen(program_invocation_name);
                strncpy(program_invocation_name, invocation, k);
                எனில் (l > k)
                        truncated = true;

                /* Also update the short name. */
                எழுத்து *p = strrchr(program_invocation_name, '/');
                program_invocation_short_name = p ? p + 1 : program_invocation_name;
        }

        /* Third step, completely replace the argv[] array the kernel maintains for us. This requires privileges, but
         * has the advantage that the argv[] array is exactly what we want it to be, and not filled up with zeros at
         * the end. This is the best option for changing /proc/self/cmdline. */
        (வெறுமை) update_argv(invocation, l);

        /* Fourth step: in all cases we'll also update the original argv[], so that our own code gets it right too if
         * it still looks here */
        எனில் (saved_argc > 0) {
                எனில் (saved_argv[0]) {
                        size_t k;

                        k = strlen(saved_argv[0]);
                        strncpy(saved_argv[0], invocation, k);
                        எனில் (l > k)
                                truncated = true;
                }

                க்கு (முழு i = 1; i < saved_argc; i++) {
                        எனில் (!saved_argv[i])
                                நிறுத்து;

                        memzero(saved_argv[i], strlen(saved_argv[i]));
                }
        }

        திரும்பு !truncated;
}

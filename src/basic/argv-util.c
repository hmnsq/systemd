/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <sched.h>
#समावेश <stdlib.h>
#समावेश <sys/mman.h>
#समावेश <sys/prctl.h> /* IWYU pragma: keep */

#समावेश "argv-util.h"
#समावेश "capability-util.h"
#समावेश "errno-util.h"
#समावेश "log.h"
#समावेश "memory-util.h"
#समावेश "parse-util.h"
#समावेश "path-util.h"
#समावेश "process-util.h"
#समावेश "string-util.h"

पूर्णांक saved_argc = 0;
अक्षर **saved_argv = शून्य;

व्योम save_argc_argv(पूर्णांक argc, अक्षर **argv) {
        /* Protect against CVE-2021-4034 style attacks */
        assert_se(argc > 0);
        assert_se(argv);
        assert_se(argv[0]);

        saved_argc = argc;
        saved_argv = argv;
}

bool invoked_as(अक्षर *argv[], स्थिर अक्षर *token) {
        स्थिर अक्षर *progname = empty_to_null(secure_getenv("SYSTEMD_INVOKED_AS"));

        अगर (!progname && argv)
                progname = argv[0];

        अगर (isempty(progname) || isempty(token))
                वापस false;

        वापस म_माला(last_path_component(progname), token);
}

bool invoked_by_systemd(व्योम) {
        अटल पूर्णांक cached = -1;
        पूर्णांक r;

        अगर (cached >= 0)
                वापस cached;

        /* If the process is directly executed by PID1 (e.g. ExecStart= or generator), systemd-importd,
         * or systemd-homed, then $SYSTEMD_EXEC_PID= is set, and read the command line. */
        स्थिर अक्षर *e = दो_पर्या("SYSTEMD_EXEC_PID");
        अगर (!e)
                वापस (cached = false);

        अगर (streq(e, "*"))
                /* For testing. */
                वापस (cached = true);

        pid_t p;
        r = parse_pid(e, &p);
        अगर (r < 0) {
                /* We know that systemd sets the variable correctly. Something else must have set it. */
                log_debug_errno(r, "Failed to parse \"SYSTEMD_EXEC_PID=%s\", ignoring: %m", e);
                वापस (cached = false);
        }

        cached = getpid_cached() == p;
        वापस cached;
}

अटल पूर्णांक update_argv(स्थिर अक्षर name[], माप_प्रकार l) {
        अटल पूर्णांक can_do = -1;
        पूर्णांक r;

        निश्चित(name);
        निश्चित(l < SIZE_MAX);

        अगर (can_do == 0)
                वापस 0;
        can_do = false; /* We'll set it to true only if the whole process works */

        /* Calling prctl() with PR_SET_MM_ARG_{START,END} requires CAP_SYS_RESOURCE so let's use this as quick bypass
         * check, to avoid calling mmap() should PR_SET_MM_ARG_{START,END} fail with EPERM later on anyway. */
        r = have_effective_cap(CAP_SYS_RESOURCE);
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to check if we have enough privileges: %m");
        अगर (r == 0)
                वापस log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Skipping PR_SET_MM, as we don't have privileges.");

        अटल माप_प्रकार mm_आकार = 0;
        अटल अक्षर *mm = शून्य;

        अगर (mm_आकार < l+1) {
                माप_प्रकार nn_आकार;
                अक्षर *nn;

                nn_आकार = PAGE_ALIGN(l+1);
                अगर (nn_आकार >= SIZE_MAX)
                        वापस log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The requested argument is too long.");

                nn = mmap(शून्य, nn_आकार, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
                अगर (nn == MAP_FAILED)
                        वापस log_debug_errno(त्रुटि_सं, "mmap() failed: %m");

                म_नकलन(nn, name, nn_आकार);

                /* Now, let's tell the kernel about this new memory */
                r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_START, (अचिन्हित दीर्घ) nn, 0, 0);
                अगर (ERRNO_IS_NEG_PRIVILEGE(r))
                        वापस log_debug_errno(r, "PR_SET_MM_ARG_START failed: %m");
                अगर (r < 0) {
                        /* HACK: prctl() API is kind of dumb on this point.  The existing end address may already be
                         * below the desired start address, in which case the kernel may have kicked this back due
                         * to a range-check failure (see linux/kernel/sys.c:validate_prctl_map() to see this in
                         * action).  The proper solution would be to have a prctl() API that could set both start+end
                         * simultaneously, or at least let us query the existing address to anticipate this condition
                         * and respond accordingly.  For now, we can only guess at the cause of this failure and try
                         * a workaround--which will briefly expand the arg space to something potentially huge before
                         * resizing it to what we want. */
                        log_debug_errno(r, "PR_SET_MM_ARG_START failed, attempting PR_SET_MM_ARG_END hack: %m");

                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (अचिन्हित दीर्घ) nn + l + 1, 0, 0);
                        अगर (r < 0) {
                                log_debug_errno(r, "PR_SET_MM_ARG_END hack failed, proceeding without: %m");
                                munmap_safe(nn, nn_आकार);
                                वापस r;
                        }

                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_START, (अचिन्हित दीर्घ) nn, 0, 0);
                        अगर (r < 0)
                                वापस log_debug_errno(r, "PR_SET_MM_ARG_START still failed, proceeding without: %m");
                } अन्यथा {
                        /* And update the end pointer to the new end, too. If this fails, we don't really know what
                         * to do, it's pretty unlikely that we can rollback, hence we'll just accept the failure,
                         * and continue. */
                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (अचिन्हित दीर्घ) nn + l + 1, 0, 0);
                        अगर (r < 0)
                                log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
                }

                munmap_safe(mm, mm_आकार);

                mm = nn;
                mm_आकार = nn_आकार;
        } अन्यथा {
                म_नकलन(mm, name, mm_आकार);

                /* Update the end pointer, continuing regardless of any failure. */
                r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (अचिन्हित दीर्घ) mm + l + 1, 0, 0);
                अगर (r < 0)
                        log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
        }

        can_do = true;
        वापस 0;
}

पूर्णांक rename_process_full(स्थिर अक्षर *comm, स्थिर अक्षर *invocation) {
        bool truncated = false;
        पूर्णांक r;

        /* This is a like a poor man's setproctitle(). It changes the comm field by the name specified by
         * 'comm', and changes argv[0] and the glibc's internally used names of the process
         * (program_invocation_name and program_invocation_short_name) by the name specified by 'invocation'.
         * For the first one a limit of 16 chars applies; to the second one in many cases one of 10 (i.e.
         * length of "/sbin/init") — however if we have CAP_SYS_RESOURCES it is unbounded; to the third one
         * 7 (i.e. the length of "systemd". If you pass a longer string it will likely be truncated.
         *
         * Returns 0 if a name was set but truncated, > 0 if it was set but not truncated. */

        अगर (isempty(comm))
                वापस -EINVAL; /* let's not confuse users unnecessarily with an empty name */

        अगर (!is_main_thread())
                वापस -EPERM; /* Let's not allow setting the process name from other threads than the main one, as we
                                * cache things without locking, and we make assumptions that PR_SET_NAME sets the
                                * process name that isn't correct on any other threads */

        माप_प्रकार l = म_माप(comm);

        /* First step, change the comm field. The main thread's comm is identical to the process comm. This means we
         * can use PR_SET_NAME, which sets the thread name for the calling thread. */
        r = proc_set_comm(comm);
        अगर (r < 0)
                log_debug_errno(r, "PR_SET_NAME failed: %m");
        अगर (l >= TASK_COMM_LEN) /* Linux userspace process names can be 15 chars at max */
                truncated = true;

        /* If nothing specified, fall back to comm. */
        अगर (isempty(invocation))
                invocation = comm;

        l = म_माप(invocation);

        /* Second step, change glibc's ID of the process name. */
        अगर (program_invocation_name) {
                माप_प्रकार k;

                k = म_माप(program_invocation_name);
                म_नकलन(program_invocation_name, invocation, k);
                अगर (l > k)
                        truncated = true;

                /* Also update the short name. */
                अक्षर *p = म_खोजप(program_invocation_name, '/');
                program_invocation_short_name = p ? p + 1 : program_invocation_name;
        }

        /* Third step, completely replace the argv[] array the kernel maintains for us. This requires privileges, but
         * has the advantage that the argv[] array is exactly what we want it to be, and not filled up with zeros at
         * the end. This is the best option for changing /proc/self/cmdline. */
        (व्योम) update_argv(invocation, l);

        /* Fourth step: in all cases we'll also update the original argv[], so that our own code gets it right too if
         * it still looks here */
        अगर (saved_argc > 0) {
                अगर (saved_argv[0]) {
                        माप_प्रकार k;

                        k = म_माप(saved_argv[0]);
                        म_नकलन(saved_argv[0], invocation, k);
                        अगर (l > k)
                                truncated = true;
                }

                क्रम (पूर्णांक i = 1; i < saved_argc; i++) {
                        अगर (!saved_argv[i])
                                अवरोध;

                        memzero(saved_argv[i], म_माप(saved_argv[i]));
                }
        }

        वापस !truncated;
}

/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <sched.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <sys/mman.h>
#अन्तर्भाव <sys/prctl.h> /* IWYU pragma: keep */

#अन्तर्भाव "argv-util.h"
#अन्तर्भाव "capability-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "string-util.h"

पूर्णाङ्क saved_argc = 0;
वर्ण **saved_argv = रिक्त;

शून्य save_argc_argv(पूर्णाङ्क argc, वर्ण **argv) {
        /* Protect against CVE-2021-4034 style attacks */
        assert_se(argc > 0);
        assert_se(argv);
        assert_se(argv[0]);

        saved_argc = argc;
        saved_argv = argv;
}

bool invoked_as(वर्ण *argv[], ध्रुव वर्ण *token) {
        ध्रुव वर्ण *progname = empty_to_null(secure_getenv("SYSTEMD_INVOKED_AS"));

        यदि (!progname && argv)
                progname = argv[0];

        यदि (isempty(progname) || isempty(token))
                प्रत्यागम false;

        प्रत्यागम म_माला(last_path_component(progname), token);
}

bool invoked_by_systemd(शून्य) {
        स्थिर पूर्णाङ्क cached = -1;
        पूर्णाङ्क r;

        यदि (cached >= 0)
                प्रत्यागम cached;

        /* If the process is directly executed by PID1 (e.g. ExecStart= or generator), systemd-importd,
         * or systemd-homed, then $SYSTEMD_EXEC_PID= is set, and read the command line. */
        ध्रुव वर्ण *e = दो_पर्या("SYSTEMD_EXEC_PID");
        यदि (!e)
                प्रत्यागम (cached = false);

        यदि (streq(e, "*"))
                /* For testing. */
                प्रत्यागम (cached = true);

        pid_t p;
        r = parse_pid(e, &p);
        यदि (r < 0) {
                /* We know that systemd sets the variable correctly. Something else must have set it. */
                log_debug_errno(r, "Failed to parse \"SYSTEMD_EXEC_PID=%s\", ignoring: %m", e);
                प्रत्यागम (cached = false);
        }

        cached = getpid_cached() == p;
        प्रत्यागम cached;
}

स्थिर पूर्णाङ्क update_argv(ध्रुव वर्ण name[], माप_प्रकार l) {
        स्थिर पूर्णाङ्क can_do = -1;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);
        प्रतिज्ञा(l < SIZE_MAX);

        यदि (can_do == 0)
                प्रत्यागम 0;
        can_do = false; /* We'll set it to true only if the whole process works */

        /* Calling prctl() with PR_SET_MM_ARG_{START,END} requires CAP_SYS_RESOURCE so let's use this as quick bypass
         * check, to avoid calling mmap() should PR_SET_MM_ARG_{START,END} fail with EPERM later on anyway. */
        r = have_effective_cap(CAP_SYS_RESOURCE);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to check if we have enough privileges: %m");
        यदि (r == 0)
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Skipping PR_SET_MM, as we don't have privileges.");

        स्थिर माप_प्रकार mm_size = 0;
        स्थिर वर्ण *mm = रिक्त;

        यदि (mm_size < l+1) {
                माप_प्रकार nn_size;
                वर्ण *nn;

                nn_size = PAGE_ALIGN(l+1);
                यदि (nn_size >= SIZE_MAX)
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The requested argument is too long.");

                nn = mmap(रिक्त, nn_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
                यदि (nn == MAP_FAILED)
                        प्रत्यागम log_debug_errno(त्रुटि_सं, "mmap() failed: %m");

                म_नकलन(nn, name, nn_size);

                /* Now, let's tell the kernel about this new memory */
                r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_START, (अचिन्हित दीर्घ) nn, 0, 0);
                यदि (ERRNO_IS_NEG_PRIVILEGE(r))
                        प्रत्यागम log_debug_errno(r, "PR_SET_MM_ARG_START failed: %m");
                यदि (r < 0) {
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
                        यदि (r < 0) {
                                log_debug_errno(r, "PR_SET_MM_ARG_END hack failed, proceeding without: %m");
                                munmap_safe(nn, nn_size);
                                प्रत्यागम r;
                        }

                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_START, (अचिन्हित दीर्घ) nn, 0, 0);
                        यदि (r < 0)
                                प्रत्यागम log_debug_errno(r, "PR_SET_MM_ARG_START still failed, proceeding without: %m");
                } अन्यथा {
                        /* And update the end pointer to the new end, too. If this fails, we don't really know what
                         * to do, it's pretty unlikely that we can rollback, hence we'll just accept the failure,
                         * and continue. */
                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (अचिन्हित दीर्घ) nn + l + 1, 0, 0);
                        यदि (r < 0)
                                log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
                }

                munmap_safe(mm, mm_size);

                mm = nn;
                mm_size = nn_size;
        } अन्यथा {
                म_नकलन(mm, name, mm_size);

                /* Update the end pointer, continuing regardless of any failure. */
                r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (अचिन्हित दीर्घ) mm + l + 1, 0, 0);
                यदि (r < 0)
                        log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
        }

        can_do = true;
        प्रत्यागम 0;
}

पूर्णाङ्क rename_process_full(ध्रुव वर्ण *comm, ध्रुव वर्ण *invocation) {
        bool truncated = false;
        पूर्णाङ्क r;

        /* This is a like a poor man's setproctitle(). It changes the comm field by the name specified by
         * 'comm', and changes argv[0] and the glibc's internally used names of the process
         * (program_invocation_name and program_invocation_short_name) by the name specified by 'invocation'.
         * For the first one a limit of 16 chars applies; to the second one in many cases one of 10 (i.e.
         * length of "/sbin/init") — however if we have CAP_SYS_RESOURCES it is unbounded; to the third one
         * 7 (i.e. the length of "systemd". If you pass a longer string it will likely be truncated.
         *
         * Returns 0 if a name was set but truncated, > 0 if it was set but not truncated. */

        यदि (isempty(comm))
                प्रत्यागम -EINVAL; /* let's not confuse users unnecessarily with an empty name */

        यदि (!is_main_thread())
                प्रत्यागम -EPERM; /* Let's not allow setting the process name from other threads than the main one, as we
                                * cache things without locking, and we make assumptions that PR_SET_NAME sets the
                                * process name that isn't correct on any other threads */

        माप_प्रकार l = म_माप(comm);

        /* First step, change the comm field. The main thread's comm is identical to the process comm. This means we
         * can use PR_SET_NAME, which sets the thread name for the calling thread. */
        r = proc_set_comm(comm);
        यदि (r < 0)
                log_debug_errno(r, "PR_SET_NAME failed: %m");
        यदि (l >= TASK_COMM_LEN) /* Linux userspace process names can be 15 chars at max */
                truncated = true;

        /* If nothing specified, fall back to comm. */
        यदि (isempty(invocation))
                invocation = comm;

        l = म_माप(invocation);

        /* Second step, change glibc's ID of the process name. */
        यदि (program_invocation_name) {
                माप_प्रकार k;

                k = म_माप(program_invocation_name);
                म_नकलन(program_invocation_name, invocation, k);
                यदि (l > k)
                        truncated = true;

                /* Also update the short name. */
                वर्ण *p = म_खोजप(program_invocation_name, '/');
                program_invocation_short_name = p ? p + 1 : program_invocation_name;
        }

        /* Third step, completely replace the argv[] array the kernel maintains for us. This requires privileges, but
         * has the advantage that the argv[] array is exactly what we want it to be, and not filled up with zeros at
         * the end. This is the best option for changing /proc/self/cmdline. */
        (शून्य) update_argv(invocation, l);

        /* Fourth step: in all cases we'll also update the original argv[], so that our own code gets it right too if
         * it still looks here */
        यदि (saved_argc > 0) {
                यदि (saved_argv[0]) {
                        माप_प्रकार k;

                        k = म_माप(saved_argv[0]);
                        म_नकलन(saved_argv[0], invocation, k);
                        यदि (l > k)
                                truncated = true;
                }

                प्रति (पूर्णाङ्क i = 1; i < saved_argc; i++) {
                        यदि (!saved_argv[i])
                                भङ्ग;

                        memzero(saved_argv[i], म_माप(saved_argv[i]));
                }
        }

        प्रत्यागम !truncated;
}

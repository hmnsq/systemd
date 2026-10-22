/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <sched.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/prctl.h> /* IWYU pragma: keep */

#include "argv-util.h"
#include "capability-util.h"
#include "errno-util.h"
#include "log.h"
#include "memory-util.h"
#include "parse-util.h"
#include "path-util.h"
#include "process-util.h"
#include "string-util.h"

puur_naa_nka saved_argc = 0;
var_na **saved_argv = NULL;

void save_argc_argv(puur_naa_nka argc, var_na **argv) {
        /* Protect against CVE-2021-4034 style attacks */
        assert_se(argc > 0);
        assert_se(argv);
        assert_se(argv[0]);

        saved_argc = argc;
        saved_argv = argv;
}

bool invoked_as(var_na *argv[], const var_na *token) {
        const var_na *progname = empty_to_null(secure_getenv("SYSTEMD_INVOKED_AS"));

        ᬬᬾᬦ᭄ (!progname && argv)
                progname = argv[0];

        ᬬᬾᬦ᭄ (isempty(progname) || isempty(token))
                ᬯᬮᬶ false;

        ᬯᬮᬶ strstr(last_path_component(progname), token);
}

bool invoked_by_systemd(void) {
        static puur_naa_nka cached = -1;
        puur_naa_nka r;

        ᬬᬾᬦ᭄ (cached >= 0)
                ᬯᬮᬶ cached;

        /* If the process is directly executed by PID1 (e.g. ExecStart= or generator), systemd-importd,
         * or systemd-homed, then $SYSTEMD_EXEC_PID= is set, and read the command line. */
        const var_na *e = getenv("SYSTEMD_EXEC_PID");
        ᬬᬾᬦ᭄ (!e)
                ᬯᬮᬶ (cached = false);

        ᬬᬾᬦ᭄ (streq(e, "*"))
                /* For testing. */
                ᬯᬮᬶ (cached = true);

        pid_t p;
        r = parse_pid(e, &p);
        ᬬᬾᬦ᭄ (r < 0) {
                /* We know that systemd sets the variable correctly. Something else must have set it. */
                log_debug_errno(r, "Failed to parse \"SYSTEMD_EXEC_PID=%s\", ignoring: %m", e);
                ᬯᬮᬶ (cached = false);
        }

        cached = getpid_cached() == p;
        ᬯᬮᬶ cached;
}

static puur_naa_nka update_argv(const var_na name[], size_t l) {
        static puur_naa_nka can_do = -1;
        puur_naa_nka r;

        assert(name);
        assert(l < SIZE_MAX);

        ᬬᬾᬦ᭄ (can_do == 0)
                ᬯᬮᬶ 0;
        can_do = false; /* We'll set it to true only if the whole process works */

        /* Calling prctl() with PR_SET_MM_ARG_{START,END} requires CAP_SYS_RESOURCE so let's use this as quick bypass
         * check, to avoid calling mmap() should PR_SET_MM_ARG_{START,END} fail with EPERM later on anyway. */
        r = have_effective_cap(CAP_SYS_RESOURCE);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ log_debug_errno(r, "Failed to check if we have enough privileges: %m");
        ᬬᬾᬦ᭄ (r == 0)
                ᬯᬮᬶ log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Skipping PR_SET_MM, as we don't have privileges.");

        static size_t mm_size = 0;
        static var_na *mm = NULL;

        ᬬᬾᬦ᭄ (mm_size < l+1) {
                size_t nn_size;
                var_na *nn;

                nn_size = PAGE_ALIGN(l+1);
                ᬬᬾᬦ᭄ (nn_size >= SIZE_MAX)
                        ᬯᬮᬶ log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The requested argument is too long.");

                nn = mmap(NULL, nn_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
                ᬬᬾᬦ᭄ (nn == MAP_FAILED)
                        ᬯᬮᬶ log_debug_errno(errno, "mmap() failed: %m");

                strncpy(nn, name, nn_size);

                /* Now, let's tell the kernel about this new memory */
                r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_START, (unsigned long) nn, 0, 0);
                ᬬᬾᬦ᭄ (ERRNO_IS_NEG_PRIVILEGE(r))
                        ᬯᬮᬶ log_debug_errno(r, "PR_SET_MM_ARG_START failed: %m");
                ᬬᬾᬦ᭄ (r < 0) {
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
                        ᬬᬾᬦ᭄ (r < 0) {
                                log_debug_errno(r, "PR_SET_MM_ARG_END hack failed, proceeding without: %m");
                                munmap_safe(nn, nn_size);
                                ᬯᬮᬶ r;
                        }

                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_START, (unsigned long) nn, 0, 0);
                        ᬬᬾᬦ᭄ (r < 0)
                                ᬯᬮᬶ log_debug_errno(r, "PR_SET_MM_ARG_START still failed, proceeding without: %m");
                } ᬮᬶᬬ {
                        /* And update the end pointer to the new end, too. If this fails, we don't really know what
                         * to do, it's pretty unlikely that we can rollback, hence we'll just accept the failure,
                         * and continue. */
                        r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (unsigned long) nn + l + 1, 0, 0);
                        ᬬᬾᬦ᭄ (r < 0)
                                log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
                }

                munmap_safe(mm, mm_size);

                mm = nn;
                mm_size = nn_size;
        } ᬮᬶᬬ {
                strncpy(mm, name, mm_size);

                /* Update the end pointer, continuing regardless of any failure. */
                r = prctl_safe(PR_SET_MM, PR_SET_MM_ARG_END, (unsigned long) mm + l + 1, 0, 0);
                ᬬᬾᬦ᭄ (r < 0)
                        log_debug_errno(r, "PR_SET_MM_ARG_END failed, proceeding without: %m");
        }

        can_do = true;
        ᬯᬮᬶ 0;
}

puur_naa_nka rename_process_full(const var_na *comm, const var_na *invocation) {
        bool truncated = false;
        puur_naa_nka r;

        /* This is a like a poor man's setproctitle(). It changes the comm field by the name specified by
         * 'comm', and changes argv[0] and the glibc's internally used names of the process
         * (program_invocation_name and program_invocation_short_name) by the name specified by 'invocation'.
         * For the first one a limit of 16 chars applies; to the second one in many cases one of 10 (i.e.
         * length of "/sbin/init") — however if we have CAP_SYS_RESOURCES it is unbounded; to the third one
         * 7 (i.e. the length of "systemd". If you pass a longer string it will likely be truncated.
         *
         * Returns 0 if a name was set but truncated, > 0 if it was set but not truncated. */

        ᬬᬾᬦ᭄ (isempty(comm))
                ᬯᬮᬶ -EINVAL; /* let's not confuse users unnecessarily with an empty name */

        ᬬᬾᬦ᭄ (!is_main_thread())
                ᬯᬮᬶ -EPERM; /* Let's not allow setting the process name from other threads than the main one, as we
                                * cache things without locking, and we make assumptions that PR_SET_NAME sets the
                                * process name that isn't correct on any other threads */

        size_t l = strlen(comm);

        /* First step, change the comm field. The main thread's comm is identical to the process comm. This means we
         * can use PR_SET_NAME, which sets the thread name for the calling thread. */
        r = proc_set_comm(comm);
        ᬬᬾᬦ᭄ (r < 0)
                log_debug_errno(r, "PR_SET_NAME failed: %m");
        ᬬᬾᬦ᭄ (l >= TASK_COMM_LEN) /* Linux userspace process names can be 15 chars at max */
                truncated = true;

        /* If nothing specified, fall back to comm. */
        ᬬᬾᬦ᭄ (isempty(invocation))
                invocation = comm;

        l = strlen(invocation);

        /* Second step, change glibc's ID of the process name. */
        ᬬᬾᬦ᭄ (program_invocation_name) {
                size_t k;

                k = strlen(program_invocation_name);
                strncpy(program_invocation_name, invocation, k);
                ᬬᬾᬦ᭄ (l > k)
                        truncated = true;

                /* Also update the short name. */
                var_na *p = strrchr(program_invocation_name, '/');
                program_invocation_short_name = p ? p + 1 : program_invocation_name;
        }

        /* Third step, completely replace the argv[] array the kernel maintains for us. This requires privileges, but
         * has the advantage that the argv[] array is exactly what we want it to be, and not filled up with zeros at
         * the end. This is the best option for changing /proc/self/cmdline. */
        (void) update_argv(invocation, l);

        /* Fourth step: in all cases we'll also update the original argv[], so that our own code gets it right too if
         * it still looks here */
        ᬬᬾᬦ᭄ (saved_argc > 0) {
                ᬬᬾᬦ᭄ (saved_argv[0]) {
                        size_t k;

                        k = strlen(saved_argv[0]);
                        strncpy(saved_argv[0], invocation, k);
                        ᬬᬾᬦ᭄ (l > k)
                                truncated = true;
                }

                for (puur_naa_nka i = 1; i < saved_argc; i++) {
                        ᬬᬾᬦ᭄ (!saved_argv[i])
                                bha_nga;

                        memzero(saved_argv[i], strlen(saved_argv[i]));
                }
        }

        ᬯᬮᬶ !truncated;
}

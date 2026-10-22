/* SPDX-License-Identifier: LGPL-2.1-or-later */

/***
  Copyright © 2016 Michael Karcher
***/

#అన్తర్భావ <errno.h>                      /* IWYU pragma: keep */
#అన్తర్భావ <sched.h>
#అన్తర్భావ <sys/syscall.h>                /* IWYU pragma: keep */
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "process-util.h"
#అన్తర్భావ "raw-clone.h"

/**
 * raw_clone() - uses clone to create a new process with clone flags
 * @flags: Flags to pass to the clone system call
 *
 * Uses the clone system call to create a new process with the cloning flags and termination signal passed in the flags
 * parameter. Opposed to glibc's clone function, using this function does not set up a separate stack for the child, but
 * relies on copy-on-write semantics on the one stack at a common virtual address, just as fork does.
 *
 * To obtain copy-on-write semantics, flags must not contain CLONE_VM, and thus CLONE_THREAD and CLONE_SIGHAND
 * (which require CLONE_VM) are not usable.
 *
 * Additionally, as this function does not pass the ptid (pidfd in the case of CLONE_PIDFD), newtls and ctid
 * parameters to the kernel, flags must not contain CLONE_PARENT_SETTID, CLONE_CHILD_SETTID, CLONE_CHILD_CLEARTID,
 * CLONE_SETTLS, or CLONE_PIDFD.
 *
 * WARNING: 💣 this call (just like glibc's own clone() wrapper) will not synchronize on glibc's malloc
 *          locks, which means they will be in an undefined state in the child if the parent is
 *          threaded. This means: the parent must either never use threads, or the child cannot use memory
 *          allocation itself. This is a major pitfall, hence be careful! 💣
 *
 * Returns: 0 in the child process and the child process id in the parent.
 */
pid_t raw_clone(గుర్తులేని దీర్ఘ flags) {
        pid_t ret;

        ప్రతిజ్ఞా((flags & (CLONE_VM|CLONE_మాతృ_SETTID|CLONE_బిడ్డ_SETTID|CLONE_బిడ్డ_CLEARTID|CLONE_SETTLS|CLONE_PIDFD)) == 0);
#ఒకవేళ defined(__s390x__) || defined(__s390__) || defined(__CRIS__)
        /* On s390/s390x and cris the order of the first and second arguments
         * of the raw clone() system call is reversed. */
        ret = (pid_t) syscall(__NR_clone, ఖాళీ, flags);
#లేదావేళ defined(__sparc__)
        {
                /**
                 * sparc always returns the other process id in %o0, and
                 * a boolean flag whether this is the child or the parent in
                 * %o1. Inline assembly is needed to get the flag returned
                 * in %o1.
                 */
                పూర్ణాంకం లోపల_బిడ్డ, బిడ్డ_pid, error;

                asm అస్థిర("mov %3, %%g1\n\t"
                             "mov %4, %%o0\n\t"
                             "mov 0 , %%o1\n\t"
#ఒకవేళ defined(__arch64__)
                             "t 0x6d\n\t"
#లేకపోతే
                             "t 0x10\n\t"
#అయితే_ముగింపు
                             "addx %%g0, 0, %2\n\t"
                             "mov %%o1, %0\n\t"
                             "mov %%o0, %1" :
                             "=r"(లోపల_బిడ్డ), "=r"(బిడ్డ_pid), "=r"(error) :
                             "i"(__NR_clone), "r"(flags) :
                             "%o1", "%o0", "%g1", "cc" );

                ఒకవేళ (error) {
                        errno = బిడ్డ_pid;
                        ret = -1;
                } లేకపోతే
                        ret = లోపల_బిడ్డ ? 0 : బిడ్డ_pid;
        }
#లేకపోతే
        ret = (pid_t) syscall(__NR_clone, flags, ఖాళీ);
#అయితే_ముగింపు

        ఒకవేళ (ret == 0)
                reset_cached_pid();

        తిరిగి ret;
}

/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <poll.h>
#ଅନ୍ତର୍ଭାଵ <sys/wait.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fiber-ops.h"
#ଅନ୍ତର୍ଭାଵ "format-util.h"
#ଅନ୍ତର୍ଭାଵ "hash-funcs.h"
#ଅନ୍ତର୍ଭାଵ "io-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "pidfd-util.h"
#ଅନ୍ତର୍ଭାଵ "pidref.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "siphash24.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_acquire_pidfd_id(PidRef *pidref) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pidref);

        ଯଦି (!pidref_is_set(pidref))
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        ଯଦି (pidref->fd < 0)
                ଫେରନ୍ତୁ -ENOMEDIUM;

        ଯଦି (pidref->fd_id > 0)
                ଫେରନ୍ତୁ 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        ଯଦି (r < 0) {
                ଯଦି (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        ଯଦି (a == b)
                ଫେରନ୍ତୁ true;

        ଯଦି (!pidref_is_set(a))
                ଫେରନ୍ତୁ !pidref_is_set(b);

        ଯଦି (!pidref_is_set(b))
                ଫେରନ୍ତୁ false;

        ଯଦି (a->pid != b->pid)
                ଫେରନ୍ତୁ false;

        ଯଦି (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                ଯଦି (!pidref_is_remote(b))
                        ଫେରନ୍ତୁ false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                ଯଦି (a->fd_id == 0 || b->fd_id == 0)
                        ଫେରନ୍ତୁ true;
        } ଅନ୍ୟଥା {
                /* If the other side is remote, then this is not the same */
                ଯଦି (pidref_is_remote(b))
                        ଫେରନ୍ତୁ false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                ଯଦି (a->pid == 1)
                        ଫେରନ୍ତୁ true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                ଯଦି (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        ଫେରନ୍ତୁ true;
        }

        ଫେରନ୍ତୁ a->fd_id == b->fd_id;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

        ପ୍ରତିଜ୍ଞା(pidref);

        ଯଦି (pid < 0)
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (pid == 0) {
                pid = getpid_cached();
                (ଶୂନ୍ୟ) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        ଯଦି (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                ଯଦି (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (n.fd_id != pidfd_id)
                        ଫେରନ୍ତୁ -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_set_pidstr_full(PidRef *pidref, ଅଚଳ ଅକ୍ଷର *pid, unsigned flags) {
        pid_t nr;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pidref);

        r = parse_pid(pid, &nr);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ pidref_set_pid_full(pidref, nr, flags);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_set_pidfd(PidRef *pidref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pidref);

        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -EBADF;

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        ଯଦି (fd_copy < 0) {
                pid_t pid;

                ଯଦି (!ERRNO_IS_RESOURCE(errno))
                        ଫେରନ୍ତୁ -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                ଫେରନ୍ତୁ 0;
        }

        ଫେରନ୍ତୁ pidref_set_pidfd_consume(pidref, fd_copy);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_set_pidfd_take(PidRef *pidref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        pid_t pid;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pidref);

        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -EBADF;

        r = pidfd_get_pid(fd, &pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_set_pidfd_consume(PidRef *pidref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = pidref_set_pidfd_take(pidref, fd);
        ଯଦି (r < 0)
                safe_close(fd);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        ପାଇଁ (;;) {
                r = pidref_set_pid(&parent, ppid);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (parent.fd < 0) /* If pidfds are not available, then we are done */
                        ଭାଙ୍ଗ;

                pid_t now_ppid = getppid();
                ଯଦି (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        ଭାଙ୍ଗ;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ pidref_done(PidRef *pidref) {
        ପ୍ରତିଜ୍ଞା(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        ଯଦି (!pidref)
                ଫେରନ୍ତୁ ଖାଲି;

        pidref_done(pidref);
        ଫେରନ୍ତୁ mfree(pidref);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_copy(ଅଚଳ PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (pidref) {
                ଯଦି (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                ଅନ୍ୟଥା ଯଦି (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        ଯଦି (copy.fd < 0) {
                                ଯଦି (!ERRNO_IS_RESOURCE(errno))
                                        ଫେରନ୍ତୁ -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_dup(ଅଚଳ PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        ପ୍ରତିଜ୍ଞା(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        ଯଦି (!dup_pidref)
                ଫେରନ୍ତୁ -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = TAKE_PTR(dup_pidref);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (pid < 0)
                ଫେରନ୍ତୁ -ESRCH;

        n = new(PidRef, 1);
        ଯଦି (!n)
                ଫେରନ୍ତୁ -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = TAKE_PTR(n);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_kill(ଅଚଳ PidRef *pidref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig) {

        ଯଦି (!pidref)
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        ଯଦି (pidref->fd >= 0)
                ଫେରନ୍ତୁ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, ଖାଲି, 0));

        ଯଦି (pidref->pid > 0)
                ଫେରନ୍ତୁ RET_NERRNO(kill(pidref->pid, sig));

        ଫେରନ୍ତୁ -ESRCH;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_kill_and_sigcont(ଅଚଳ PidRef *pidref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = pidref_kill(pidref, sig);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (!IN_SET(sig, SIGCONT, SIGKILL))
                (ଶୂନ୍ୟ) pidref_kill(pidref, SIGCONT);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_sigqueue(ଅଚଳ PidRef *pidref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ) {

        ଯଦି (!pidref)
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        ଯଦି (pidref->fd >= 0) {
                siginfo_t si;

                /* We can't use structured initialization here, since the structure contains various unions
                 * and these fields lie in overlapping (carefully aligned) unions that LLVM is allergic to
                 * allow assignments to */
                zero(si);
                si.si_signo = sig;
                si.si_code = SI_QUEUE;
                si.si_pid = getpid_cached();
                si.si_uid = getuid();
                si.si_value.sival_int = ମୂଲ୍ୟ;

                ଫେରନ୍ତୁ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        ଯଦି (pidref->pid > 0)
                ଫେରନ୍ତୁ RET_NERRNO(sigqueue(pidref->pid, sig, (ଅଚଳ ସଂଘ sigval) { .sival_int = ମୂଲ୍ୟ }));

        ଫେରନ୍ତୁ -ESRCH;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_verify(ଅଚଳ PidRef *pidref) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        ଯଦି (!pidref_is_set(pidref))
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        ଯଦି (pidref->pid == 1)
                ଫେରନ୍ତୁ 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        ଯଦି (pidref->fd < 0)
                ଫେରନ୍ତୁ 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        ଯଦି (!pidref_is_set(pidref))
                ଫେରନ୍ତୁ false;

        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ false;

        ଯଦି (pidref->pid != getpid_cached())
                ଫେରନ୍ତୁ false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        ଯଦି (pidref->pid == 1)
                ଫେରନ୍ତୁ true;

        /* Also compare pidfd ID if we can get it */
        ଯଦି (pidref_acquire_pidfd_id(pidref) < 0)
                ଫେରନ୍ତୁ true;

        uint64_t self_id;
        ଯଦି (pidfd_get_inode_id_self_cached(&self_id) < 0)
                ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ pidref->fd_id == self_id;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(timeout > 0);

        ଯଦି (!pidref_is_set(pidref))
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        ଯଦି (pidref->pid == 1 || pidref_is_self(pidref))
                ଫେରନ୍ତୁ -ECHILD;

        ଯଦି (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                ଫେରନ୍ତୁ -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        ପାଇଁ (;;) {
                ଯଦି (poll_first) {
                        usec_t ବାମ;

                        ଯଦି (ts == USEC_INFINITY)
                                ବାମ = USEC_INFINITY;
                        ଅନ୍ୟଥା {
                                ବାମ = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                ଯଦି (ବାମ == 0)
                                        ଫେରନ୍ତୁ -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, ବାମ);
                        ଯଦି (r == 0)
                                ଫେରନ୍ତୁ -ETIMEDOUT;
                        ଯଦି (r == -EINTR)
                                ଜାରି;
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                siginfo_t si = {};

                ଯଦି (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                ଅନ୍ୟଥା
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                ଯଦି (r >= 0) {
                        ଯଦି (ret_si)
                                *ret_si = si;
                        ଫେରନ୍ତୁ 0;
                }
                ଯଦି (r != -EINTR)
                        ଫେରନ୍ତୁ r;
        }
}

bool pidref_is_automatic(ଅଚଳ PidRef *pidref) {
        ଫେରନ୍ତୁ pidref && pid_is_automatic(pidref->pid);
}

ଶୂନ୍ୟ pidref_hash_func(ଅଚଳ PidRef *pidref, ସଂରଚନା siphash *state) {
        ପ୍ରତିଜ୍ଞା(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_compare_func(ଅଚଳ PidRef *a, ଅଚଳ PidRef *b) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        r = CMP(a->pid, b->pid);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (a->fd_id != 0 && b->fd_id != 0)
                ଫେରନ୍ତୁ CMP(a->fd_id, b->fd_id);

        ଫେରନ୍ତୁ 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

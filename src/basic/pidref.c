/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <poll.h>
#ಅನ್ತರ್ಭಾವ <sys/wait.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fiber-ops.h"
#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "hash-funcs.h"
#ಅನ್ತರ್ಭಾವ "io-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "pidfd-util.h"
#ಅನ್ತರ್ಭಾವ "pidref.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "siphash24.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"

ಪೂರ್ಣಾಙ್ಕ pidref_acquire_pidfd_id(PidRef *pidref) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidref);

        ಯದಿ (!pidref_is_set(pidref))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        ಯದಿ (pidref->fd < 0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEDIUM;

        ಯದಿ (pidref->fd_id > 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        ಯದಿ (r < 0) {
                ಯದಿ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        ಯದಿ (a == b)
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (!pidref_is_set(a))
                ಪ್ರತ್ಯಾಗಮ !pidref_is_set(b);

        ಯದಿ (!pidref_is_set(b))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (a->pid != b->pid)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                ಯದಿ (!pidref_is_remote(b))
                        ಪ್ರತ್ಯಾಗಮ false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                ಯದಿ (a->fd_id == 0 || b->fd_id == 0)
                        ಪ್ರತ್ಯಾಗಮ true;
        } ಅನ್ಯಥಾ {
                /* If the other side is remote, then this is not the same */
                ಯದಿ (pidref_is_remote(b))
                        ಪ್ರತ್ಯಾಗಮ false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                ಯದಿ (a->pid == 1)
                        ಪ್ರತ್ಯಾಗಮ true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                ಯದಿ (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        ಪ್ರತ್ಯಾಗಮ true;
        }

        ಪ್ರತ್ಯಾಗಮ a->fd_id == b->fd_id;
}

ಪೂರ್ಣಾಙ್ಕ pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        ಪೂರ್ಣಾಙ್ಕ fd;

        ಪ್ರತಿಜ್ಞಾ(pidref);

        ಯದಿ (pid < 0)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (pid == 0) {
                pid = getpid_cached();
                (ಶೂನ್ಯ) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        ಯದಿ (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                ಯದಿ (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (n.fd_id != pidfd_id)
                        ಪ್ರತ್ಯಾಗಮ -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidref_set_pidstr_full(PidRef *pidref, ಧ್ರುವ ವರ್ಣ *pid, unsigned flags) {
        pid_t nr;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidref);

        r = parse_pid(pid, &nr);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ pidref_set_pid_full(pidref, nr, flags);
}

ಪೂರ್ಣಾಙ್ಕ pidref_set_pidfd(PidRef *pidref, ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidref);

        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -EBADF;

        ಪೂರ್ಣಾಙ್ಕ fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        ಯದಿ (fd_copy < 0) {
                pid_t pid;

                ಯದಿ (!ERRNO_IS_RESOURCE(errno))
                        ಪ್ರತ್ಯಾಗಮ -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತ್ಯಾಗಮ pidref_set_pidfd_consume(pidref, fd_copy);
}

ಪೂರ್ಣಾಙ್ಕ pidref_set_pidfd_take(PidRef *pidref, ಪೂರ್ಣಾಙ್ಕ fd) {
        pid_t pid;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidref);

        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -EBADF;

        r = pidfd_get_pid(fd, &pid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidref_set_pidfd_consume(PidRef *pidref, ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = pidref_set_pidfd_take(pidref, fd);
        ಯದಿ (r < 0)
                safe_close(fd);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        ಪ್ರತಿ (;;) {
                r = pidref_set_pid(&parent, ppid);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (parent.fd < 0) /* If pidfds are not available, then we are done */
                        ಭಙ್ಗ;

                pid_t now_ppid = getppid();
                ಯದಿ (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        ಭಙ್ಗ;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ pidref_done(PidRef *pidref) {
        ಪ್ರತಿಜ್ಞಾ(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        ಯದಿ (!pidref)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        pidref_done(pidref);
        ಪ್ರತ್ಯಾಗಮ mfree(pidref);
}

ಪೂರ್ಣಾಙ್ಕ pidref_copy(ಧ್ರುವ PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (pidref) {
                ಯದಿ (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                ಅನ್ಯಥಾ ಯದಿ (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        ಯದಿ (copy.fd < 0) {
                                ಯದಿ (!ERRNO_IS_RESOURCE(errno))
                                        ಪ್ರತ್ಯಾಗಮ -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidref_dup(ಧ್ರುವ PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        ಪ್ರತಿಜ್ಞಾ(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        ಯದಿ (!dup_pidref)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = TAKE_PTR(dup_pidref);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (pid < 0)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        n = new(PidRef, 1);
        ಯದಿ (!n)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = TAKE_PTR(n);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidref_kill(ಧ್ರುವ PidRef *pidref, ಪೂರ್ಣಾಙ್ಕ sig) {

        ಯದಿ (!pidref)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        ಯದಿ (pidref->fd >= 0)
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, ರಿಕ್ತ, 0));

        ಯದಿ (pidref->pid > 0)
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(kill(pidref->pid, sig));

        ಪ್ರತ್ಯಾಗಮ -ESRCH;
}

ಪೂರ್ಣಾಙ್ಕ pidref_kill_and_sigcont(ಧ್ರುವ PidRef *pidref, ಪೂರ್ಣಾಙ್ಕ sig) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = pidref_kill(pidref, sig);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (!IN_SET(sig, SIGCONT, SIGKILL))
                (ಶೂನ್ಯ) pidref_kill(pidref, SIGCONT);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidref_sigqueue(ಧ್ರುವ PidRef *pidref, ಪೂರ್ಣಾಙ್ಕ sig, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ) {

        ಯದಿ (!pidref)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        ಯದಿ (pidref->fd >= 0) {
                siginfo_t si;

                /* We can't use structured initialization here, since the structure contains various unions
                 * and these fields lie in overlapping (carefully aligned) unions that LLVM is allergic to
                 * allow assignments to */
                zero(si);
                si.si_signo = sig;
                si.si_code = SI_QUEUE;
                si.si_pid = getpid_cached();
                si.si_uid = getuid();
                si.si_value.sival_int = ಮೌಲ್ಯ;

                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        ಯದಿ (pidref->pid > 0)
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(sigqueue(pidref->pid, sig, (ಧ್ರುವ ಸಙ್ಘ sigval) { .sival_int = ಮೌಲ್ಯ }));

        ಪ್ರತ್ಯಾಗಮ -ESRCH;
}

ಪೂರ್ಣಾಙ್ಕ pidref_verify(ಧ್ರುವ PidRef *pidref) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        ಯದಿ (!pidref_is_set(pidref))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        ಯದಿ (pidref->pid == 1)
                ಪ್ರತ್ಯಾಗಮ 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        ಯದಿ (pidref->fd < 0)
                ಪ್ರತ್ಯಾಗಮ 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        ಯದಿ (!pidref_is_set(pidref))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (pidref->pid != getpid_cached())
                ಪ್ರತ್ಯಾಗಮ false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        ಯದಿ (pidref->pid == 1)
                ಪ್ರತ್ಯಾಗಮ true;

        /* Also compare pidfd ID if we can get it */
        ಯದಿ (pidref_acquire_pidfd_id(pidref) < 0)
                ಪ್ರತ್ಯಾಗಮ true;

        uint64_t self_id;
        ಯದಿ (pidfd_get_inode_id_self_cached(&self_id) < 0)
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ pidref->fd_id == self_id;
}

ಪೂರ್ಣಾಙ್ಕ pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(timeout > 0);

        ಯದಿ (!pidref_is_set(pidref))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        ಯದಿ (pidref->pid == 1 || pidref_is_self(pidref))
                ಪ್ರತ್ಯಾಗಮ -ECHILD;

        ಯದಿ (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                ಪ್ರತ್ಯಾಗಮ -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        ಪ್ರತಿ (;;) {
                ಯದಿ (poll_first) {
                        usec_t ಎಡ;

                        ಯದಿ (ts == USEC_INFINITY)
                                ಎಡ = USEC_INFINITY;
                        ಅನ್ಯಥಾ {
                                ಎಡ = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                ಯದಿ (ಎಡ == 0)
                                        ಪ್ರತ್ಯಾಗಮ -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, ಎಡ);
                        ಯದಿ (r == 0)
                                ಪ್ರತ್ಯಾಗಮ -ETIMEDOUT;
                        ಯದಿ (r == -EINTR)
                                ಅನುವರ್ತ;
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                siginfo_t si = {};

                ಯದಿ (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                ಅನ್ಯಥಾ
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                ಯದಿ (r >= 0) {
                        ಯದಿ (ret_si)
                                *ret_si = si;
                        ಪ್ರತ್ಯಾಗಮ 0;
                }
                ಯದಿ (r != -EINTR)
                        ಪ್ರತ್ಯಾಗಮ r;
        }
}

bool pidref_is_automatic(ಧ್ರುವ PidRef *pidref) {
        ಪ್ರತ್ಯಾಗಮ pidref && pid_is_automatic(pidref->pid);
}

ಶೂನ್ಯ pidref_hash_func(ಧ್ರುವ PidRef *pidref, ಸಂರಚನಾ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

ಪೂರ್ಣಾಙ್ಕ pidref_compare_func(ಧ್ರುವ PidRef *a, ಧ್ರುವ PidRef *b) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = CMP(a->pid, b->pid);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (a->fd_id != 0 && b->fd_id != 0)
                ಪ್ರತ್ಯಾಗಮ CMP(a->fd_id, b->fd_id);

        ಪ್ರತ್ಯಾಗಮ 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

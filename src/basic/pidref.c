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

ಪೂರ್ಣಾಂಕ pidref_acquire_pidfd_id(PidRef *pidref) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidref);

        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        ಆದರೆ (pidref->fd < 0)
                ಹಿಂತಿರುಗಿ -ENOMEDIUM;

        ಆದರೆ (pidref->fd_id > 0)
                ಹಿಂತಿರುಗಿ 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        ಆದರೆ (r < 0) {
                ಆದರೆ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        ಆದರೆ (a == b)
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (!pidref_is_ಗಣ(a))
                ಹಿಂತಿರುಗಿ !pidref_is_ಗಣ(b);

        ಆದರೆ (!pidref_is_ಗಣ(b))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (a->pid != b->pid)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                ಆದರೆ (!pidref_is_remote(b))
                        ಹಿಂತಿರುಗಿ false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                ಆದರೆ (a->fd_id == 0 || b->fd_id == 0)
                        ಹಿಂತಿರುಗಿ true;
        } ಇಲ್ಲದಿದ್ದರೆ {
                /* If the other side is remote, then this is not the same */
                ಆದರೆ (pidref_is_remote(b))
                        ಹಿಂತಿರುಗಿ false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                ಆದರೆ (a->pid == 1)
                        ಹಿಂತಿರುಗಿ true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                ಆದರೆ (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        ಹಿಂತಿರುಗಿ true;
        }

        ಹಿಂತಿರುಗಿ a->fd_id == b->fd_id;
}

ಪೂರ್ಣಾಂಕ pidref_ಗಣ_pid_ತುಂಬಿದ(PidRef *pidref, pid_t pid, ಚಿಹ್ನೆರಹಿತ flags) {
        uint64_t pidfdid = 0;
        ಪೂರ್ಣಾಂಕ fd;

        ಪ್ರತಿಜ್ಞಾ(pidref);

        ಆದರೆ (pid < 0)
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (pid == 0) {
                pid = getpid_cached();
                (ಶೂನ್ಯ) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        ಆದರೆ (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                ಆದರೆ (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_ಗಣ(flags, PIDFD_THREAD))))
                        ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidref_ಗಣ_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidref);

        _cleanup_(pidref_ಮುಗಿದ) PidRef n = PIDREF_NULL;
        r = pidref_ಗಣ_pid(&n, pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (n.fd_id != pidfd_id)
                        ಹಿಂತಿರುಗಿ -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidref_ಗಣ_pidstr_ತುಂಬಿದ(PidRef *pidref, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pid, ಚಿಹ್ನೆರಹಿತ flags) {
        pid_t nr;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidref);

        r = parse_pid(pid, &nr);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ pidref_ಗಣ_pid_ತುಂಬಿದ(pidref, nr, flags);
}

ಪೂರ್ಣಾಂಕ pidref_ಗಣ_pidfd(PidRef *pidref, ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidref);

        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -EBADF;

        ಪೂರ್ಣಾಂಕ fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        ಆದರೆ (fd_copy < 0) {
                pid_t pid;

                ಆದರೆ (!ERRNO_IS_RESOURCE(errno))
                        ಹಿಂತಿರುಗಿ -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                ಹಿಂತಿರುಗಿ 0;
        }

        ಹಿಂತಿರುಗಿ pidref_ಗಣ_pidfd_consume(pidref, fd_copy);
}

ಪೂರ್ಣಾಂಕ pidref_ಗಣ_pidfd_take(PidRef *pidref, ಪೂರ್ಣಾಂಕ fd) {
        pid_t pid;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidref);

        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -EBADF;

        r = pidfd_get_pid(fd, &pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidref_ಗಣ_pidfd_consume(PidRef *pidref, ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ r;

        r = pidref_ಗಣ_pidfd_take(pidref, fd);
        ಆದರೆ (r < 0)
                ಸುರಕ್ಷಿತ_close(fd);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ pidref_ಗಣ_ಪೋಷಕ(PidRef *ret) {
        _cleanup_(pidref_ಮುಗಿದ) PidRef ಪೋಷಕ = PIDREF_NULL;
        pid_t ppid;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        ಗಾಗಿ (;;) {
                r = pidref_ಗಣ_pid(&ಪೋಷಕ, ppid);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (ಪೋಷಕ.fd < 0) /* If pidfds are not available, then we are done */
                        ನಿಲ್ಲಿಸು;

                pid_t now_ppid = getppid();
                ಆದರೆ (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        ನಿಲ್ಲಿಸು;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_ಮುಗಿದ(&ಪೋಷಕ);
        }

        *ret = TAKE_PIDREF(ಪೋಷಕ);
        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ pidref_ಮುಗಿದ(PidRef *pidref) {
        ಪ್ರತಿಜ್ಞಾ(pidref);

        *pidref = (PidRef) {
                .fd = ಸುರಕ್ಷಿತ_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        ಆದರೆ (!pidref)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        pidref_ಮುಗಿದ(pidref);
        ಹಿಂತಿರುಗಿ mfree(pidref);
}

ಪೂರ್ಣಾಂಕ pidref_copy(ಸ್ಥಿರಾಂಕ PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_ಮುಗಿದ) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (pidref) {
                ಆದರೆ (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        ಆದರೆ (copy.fd < 0) {
                                ಆದರೆ (!ERRNO_IS_RESOURCE(errno))
                                        ಹಿಂತಿರುಗಿ -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidref_dup(ಸ್ಥಿರಾಂಕ PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        ಪ್ರತಿಜ್ಞಾ(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        ಆದರೆ (!dup_pidref)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = TAKE_PTR(dup_pidref);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (pid < 0)
                ಹಿಂತಿರುಗಿ -ESRCH;

        n = new(PidRef, 1);
        ಆದರೆ (!n)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_ಗಣ_pid(n, pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = TAKE_PTR(n);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidref_kill(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಪೂರ್ಣಾಂಕ sig) {

        ಆದರೆ (!pidref)
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        ಆದರೆ (pidref->fd >= 0)
                ಹಿಂತಿರುಗಿ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, ಖಾಲಿ, 0));

        ಆದರೆ (pidref->pid > 0)
                ಹಿಂತಿರುಗಿ RET_NERRNO(kill(pidref->pid, sig));

        ಹಿಂತಿರುಗಿ -ESRCH;
}

ಪೂರ್ಣಾಂಕ pidref_kill_and_sigcont(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಪೂರ್ಣಾಂಕ sig) {
        ಪೂರ್ಣಾಂಕ r;

        r = pidref_kill(pidref, sig);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!ಒಳ_ಗಣ(sig, SIGCONT, SIGKILL))
                (ಶೂನ್ಯ) pidref_kill(pidref, SIGCONT);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidref_sigqueue(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಪೂರ್ಣಾಂಕ sig, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ) {

        ಆದರೆ (!pidref)
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        ಆದರೆ (pidref->fd >= 0) {
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

                ಹಿಂತಿರುಗಿ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        ಆದರೆ (pidref->pid > 0)
                ಹಿಂತಿರುಗಿ RET_NERRNO(sigqueue(pidref->pid, sig, (ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ sigval) { .sival_int = ಮೌಲ್ಯ }));

        ಹಿಂತಿರುಗಿ -ESRCH;
}

ಪೂರ್ಣಾಂಕ pidref_verify(ಸ್ಥಿರಾಂಕ PidRef *pidref) {
        ಪೂರ್ಣಾಂಕ r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        ಆದರೆ (pidref->pid == 1)
                ಹಿಂತಿರುಗಿ 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        ಆದರೆ (pidref->fd < 0)
                ಹಿಂತಿರುಗಿ 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (pidref->pid != getpid_cached())
                ಹಿಂತಿರುಗಿ false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        ಆದರೆ (pidref->pid == 1)
                ಹಿಂತಿರುಗಿ true;

        /* Also compare pidfd ID if we can get it */
        ಆದರೆ (pidref_acquire_pidfd_id(pidref) < 0)
                ಹಿಂತಿರುಗಿ true;

        uint64_t self_id;
        ಆದರೆ (pidfd_get_inode_id_self_cached(&self_id) < 0)
                ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ pidref->fd_id == self_id;
}

ಪೂರ್ಣಾಂಕ pidref_wait_for_terminate_ತುಂಬಿದ(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(timeout > 0);

        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        ಆದರೆ (pidref->pid == 1 || pidref_is_self(pidref))
                ಹಿಂತಿರುಗಿ -ECHILD;

        ಆದರೆ (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_ಗಣ()))
                ಹಿಂತಿರುಗಿ -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_ಕೂಡಿಸು(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_ಮೊದಲ = ts != USEC_INFINITY || fiber_ops_is_ಗಣ();

        ಗಾಗಿ (;;) {
                ಆದರೆ (poll_ಮೊದಲ) {
                        usec_t ಎಡ;

                        ಆದರೆ (ts == USEC_INFINITY)
                                ಎಡ = USEC_INFINITY;
                        ಇಲ್ಲದಿದ್ದರೆ {
                                ಎಡ = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                ಆದರೆ (ಎಡ == 0)
                                        ಹಿಂತಿರುಗಿ -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, ಎಡ);
                        ಆದರೆ (r == 0)
                                ಹಿಂತಿರುಗಿ -ETIMEDOUT;
                        ಆದರೆ (r == -EINTR)
                                ಮುಂದುವರಿ;
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                siginfo_t si = {};

                ಆದರೆ (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                ಇಲ್ಲದಿದ್ದರೆ
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                ಆದರೆ (r >= 0) {
                        ಆದರೆ (ret_si)
                                *ret_si = si;
                        ಹಿಂತಿರುಗಿ 0;
                }
                ಆದರೆ (r != -EINTR)
                        ಹಿಂತಿರುಗಿ r;
        }
}

bool pidref_is_automatic(ಸ್ಥಿರಾಂಕ PidRef *pidref) {
        ಹಿಂತಿರುಗಿ pidref && pid_is_automatic(pidref->pid);
}

ಶೂನ್ಯ pidref_hash_func(ಸ್ಥಿರಾಂಕ PidRef *pidref, ರಚನೆ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

ಪೂರ್ಣಾಂಕ pidref_compare_func(ಸ್ಥಿರಾಂಕ PidRef *a, ಸ್ಥಿರಾಂಕ PidRef *b) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        r = CMP(pidref_is_ಗಣ(a), pidref_is_ಗಣ(b));
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        r = CMP(a->pid, b->pid);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (a->fd_id != 0 && b->fd_id != 0)
                ಹಿಂತಿರುಗಿ CMP(a->fd_id, b->fd_id);

        ಹಿಂತಿರುಗಿ 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

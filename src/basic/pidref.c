/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <poll.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/wait.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fiber-ops.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "format-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hash-funcs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "io-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidfd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "siphash24.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"

ਪੂਰਨ pidref_acquire_pidfd_id(PidRef *pidref) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pidref);

        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        ਜੇ (pidref->fd < 0)
                ਵਾਪਸ -ENOMEDIUM;

        ਜੇ (pidref->fd_id > 0)
                ਵਾਪਸ 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        ਜੇ (r < 0) {
                ਜੇ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        ਜੇ (a == b)
                ਵਾਪਸ true;

        ਜੇ (!pidref_is_ਸੈੱਟ(a))
                ਵਾਪਸ !pidref_is_ਸੈੱਟ(b);

        ਜੇ (!pidref_is_ਸੈੱਟ(b))
                ਵਾਪਸ false;

        ਜੇ (a->pid != b->pid)
                ਵਾਪਸ false;

        ਜੇ (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                ਜੇ (!pidref_is_remote(b))
                        ਵਾਪਸ false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                ਜੇ (a->fd_id == 0 || b->fd_id == 0)
                        ਵਾਪਸ true;
        } ਨਹੀਂਤਾਂ {
                /* If the other side is remote, then this is not the same */
                ਜੇ (pidref_is_remote(b))
                        ਵਾਪਸ false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                ਜੇ (a->pid == 1)
                        ਵਾਪਸ true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                ਜੇ (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        ਵਾਪਸ true;
        }

        ਵਾਪਸ a->fd_id == b->fd_id;
}

ਪੂਰਨ pidref_ਸੈੱਟ_pid_ਭਰਿਆ(PidRef *pidref, pid_t pid, ਬਿਨਾਨਿਸ਼ਾਨੀ flags) {
        uint64_t pidfdid = 0;
        ਪੂਰਨ fd;

        ਪ੍ਰਤਿਜ੍ਞਾ(pidref);

        ਜੇ (pid < 0)
                ਵਾਪਸ -ESRCH;
        ਜੇ (pid == 0) {
                pid = getpid_cached();
                (ਖਾਲੀ) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        ਜੇ (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                ਜੇ (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_ਸੈੱਟ(flags, PIDFD_THREAD))))
                        ਵਾਪਸ log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        ਵਾਪਸ 0;
}

ਪੂਰਨ pidref_ਸੈੱਟ_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pidref);

        _cleanup_(pidref_ਪੂਰਾ) PidRef n = PIDREF_NULL;
        r = pidref_ਸੈੱਟ_pid(&n, pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (n.fd_id != pidfd_id)
                        ਵਾਪਸ -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        ਵਾਪਸ 0;
}

ਪੂਰਨ pidref_ਸੈੱਟ_pidstr_ਭਰਿਆ(PidRef *pidref, ਅਚਲ ਅੱਖਰ *pid, ਬਿਨਾਨਿਸ਼ਾਨੀ flags) {
        pid_t nr;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pidref);

        r = parse_pid(pid, &nr);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ pidref_ਸੈੱਟ_pid_ਭਰਿਆ(pidref, nr, flags);
}

ਪੂਰਨ pidref_ਸੈੱਟ_pidfd(PidRef *pidref, ਪੂਰਨ fd) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pidref);

        ਜੇ (fd < 0)
                ਵਾਪਸ -EBADF;

        ਪੂਰਨ fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        ਜੇ (fd_copy < 0) {
                pid_t pid;

                ਜੇ (!ERRNO_IS_RESOURCE(errno))
                        ਵਾਪਸ -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                ਵਾਪਸ 0;
        }

        ਵਾਪਸ pidref_ਸੈੱਟ_pidfd_consume(pidref, fd_copy);
}

ਪੂਰਨ pidref_ਸੈੱਟ_pidfd_take(PidRef *pidref, ਪੂਰਨ fd) {
        pid_t pid;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pidref);

        ਜੇ (fd < 0)
                ਵਾਪਸ -EBADF;

        r = pidfd_get_pid(fd, &pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        ਵਾਪਸ 0;
}

ਪੂਰਨ pidref_ਸੈੱਟ_pidfd_consume(PidRef *pidref, ਪੂਰਨ fd) {
        ਪੂਰਨ r;

        r = pidref_ਸੈੱਟ_pidfd_take(pidref, fd);
        ਜੇ (r < 0)
                ਸੁਰੱਖਿਅਤ_close(fd);

        ਵਾਪਸ r;
}

ਪੂਰਨ pidref_ਸੈੱਟ_ਮਾਪਾ(PidRef *ret) {
        _cleanup_(pidref_ਪੂਰਾ) PidRef ਮਾਪਾ = PIDREF_NULL;
        pid_t ppid;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        ਲਈ (;;) {
                r = pidref_ਸੈੱਟ_pid(&ਮਾਪਾ, ppid);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (ਮਾਪਾ.fd < 0) /* If pidfds are not available, then we are done */
                        ਰੋਕੋ;

                pid_t now_ppid = getppid();
                ਜੇ (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        ਰੋਕੋ;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_ਪੂਰਾ(&ਮਾਪਾ);
        }

        *ret = TAKE_PIDREF(ਮਾਪਾ);
        ਵਾਪਸ 0;
}

ਖਾਲੀ pidref_ਪੂਰਾ(PidRef *pidref) {
        ਪ੍ਰਤਿਜ੍ਞਾ(pidref);

        *pidref = (PidRef) {
                .fd = ਸੁਰੱਖਿਅਤ_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        ਜੇ (!pidref)
                ਵਾਪਸ ਨਿਰਾ;

        pidref_ਪੂਰਾ(pidref);
        ਵਾਪਸ mfree(pidref);
}

ਪੂਰਨ pidref_copy(ਅਚਲ PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_ਪੂਰਾ) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (pidref) {
                ਜੇ (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                ਨਹੀਂਤਾਂ ਜੇ (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        ਜੇ (copy.fd < 0) {
                                ਜੇ (!ERRNO_IS_RESOURCE(errno))
                                        ਵਾਪਸ -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        ਵਾਪਸ 0;
}

ਪੂਰਨ pidref_dup(ਅਚਲ PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = ਨਿਰਾ;
        ਪੂਰਨ r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        ਜੇ (!dup_pidref)
                ਵਾਪਸ -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = TAKE_PTR(dup_pidref);
        ਵਾਪਸ 0;
}

ਪੂਰਨ pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (pid < 0)
                ਵਾਪਸ -ESRCH;

        n = new(PidRef, 1);
        ਜੇ (!n)
                ਵਾਪਸ -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_ਸੈੱਟ_pid(n, pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = TAKE_PTR(n);
        ਵਾਪਸ 0;
}

ਪੂਰਨ pidref_kill(ਅਚਲ PidRef *pidref, ਪੂਰਨ sig) {

        ਜੇ (!pidref)
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        ਜੇ (pidref->fd >= 0)
                ਵਾਪਸ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, ਨਿਰਾ, 0));

        ਜੇ (pidref->pid > 0)
                ਵਾਪਸ RET_NERRNO(kill(pidref->pid, sig));

        ਵਾਪਸ -ESRCH;
}

ਪੂਰਨ pidref_kill_and_sigcont(ਅਚਲ PidRef *pidref, ਪੂਰਨ sig) {
        ਪੂਰਨ r;

        r = pidref_kill(pidref, sig);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(sig, SIGCONT, SIGKILL))
                (ਖਾਲੀ) pidref_kill(pidref, SIGCONT);

        ਵਾਪਸ 0;
}

ਪੂਰਨ pidref_sigqueue(ਅਚਲ PidRef *pidref, ਪੂਰਨ sig, ਪੂਰਨ ਮੁੱਲ) {

        ਜੇ (!pidref)
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        ਜੇ (pidref->fd >= 0) {
                siginfo_t si;

                /* We can't use structured initialization here, since the structure contains various unions
                 * and these fields lie in overlapping (carefully aligned) unions that LLVM is allergic to
                 * allow assignments to */
                zero(si);
                si.si_signo = sig;
                si.si_code = SI_QUEUE;
                si.si_pid = getpid_cached();
                si.si_uid = getuid();
                si.si_value.sival_int = ਮੁੱਲ;

                ਵਾਪਸ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        ਜੇ (pidref->pid > 0)
                ਵਾਪਸ RET_NERRNO(sigqueue(pidref->pid, sig, (ਅਚਲ ਸੰਘ sigval) { .sival_int = ਮੁੱਲ }));

        ਵਾਪਸ -ESRCH;
}

ਪੂਰਨ pidref_verify(ਅਚਲ PidRef *pidref) {
        ਪੂਰਨ r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        ਜੇ (pidref->pid == 1)
                ਵਾਪਸ 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        ਜੇ (pidref->fd < 0)
                ਵਾਪਸ 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ false;

        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ false;

        ਜੇ (pidref->pid != getpid_cached())
                ਵਾਪਸ false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        ਜੇ (pidref->pid == 1)
                ਵਾਪਸ true;

        /* Also compare pidfd ID if we can get it */
        ਜੇ (pidref_acquire_pidfd_id(pidref) < 0)
                ਵਾਪਸ true;

        uint64_t self_id;
        ਜੇ (pidfd_get_inode_id_self_cached(&self_id) < 0)
                ਵਾਪਸ true;

        ਵਾਪਸ pidref->fd_id == self_id;
}

ਪੂਰਨ pidref_wait_for_terminate_ਭਰਿਆ(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(timeout > 0);

        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        ਜੇ (pidref->pid == 1 || pidref_is_self(pidref))
                ਵਾਪਸ -ECHILD;

        ਜੇ (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_ਸੈੱਟ()))
                ਵਾਪਸ -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_ਜੋੜੋ(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_ਪਹਿਲਾ = ts != USEC_INFINITY || fiber_ops_is_ਸੈੱਟ();

        ਲਈ (;;) {
                ਜੇ (poll_ਪਹਿਲਾ) {
                        usec_t ਖੱਬਾ;

                        ਜੇ (ts == USEC_INFINITY)
                                ਖੱਬਾ = USEC_INFINITY;
                        ਨਹੀਂਤਾਂ {
                                ਖੱਬਾ = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                ਜੇ (ਖੱਬਾ == 0)
                                        ਵਾਪਸ -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, ਖੱਬਾ);
                        ਜੇ (r == 0)
                                ਵਾਪਸ -ETIMEDOUT;
                        ਜੇ (r == -EINTR)
                                ਜਾਰੀ;
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                siginfo_t si = {};

                ਜੇ (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                ਨਹੀਂਤਾਂ
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                ਜੇ (r >= 0) {
                        ਜੇ (ret_si)
                                *ret_si = si;
                        ਵਾਪਸ 0;
                }
                ਜੇ (r != -EINTR)
                        ਵਾਪਸ r;
        }
}

bool pidref_is_automatic(ਅਚਲ PidRef *pidref) {
        ਵਾਪਸ pidref && pid_is_automatic(pidref->pid);
}

ਖਾਲੀ pidref_hash_func(ਅਚਲ PidRef *pidref, ਬਣਤਰ siphash *state) {
        ਪ੍ਰਤਿਜ੍ਞਾ(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

ਪੂਰਨ pidref_compare_func(ਅਚਲ PidRef *a, ਅਚਲ PidRef *b) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        r = CMP(pidref_is_ਸੈੱਟ(a), pidref_is_ਸੈੱਟ(b));
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        r = CMP(a->pid, b->pid);
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        ਜੇ (a->fd_id != 0 && b->fd_id != 0)
                ਵਾਪਸ CMP(a->fd_id, b->fd_id);

        ਵਾਪਸ 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

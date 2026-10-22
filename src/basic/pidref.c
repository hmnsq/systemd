/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <poll.h>
#ضمن <sys/wait.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fiber-ops.h"
#ضمن "format-util.h"
#ضمن "hash-funcs.h"
#ضمن "io-util.h"
#ضمن "log.h"
#ضمن "parse-util.h"
#ضمن "pidfd-util.h"
#ضمن "pidref.h"
#ضمن "process-util.h"
#ضمن "siphash24.h"
#ضمن "time-util.h"

صحيح pidref_acquire_pidfd_id(PidRef *pidref) {
        صحيح r;

        أكد(pidref);

        إذا (!pidref_is_set(pidref))
                أرجع -ESRCH;

        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        إذا (pidref->fd < 0)
                أرجع -ENOMEDIUM;

        إذا (pidref->fd_id > 0)
                أرجع 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        إذا (r < 0) {
                إذا (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                أرجع r;
        }

        أرجع 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        إذا (a == b)
                أرجع true;

        إذا (!pidref_is_set(a))
                أرجع !pidref_is_set(b);

        إذا (!pidref_is_set(b))
                أرجع false;

        إذا (a->pid != b->pid)
                أرجع false;

        إذا (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                إذا (!pidref_is_remote(b))
                        أرجع false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                إذا (a->fd_id == 0 || b->fd_id == 0)
                        أرجع true;
        } وإلا {
                /* If the other side is remote, then this is not the same */
                إذا (pidref_is_remote(b))
                        أرجع false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                إذا (a->pid == 1)
                        أرجع true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                إذا (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        أرجع true;
        }

        أرجع a->fd_id == b->fd_id;
}

صحيح pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        صحيح fd;

        أكد(pidref);

        إذا (pid < 0)
                أرجع -ESRCH;
        إذا (pid == 0) {
                pid = getpid_cached();
                (فراغ) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        إذا (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                إذا (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        أرجع log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        أرجع 0;
}

صحيح pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        صحيح r;

        أكد(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        إذا (r < 0)
                أرجع r;

        إذا (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                إذا (r < 0)
                        أرجع r;

                إذا (n.fd_id != pidfd_id)
                        أرجع -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        أرجع 0;
}

صحيح pidref_set_pidstr_full(PidRef *pidref, ثابت حرف *pid, unsigned flags) {
        pid_t nr;
        صحيح r;

        أكد(pidref);

        r = parse_pid(pid, &nr);
        إذا (r < 0)
                أرجع r;

        أرجع pidref_set_pid_full(pidref, nr, flags);
}

صحيح pidref_set_pidfd(PidRef *pidref, صحيح fd) {
        صحيح r;

        أكد(pidref);

        إذا (fd < 0)
                أرجع -EBADF;

        صحيح fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        إذا (fd_copy < 0) {
                pid_t pid;

                إذا (!ERRNO_IS_RESOURCE(errno))
                        أرجع -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                إذا (r < 0)
                        أرجع r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                أرجع 0;
        }

        أرجع pidref_set_pidfd_consume(pidref, fd_copy);
}

صحيح pidref_set_pidfd_take(PidRef *pidref, صحيح fd) {
        pid_t pid;
        صحيح r;

        أكد(pidref);

        إذا (fd < 0)
                أرجع -EBADF;

        r = pidfd_get_pid(fd, &pid);
        إذا (r < 0)
                أرجع r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        أرجع 0;
}

صحيح pidref_set_pidfd_consume(PidRef *pidref, صحيح fd) {
        صحيح r;

        r = pidref_set_pidfd_take(pidref, fd);
        إذا (r < 0)
                safe_close(fd);

        أرجع r;
}

صحيح pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        صحيح r;

        أكد(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        لكل (;;) {
                r = pidref_set_pid(&parent, ppid);
                إذا (r < 0)
                        أرجع r;

                إذا (parent.fd < 0) /* If pidfds are not available, then we are done */
                        اكسر;

                pid_t now_ppid = getppid();
                إذا (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        اكسر;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        أرجع 0;
}

فراغ pidref_done(PidRef *pidref) {
        أكد(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        إذا (!pidref)
                أرجع عدم;

        pidref_done(pidref);
        أرجع mfree(pidref);
}

صحيح pidref_copy(ثابت PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        أكد(ret);

        إذا (pidref) {
                إذا (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                وإلا إذا (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        إذا (copy.fd < 0) {
                                إذا (!ERRNO_IS_RESOURCE(errno))
                                        أرجع -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        أرجع 0;
}

صحيح pidref_dup(ثابت PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = عدم;
        صحيح r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        أكد(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        إذا (!dup_pidref)
                أرجع -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(dup_pidref);
        أرجع 0;
}

صحيح pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = عدم;
        صحيح r;

        أكد(ret);

        إذا (pid < 0)
                أرجع -ESRCH;

        n = new(PidRef, 1);
        إذا (!n)
                أرجع -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(n);
        أرجع 0;
}

صحيح pidref_kill(ثابت PidRef *pidref, صحيح sig) {

        إذا (!pidref)
                أرجع -ESRCH;

        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        إذا (pidref->fd >= 0)
                أرجع RET_NERRNO(pidfd_send_signal(pidref->fd, sig, عدم, 0));

        إذا (pidref->pid > 0)
                أرجع RET_NERRNO(kill(pidref->pid, sig));

        أرجع -ESRCH;
}

صحيح pidref_kill_and_sigcont(ثابت PidRef *pidref, صحيح sig) {
        صحيح r;

        r = pidref_kill(pidref, sig);
        إذا (r < 0)
                أرجع r;

        إذا (!IN_SET(sig, SIGCONT, SIGKILL))
                (فراغ) pidref_kill(pidref, SIGCONT);

        أرجع 0;
}

صحيح pidref_sigqueue(ثابت PidRef *pidref, صحيح sig, صحيح بەها) {

        إذا (!pidref)
                أرجع -ESRCH;

        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        إذا (pidref->fd >= 0) {
                siginfo_t si;

                /* We can't use structured initialization here, since the structure contains various unions
                 * and these fields lie in overlapping (carefully aligned) unions that LLVM is allergic to
                 * allow assignments to */
                zero(si);
                si.si_signo = sig;
                si.si_code = SI_QUEUE;
                si.si_pid = getpid_cached();
                si.si_uid = getuid();
                si.si_value.sival_int = بەها;

                أرجع RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        إذا (pidref->pid > 0)
                أرجع RET_NERRNO(sigqueue(pidref->pid, sig, (ثابت اتحاد sigval) { .sival_int = بەها }));

        أرجع -ESRCH;
}

صحيح pidref_verify(ثابت PidRef *pidref) {
        صحيح r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        إذا (!pidref_is_set(pidref))
                أرجع -ESRCH;

        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        إذا (pidref->pid == 1)
                أرجع 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        إذا (pidref->fd < 0)
                أرجع 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        إذا (r < 0)
                أرجع r;

        أرجع 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        إذا (!pidref_is_set(pidref))
                أرجع false;

        إذا (pidref_is_remote(pidref))
                أرجع false;

        إذا (pidref->pid != getpid_cached())
                أرجع false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        إذا (pidref->pid == 1)
                أرجع true;

        /* Also compare pidfd ID if we can get it */
        إذا (pidref_acquire_pidfd_id(pidref) < 0)
                أرجع true;

        uint64_t self_id;
        إذا (pidfd_get_inode_id_self_cached(&self_id) < 0)
                أرجع true;

        أرجع pidref->fd_id == self_id;
}

صحيح pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        صحيح r;

        أكد(timeout > 0);

        إذا (!pidref_is_set(pidref))
                أرجع -ESRCH;

        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        إذا (pidref->pid == 1 || pidref_is_self(pidref))
                أرجع -ECHILD;

        إذا (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                أرجع -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        لكل (;;) {
                إذا (poll_first) {
                        usec_t چەپ;

                        إذا (ts == USEC_INFINITY)
                                چەپ = USEC_INFINITY;
                        وإلا {
                                چەپ = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                إذا (چەپ == 0)
                                        أرجع -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, چەپ);
                        إذا (r == 0)
                                أرجع -ETIMEDOUT;
                        إذا (r == -EINTR)
                                استمر;
                        إذا (r < 0)
                                أرجع r;
                }

                siginfo_t si = {};

                إذا (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                وإلا
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                إذا (r >= 0) {
                        إذا (ret_si)
                                *ret_si = si;
                        أرجع 0;
                }
                إذا (r != -EINTR)
                        أرجع r;
        }
}

bool pidref_is_automatic(ثابت PidRef *pidref) {
        أرجع pidref && pid_is_automatic(pidref->pid);
}

فراغ pidref_hash_func(ثابت PidRef *pidref, بنية siphash *state) {
        أكد(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

صحيح pidref_compare_func(ثابت PidRef *a, ثابت PidRef *b) {
        صحيح r;

        أكد(a);
        أكد(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        إذا (r != 0)
                أرجع r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        إذا (r != 0)
                أرجع r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        إذا (r != 0)
                أرجع r;

        r = CMP(a->pid, b->pid);
        إذا (r != 0)
                أرجع r;

        إذا (a->fd_id != 0 && b->fd_id != 0)
                أرجع CMP(a->fd_id, b->fd_id);

        أرجع 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_کلیل_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

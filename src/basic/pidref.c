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

        אם (!pidref_is_set(pidref))
                החזר -ESRCH;

        אם (pidref_is_remote(pidref))
                החזר -EREMOTE;

        אם (pidref->fd < 0)
                החזר -ENOMEDIUM;

        אם (pidref->fd_id > 0)
                החזר 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        אם (r < 0) {
                אם (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                החזר r;
        }

        החזר 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        אם (a == b)
                החזר true;

        אם (!pidref_is_set(a))
                החזר !pidref_is_set(b);

        אם (!pidref_is_set(b))
                החזר false;

        אם (a->pid != b->pid)
                החזר false;

        אם (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                אם (!pidref_is_remote(b))
                        החזר false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                אם (a->fd_id == 0 || b->fd_id == 0)
                        החזר true;
        } אחרת {
                /* If the other side is remote, then this is not the same */
                אם (pidref_is_remote(b))
                        החזר false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                אם (a->pid == 1)
                        החזר true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                אם (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        החזר true;
        }

        החזר a->fd_id == b->fd_id;
}

صحيح pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        صحيح fd;

        أكد(pidref);

        אם (pid < 0)
                החזר -ESRCH;
        אם (pid == 0) {
                pid = getpid_cached();
                (فراغ) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        אם (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                אם (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        החזר log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        החזר 0;
}

صحيح pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        صحيح r;

        أكد(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        אם (r < 0)
                החזר r;

        אם (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                אם (r < 0)
                        החזר r;

                אם (n.fd_id != pidfd_id)
                        החזר -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        החזר 0;
}

صحيح pidref_set_pidstr_full(PidRef *pidref, ثابت حرف *pid, unsigned flags) {
        pid_t nr;
        صحيح r;

        أكد(pidref);

        r = parse_pid(pid, &nr);
        אם (r < 0)
                החזר r;

        החזר pidref_set_pid_full(pidref, nr, flags);
}

صحيح pidref_set_pidfd(PidRef *pidref, صحيح fd) {
        صحيح r;

        أكد(pidref);

        אם (fd < 0)
                החזר -EBADF;

        صحيح fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        אם (fd_copy < 0) {
                pid_t pid;

                אם (!ERRNO_IS_RESOURCE(errno))
                        החזר -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                אם (r < 0)
                        החזר r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                החזר 0;
        }

        החזר pidref_set_pidfd_consume(pidref, fd_copy);
}

صحيح pidref_set_pidfd_take(PidRef *pidref, صحيح fd) {
        pid_t pid;
        صحيح r;

        أكد(pidref);

        אם (fd < 0)
                החזר -EBADF;

        r = pidfd_get_pid(fd, &pid);
        אם (r < 0)
                החזר r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        החזר 0;
}

صحيح pidref_set_pidfd_consume(PidRef *pidref, صحيح fd) {
        صحيح r;

        r = pidref_set_pidfd_take(pidref, fd);
        אם (r < 0)
                safe_close(fd);

        החזר r;
}

صحيح pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        صحيح r;

        أكد(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        עבור (;;) {
                r = pidref_set_pid(&parent, ppid);
                אם (r < 0)
                        החזר r;

                אם (parent.fd < 0) /* If pidfds are not available, then we are done */
                        اكسر;

                pid_t now_ppid = getppid();
                אם (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        اكسر;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        החזר 0;
}

فراغ pidref_done(PidRef *pidref) {
        أكد(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        אם (!pidref)
                החזר عدم;

        pidref_done(pidref);
        החזר mfree(pidref);
}

صحيح pidref_copy(ثابت PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        أكد(ret);

        אם (pidref) {
                אם (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                אחרת אם (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        אם (copy.fd < 0) {
                                אם (!ERRNO_IS_RESOURCE(errno))
                                        החזר -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        החזר 0;
}

صحيح pidref_dup(ثابت PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = عدم;
        صحيح r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        أكد(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        אם (!dup_pidref)
                החזר -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        אם (r < 0)
                החזר r;

        *ret = TAKE_PTR(dup_pidref);
        החזר 0;
}

صحيح pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = عدم;
        صحيح r;

        أكد(ret);

        אם (pid < 0)
                החזר -ESRCH;

        n = new(PidRef, 1);
        אם (!n)
                החזר -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        אם (r < 0)
                החזר r;

        *ret = TAKE_PTR(n);
        החזר 0;
}

صحيح pidref_kill(ثابت PidRef *pidref, صحيح sig) {

        אם (!pidref)
                החזר -ESRCH;

        אם (pidref_is_remote(pidref))
                החזר -EREMOTE;

        אם (pidref->fd >= 0)
                החזר RET_NERRNO(pidfd_send_signal(pidref->fd, sig, عدم, 0));

        אם (pidref->pid > 0)
                החזר RET_NERRNO(kill(pidref->pid, sig));

        החזר -ESRCH;
}

صحيح pidref_kill_and_sigcont(ثابت PidRef *pidref, صحيح sig) {
        صحيح r;

        r = pidref_kill(pidref, sig);
        אם (r < 0)
                החזר r;

        אם (!IN_SET(sig, SIGCONT, SIGKILL))
                (فراغ) pidref_kill(pidref, SIGCONT);

        החזר 0;
}

صحيح pidref_sigqueue(ثابت PidRef *pidref, صحيح sig, صحيح value) {

        אם (!pidref)
                החזר -ESRCH;

        אם (pidref_is_remote(pidref))
                החזר -EREMOTE;

        אם (pidref->fd >= 0) {
                siginfo_t si;

                /* We can't use structured initialization here, since the structure contains various unions
                 * and these fields lie in overlapping (carefully aligned) unions that LLVM is allergic to
                 * allow assignments to */
                zero(si);
                si.si_signo = sig;
                si.si_code = SI_QUEUE;
                si.si_pid = getpid_cached();
                si.si_uid = getuid();
                si.si_value.sival_int = value;

                החזר RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        אם (pidref->pid > 0)
                החזר RET_NERRNO(sigqueue(pidref->pid, sig, (ثابت اتحاد sigval) { .sival_int = value }));

        החזר -ESRCH;
}

صحيح pidref_verify(ثابت PidRef *pidref) {
        صحيح r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        אם (!pidref_is_set(pidref))
                החזר -ESRCH;

        אם (pidref_is_remote(pidref))
                החזר -EREMOTE;

        אם (pidref->pid == 1)
                החזר 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        אם (pidref->fd < 0)
                החזר 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        אם (r < 0)
                החזר r;

        החזר 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        אם (!pidref_is_set(pidref))
                החזר false;

        אם (pidref_is_remote(pidref))
                החזר false;

        אם (pidref->pid != getpid_cached())
                החזר false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        אם (pidref->pid == 1)
                החזר true;

        /* Also compare pidfd ID if we can get it */
        אם (pidref_acquire_pidfd_id(pidref) < 0)
                החזר true;

        uint64_t self_id;
        אם (pidfd_get_inode_id_self_cached(&self_id) < 0)
                החזר true;

        החזר pidref->fd_id == self_id;
}

صحيح pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        صحيح r;

        أكد(timeout > 0);

        אם (!pidref_is_set(pidref))
                החזר -ESRCH;

        אם (pidref_is_remote(pidref))
                החזר -EREMOTE;

        אם (pidref->pid == 1 || pidref_is_self(pidref))
                החזר -ECHILD;

        אם (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                החזר -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        עבור (;;) {
                אם (poll_first) {
                        usec_t left;

                        אם (ts == USEC_INFINITY)
                                left = USEC_INFINITY;
                        אחרת {
                                left = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                אם (left == 0)
                                        החזר -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, left);
                        אם (r == 0)
                                החזר -ETIMEDOUT;
                        אם (r == -EINTR)
                                استمر;
                        אם (r < 0)
                                החזר r;
                }

                siginfo_t si = {};

                אם (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                אחרת
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                אם (r >= 0) {
                        אם (ret_si)
                                *ret_si = si;
                        החזר 0;
                }
                אם (r != -EINTR)
                        החזר r;
        }
}

bool pidref_is_automatic(ثابت PidRef *pidref) {
        החזר pidref && pid_is_automatic(pidref->pid);
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
        אם (r != 0)
                החזר r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        אם (r != 0)
                החזר r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        אם (r != 0)
                החזר r;

        r = CMP(a->pid, b->pid);
        אם (r != 0)
                החזר r;

        אם (a->fd_id != 0 && b->fd_id != 0)
                החזר CMP(a->fd_id, b->fd_id);

        החזר 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

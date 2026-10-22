/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <poll.h>
#شامل <sys/wait.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fiber-ops.h"
#شامل "format-util.h"
#شامل "hash-funcs.h"
#شامل "io-util.h"
#شامل "log.h"
#شامل "parse-util.h"
#شامل "pidfd-util.h"
#شامل "pidref.h"
#شامل "process-util.h"
#شامل "siphash24.h"
#شامل "time-util.h"

صحیح pidref_acquire_pidfd_id(PidRef *pidref) {
        صحیح r;

        أكد(pidref);

        اگر (!pidref_is_set(pidref))
                بازگشت -ESRCH;

        اگر (pidref_is_remote(pidref))
                بازگشت -EREMOTE;

        اگر (pidref->fd < 0)
                بازگشت -ENOMEDIUM;

        اگر (pidref->fd_id > 0)
                بازگشت 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        اگر (r < 0) {
                اگر (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                بازگشت r;
        }

        بازگشت 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        اگر (a == b)
                بازگشت true;

        اگر (!pidref_is_set(a))
                بازگشت !pidref_is_set(b);

        اگر (!pidref_is_set(b))
                بازگشت false;

        اگر (a->pid != b->pid)
                بازگشت false;

        اگر (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                اگر (!pidref_is_remote(b))
                        بازگشت false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                اگر (a->fd_id == 0 || b->fd_id == 0)
                        بازگشت true;
        } وگرنه {
                /* If the other side is remote, then this is not the same */
                اگر (pidref_is_remote(b))
                        بازگشت false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                اگر (a->pid == 1)
                        بازگشت true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                اگر (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        بازگشت true;
        }

        بازگشت a->fd_id == b->fd_id;
}

صحیح pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        صحیح fd;

        أكد(pidref);

        اگر (pid < 0)
                بازگشت -ESRCH;
        اگر (pid == 0) {
                pid = getpid_cached();
                (تهی) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        اگر (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                اگر (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        بازگشت log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        بازگشت 0;
}

صحیح pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        صحیح r;

        أكد(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        اگر (r < 0)
                بازگشت r;

        اگر (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                اگر (r < 0)
                        بازگشت r;

                اگر (n.fd_id != pidfd_id)
                        بازگشت -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        بازگشت 0;
}

صحیح pidref_set_pidstr_full(PidRef *pidref, ثابت نویسه *pid, unsigned flags) {
        pid_t nr;
        صحیح r;

        أكد(pidref);

        r = parse_pid(pid, &nr);
        اگر (r < 0)
                بازگشت r;

        بازگشت pidref_set_pid_full(pidref, nr, flags);
}

صحیح pidref_set_pidfd(PidRef *pidref, صحیح fd) {
        صحیح r;

        أكد(pidref);

        اگر (fd < 0)
                بازگشت -EBADF;

        صحیح fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        اگر (fd_copy < 0) {
                pid_t pid;

                اگر (!ERRNO_IS_RESOURCE(errno))
                        بازگشت -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                اگر (r < 0)
                        بازگشت r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                بازگشت 0;
        }

        بازگشت pidref_set_pidfd_consume(pidref, fd_copy);
}

صحیح pidref_set_pidfd_take(PidRef *pidref, صحیح fd) {
        pid_t pid;
        صحیح r;

        أكد(pidref);

        اگر (fd < 0)
                بازگشت -EBADF;

        r = pidfd_get_pid(fd, &pid);
        اگر (r < 0)
                بازگشت r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        بازگشت 0;
}

صحیح pidref_set_pidfd_consume(PidRef *pidref, صحیح fd) {
        صحیح r;

        r = pidref_set_pidfd_take(pidref, fd);
        اگر (r < 0)
                safe_close(fd);

        بازگشت r;
}

صحیح pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        صحیح r;

        أكد(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        برای (;;) {
                r = pidref_set_pid(&parent, ppid);
                اگر (r < 0)
                        بازگشت r;

                اگر (parent.fd < 0) /* If pidfds are not available, then we are done */
                        بشکن;

                pid_t now_ppid = getppid();
                اگر (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        بشکن;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        بازگشت 0;
}

تهی pidref_done(PidRef *pidref) {
        أكد(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        اگر (!pidref)
                بازگشت پوچ;

        pidref_done(pidref);
        بازگشت mfree(pidref);
}

صحیح pidref_copy(ثابت PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        أكد(ret);

        اگر (pidref) {
                اگر (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                وگرنه اگر (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        اگر (copy.fd < 0) {
                                اگر (!ERRNO_IS_RESOURCE(errno))
                                        بازگشت -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        بازگشت 0;
}

صحیح pidref_dup(ثابت PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = پوچ;
        صحیح r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        أكد(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        اگر (!dup_pidref)
                بازگشت -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        اگر (r < 0)
                بازگشت r;

        *ret = TAKE_PTR(dup_pidref);
        بازگشت 0;
}

صحیح pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = پوچ;
        صحیح r;

        أكد(ret);

        اگر (pid < 0)
                بازگشت -ESRCH;

        n = new(PidRef, 1);
        اگر (!n)
                بازگشت -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        اگر (r < 0)
                بازگشت r;

        *ret = TAKE_PTR(n);
        بازگشت 0;
}

صحیح pidref_kill(ثابت PidRef *pidref, صحیح sig) {

        اگر (!pidref)
                بازگشت -ESRCH;

        اگر (pidref_is_remote(pidref))
                بازگشت -EREMOTE;

        اگر (pidref->fd >= 0)
                بازگشت RET_NERRNO(pidfd_send_signal(pidref->fd, sig, پوچ, 0));

        اگر (pidref->pid > 0)
                بازگشت RET_NERRNO(kill(pidref->pid, sig));

        بازگشت -ESRCH;
}

صحیح pidref_kill_and_sigcont(ثابت PidRef *pidref, صحیح sig) {
        صحیح r;

        r = pidref_kill(pidref, sig);
        اگر (r < 0)
                بازگشت r;

        اگر (!IN_SET(sig, SIGCONT, SIGKILL))
                (تهی) pidref_kill(pidref, SIGCONT);

        بازگشت 0;
}

صحیح pidref_sigqueue(ثابت PidRef *pidref, صحیح sig, صحیح مقدار) {

        اگر (!pidref)
                بازگشت -ESRCH;

        اگر (pidref_is_remote(pidref))
                بازگشت -EREMOTE;

        اگر (pidref->fd >= 0) {
                siginfo_t si;

                /* We can't use structured initialization here, since the structure contains various unions
                 * and these fields lie in overlapping (carefully aligned) unions that LLVM is allergic to
                 * allow assignments to */
                zero(si);
                si.si_signo = sig;
                si.si_code = SI_QUEUE;
                si.si_pid = getpid_cached();
                si.si_uid = getuid();
                si.si_value.sival_int = مقدار;

                بازگشت RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        اگر (pidref->pid > 0)
                بازگشت RET_NERRNO(sigqueue(pidref->pid, sig, (ثابت اجتماع sigval) { .sival_int = مقدار }));

        بازگشت -ESRCH;
}

صحیح pidref_verify(ثابت PidRef *pidref) {
        صحیح r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        اگر (!pidref_is_set(pidref))
                بازگشت -ESRCH;

        اگر (pidref_is_remote(pidref))
                بازگشت -EREMOTE;

        اگر (pidref->pid == 1)
                بازگشت 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        اگر (pidref->fd < 0)
                بازگشت 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        اگر (r < 0)
                بازگشت r;

        بازگشت 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        اگر (!pidref_is_set(pidref))
                بازگشت false;

        اگر (pidref_is_remote(pidref))
                بازگشت false;

        اگر (pidref->pid != getpid_cached())
                بازگشت false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        اگر (pidref->pid == 1)
                بازگشت true;

        /* Also compare pidfd ID if we can get it */
        اگر (pidref_acquire_pidfd_id(pidref) < 0)
                بازگشت true;

        uint64_t self_id;
        اگر (pidfd_get_inode_id_self_cached(&self_id) < 0)
                بازگشت true;

        بازگشت pidref->fd_id == self_id;
}

صحیح pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        صحیح r;

        أكد(timeout > 0);

        اگر (!pidref_is_set(pidref))
                بازگشت -ESRCH;

        اگر (pidref_is_remote(pidref))
                بازگشت -EREMOTE;

        اگر (pidref->pid == 1 || pidref_is_self(pidref))
                بازگشت -ECHILD;

        اگر (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                بازگشت -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        برای (;;) {
                اگر (poll_first) {
                        usec_t چپ;

                        اگر (ts == USEC_INFINITY)
                                چپ = USEC_INFINITY;
                        وگرنه {
                                چپ = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                اگر (چپ == 0)
                                        بازگشت -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, چپ);
                        اگر (r == 0)
                                بازگشت -ETIMEDOUT;
                        اگر (r == -EINTR)
                                ادامه;
                        اگر (r < 0)
                                بازگشت r;
                }

                siginfo_t si = {};

                اگر (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                وگرنه
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                اگر (r >= 0) {
                        اگر (ret_si)
                                *ret_si = si;
                        بازگشت 0;
                }
                اگر (r != -EINTR)
                        بازگشت r;
        }
}

bool pidref_is_automatic(ثابت PidRef *pidref) {
        بازگشت pidref && pid_is_automatic(pidref->pid);
}

تهی pidref_hash_func(ثابت PidRef *pidref, ساختار siphash *state) {
        أكد(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

صحیح pidref_compare_func(ثابت PidRef *a, ثابت PidRef *b) {
        صحیح r;

        أكد(a);
        أكد(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        اگر (r != 0)
                بازگشت r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        اگر (r != 0)
                بازگشت r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        اگر (r != 0)
                بازگشت r;

        r = CMP(a->pid, b->pid);
        اگر (r != 0)
                بازگشت r;

        اگر (a->fd_id != 0 && b->fd_id != 0)
                بازگشت CMP(a->fd_id, b->fd_id);

        بازگشت 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

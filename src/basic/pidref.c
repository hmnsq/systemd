/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <poll.h>
#include <sys/wait.h>
#include <unistd.h>

#include "alloc-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fiber-ops.h"
#include "format-util.h"
#include "hash-funcs.h"
#include "io-util.h"
#include "log.h"
#include "parse-util.h"
#include "pidfd-util.h"
#include "pidref.h"
#include "process-util.h"
#include "siphash24.h"
#include "time-util.h"

целое pidref_acquire_pidfd_id(PidRef *pidref) {
        целое r;

        assert(pidref);

        если (!pidref_is_set(pidref))
                вернуть -ESRCH;

        если (pidref_is_remote(pidref))
                вернуть -EREMOTE;

        если (pidref->fd < 0)
                вернуть -ENOMEDIUM;

        если (pidref->fd_id > 0)
                вернуть 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        если (r < 0) {
                если (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                вернуть r;
        }

        вернуть 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        если (a == b)
                вернуть true;

        если (!pidref_is_set(a))
                вернуть !pidref_is_set(b);

        если (!pidref_is_set(b))
                вернуть false;

        если (a->pid != b->pid)
                вернуть false;

        если (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                если (!pidref_is_remote(b))
                        вернуть false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                если (a->fd_id == 0 || b->fd_id == 0)
                        вернуть true;
        } иначе {
                /* If the other side is remote, then this is not the same */
                если (pidref_is_remote(b))
                        вернуть false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                если (a->pid == 1)
                        вернуть true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                если (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        вернуть true;
        }

        вернуть a->fd_id == b->fd_id;
}

целое pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        целое fd;

        assert(pidref);

        если (pid < 0)
                вернуть -ESRCH;
        если (pid == 0) {
                pid = getpid_cached();
                (void) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        если (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                если (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        вернуть log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        вернуть 0;
}

целое pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        целое r;

        assert(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        если (r < 0)
                вернуть r;

        если (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                если (r < 0)
                        вернуть r;

                если (n.fd_id != pidfd_id)
                        вернуть -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        вернуть 0;
}

целое pidref_set_pidstr_full(PidRef *pidref, const char *pid, unsigned flags) {
        pid_t nr;
        целое r;

        assert(pidref);

        r = parse_pid(pid, &nr);
        если (r < 0)
                вернуть r;

        вернуть pidref_set_pid_full(pidref, nr, flags);
}

целое pidref_set_pidfd(PidRef *pidref, целое fd) {
        целое r;

        assert(pidref);

        если (fd < 0)
                вернуть -EBADF;

        целое fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        если (fd_copy < 0) {
                pid_t pid;

                если (!ERRNO_IS_RESOURCE(errno))
                        вернуть -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                если (r < 0)
                        вернуть r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                вернуть 0;
        }

        вернуть pidref_set_pidfd_consume(pidref, fd_copy);
}

целое pidref_set_pidfd_take(PidRef *pidref, целое fd) {
        pid_t pid;
        целое r;

        assert(pidref);

        если (fd < 0)
                вернуть -EBADF;

        r = pidfd_get_pid(fd, &pid);
        если (r < 0)
                вернуть r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        вернуть 0;
}

целое pidref_set_pidfd_consume(PidRef *pidref, целое fd) {
        целое r;

        r = pidref_set_pidfd_take(pidref, fd);
        если (r < 0)
                safe_close(fd);

        вернуть r;
}

целое pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        целое r;

        assert(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        для (;;) {
                r = pidref_set_pid(&parent, ppid);
                если (r < 0)
                        вернуть r;

                если (parent.fd < 0) /* If pidfds are not available, then we are done */
                        break;

                pid_t now_ppid = getppid();
                если (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        break;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        вернуть 0;
}

void pidref_done(PidRef *pidref) {
        assert(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        если (!pidref)
                вернуть NULL;

        pidref_done(pidref);
        вернуть mfree(pidref);
}

целое pidref_copy(const PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        assert(ret);

        если (pidref) {
                если (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                иначе если (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        если (copy.fd < 0) {
                                если (!ERRNO_IS_RESOURCE(errno))
                                        вернуть -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        вернуть 0;
}

целое pidref_dup(const PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = NULL;
        целое r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        assert(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        если (!dup_pidref)
                вернуть -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        если (r < 0)
                вернуть r;

        *ret = TAKE_PTR(dup_pidref);
        вернуть 0;
}

целое pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = NULL;
        целое r;

        assert(ret);

        если (pid < 0)
                вернуть -ESRCH;

        n = new(PidRef, 1);
        если (!n)
                вернуть -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        если (r < 0)
                вернуть r;

        *ret = TAKE_PTR(n);
        вернуть 0;
}

целое pidref_kill(const PidRef *pidref, целое sig) {

        если (!pidref)
                вернуть -ESRCH;

        если (pidref_is_remote(pidref))
                вернуть -EREMOTE;

        если (pidref->fd >= 0)
                вернуть RET_NERRNO(pidfd_send_signal(pidref->fd, sig, NULL, 0));

        если (pidref->pid > 0)
                вернуть RET_NERRNO(kill(pidref->pid, sig));

        вернуть -ESRCH;
}

целое pidref_kill_and_sigcont(const PidRef *pidref, целое sig) {
        целое r;

        r = pidref_kill(pidref, sig);
        если (r < 0)
                вернуть r;

        если (!IN_SET(sig, SIGCONT, SIGKILL))
                (void) pidref_kill(pidref, SIGCONT);

        вернуть 0;
}

целое pidref_sigqueue(const PidRef *pidref, целое sig, целое value) {

        если (!pidref)
                вернуть -ESRCH;

        если (pidref_is_remote(pidref))
                вернуть -EREMOTE;

        если (pidref->fd >= 0) {
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

                вернуть RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        если (pidref->pid > 0)
                вернуть RET_NERRNO(sigqueue(pidref->pid, sig, (const union sigval) { .sival_int = value }));

        вернуть -ESRCH;
}

целое pidref_verify(const PidRef *pidref) {
        целое r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        если (!pidref_is_set(pidref))
                вернуть -ESRCH;

        если (pidref_is_remote(pidref))
                вернуть -EREMOTE;

        если (pidref->pid == 1)
                вернуть 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        если (pidref->fd < 0)
                вернуть 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        если (r < 0)
                вернуть r;

        вернуть 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        если (!pidref_is_set(pidref))
                вернуть false;

        если (pidref_is_remote(pidref))
                вернуть false;

        если (pidref->pid != getpid_cached())
                вернуть false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        если (pidref->pid == 1)
                вернуть true;

        /* Also compare pidfd ID if we can get it */
        если (pidref_acquire_pidfd_id(pidref) < 0)
                вернуть true;

        uint64_t self_id;
        если (pidfd_get_inode_id_self_cached(&self_id) < 0)
                вернуть true;

        вернуть pidref->fd_id == self_id;
}

целое pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        целое r;

        assert(timeout > 0);

        если (!pidref_is_set(pidref))
                вернуть -ESRCH;

        если (pidref_is_remote(pidref))
                вернуть -EREMOTE;

        если (pidref->pid == 1 || pidref_is_self(pidref))
                вернуть -ECHILD;

        если (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                вернуть -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        для (;;) {
                если (poll_first) {
                        usec_t left;

                        если (ts == USEC_INFINITY)
                                left = USEC_INFINITY;
                        иначе {
                                left = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                если (left == 0)
                                        вернуть -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, left);
                        если (r == 0)
                                вернуть -ETIMEDOUT;
                        если (r == -EINTR)
                                continue;
                        если (r < 0)
                                вернуть r;
                }

                siginfo_t si = {};

                если (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                иначе
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                если (r >= 0) {
                        если (ret_si)
                                *ret_si = si;
                        вернуть 0;
                }
                если (r != -EINTR)
                        вернуть r;
        }
}

bool pidref_is_automatic(const PidRef *pidref) {
        вернуть pidref && pid_is_automatic(pidref->pid);
}

void pidref_hash_func(const PidRef *pidref, struct siphash *state) {
        assert(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

целое pidref_compare_func(const PidRef *a, const PidRef *b) {
        целое r;

        assert(a);
        assert(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        если (r != 0)
                вернуть r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        если (r != 0)
                вернуть r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        если (r != 0)
                вернуть r;

        r = CMP(a->pid, b->pid);
        если (r != 0)
                вернуть r;

        если (a->fd_id != 0 && b->fd_id != 0)
                вернуть CMP(a->fd_id, b->fd_id);

        вернуть 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

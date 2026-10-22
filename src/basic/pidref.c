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

ціле pidref_acquire_pidfd_id(PidRef *pidref) {
        ціле r;

        assert(pidref);

        якщо (!pidref_is_set(pidref))
                повернути -ESRCH;

        якщо (pidref_is_remote(pidref))
                повернути -EREMOTE;

        якщо (pidref->fd < 0)
                повернути -ENOMEDIUM;

        якщо (pidref->fd_id > 0)
                повернути 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        якщо (r < 0) {
                якщо (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                повернути r;
        }

        повернути 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        якщо (a == b)
                повернути true;

        якщо (!pidref_is_set(a))
                повернути !pidref_is_set(b);

        якщо (!pidref_is_set(b))
                повернути false;

        якщо (a->pid != b->pid)
                повернути false;

        якщо (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                якщо (!pidref_is_remote(b))
                        повернути false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                якщо (a->fd_id == 0 || b->fd_id == 0)
                        повернути true;
        } інакше {
                /* If the other side is remote, then this is not the same */
                якщо (pidref_is_remote(b))
                        повернути false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                якщо (a->pid == 1)
                        повернути true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                якщо (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        повернути true;
        }

        повернути a->fd_id == b->fd_id;
}

ціле pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        ціле fd;

        assert(pidref);

        якщо (pid < 0)
                повернути -ESRCH;
        якщо (pid == 0) {
                pid = getpid_cached();
                (void) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        якщо (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                якщо (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        повернути log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        повернути 0;
}

ціле pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        ціле r;

        assert(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        якщо (r < 0)
                повернути r;

        якщо (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                якщо (r < 0)
                        повернути r;

                якщо (n.fd_id != pidfd_id)
                        повернути -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        повернути 0;
}

ціле pidref_set_pidstr_full(PidRef *pidref, const char *pid, unsigned flags) {
        pid_t nr;
        ціле r;

        assert(pidref);

        r = parse_pid(pid, &nr);
        якщо (r < 0)
                повернути r;

        повернути pidref_set_pid_full(pidref, nr, flags);
}

ціле pidref_set_pidfd(PidRef *pidref, ціле fd) {
        ціле r;

        assert(pidref);

        якщо (fd < 0)
                повернути -EBADF;

        ціле fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        якщо (fd_copy < 0) {
                pid_t pid;

                якщо (!ERRNO_IS_RESOURCE(errno))
                        повернути -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                якщо (r < 0)
                        повернути r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                повернути 0;
        }

        повернути pidref_set_pidfd_consume(pidref, fd_copy);
}

ціле pidref_set_pidfd_take(PidRef *pidref, ціле fd) {
        pid_t pid;
        ціле r;

        assert(pidref);

        якщо (fd < 0)
                повернути -EBADF;

        r = pidfd_get_pid(fd, &pid);
        якщо (r < 0)
                повернути r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        повернути 0;
}

ціле pidref_set_pidfd_consume(PidRef *pidref, ціле fd) {
        ціле r;

        r = pidref_set_pidfd_take(pidref, fd);
        якщо (r < 0)
                safe_close(fd);

        повернути r;
}

ціле pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        ціле r;

        assert(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        для (;;) {
                r = pidref_set_pid(&parent, ppid);
                якщо (r < 0)
                        повернути r;

                якщо (parent.fd < 0) /* If pidfds are not available, then we are done */
                        break;

                pid_t now_ppid = getppid();
                якщо (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        break;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        повернути 0;
}

void pidref_done(PidRef *pidref) {
        assert(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        якщо (!pidref)
                повернути NULL;

        pidref_done(pidref);
        повернути mfree(pidref);
}

ціле pidref_copy(const PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        assert(ret);

        якщо (pidref) {
                якщо (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                інакше якщо (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        якщо (copy.fd < 0) {
                                якщо (!ERRNO_IS_RESOURCE(errno))
                                        повернути -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        повернути 0;
}

ціле pidref_dup(const PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = NULL;
        ціле r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        assert(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        якщо (!dup_pidref)
                повернути -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        якщо (r < 0)
                повернути r;

        *ret = TAKE_PTR(dup_pidref);
        повернути 0;
}

ціле pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = NULL;
        ціле r;

        assert(ret);

        якщо (pid < 0)
                повернути -ESRCH;

        n = new(PidRef, 1);
        якщо (!n)
                повернути -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        якщо (r < 0)
                повернути r;

        *ret = TAKE_PTR(n);
        повернути 0;
}

ціле pidref_kill(const PidRef *pidref, ціле sig) {

        якщо (!pidref)
                повернути -ESRCH;

        якщо (pidref_is_remote(pidref))
                повернути -EREMOTE;

        якщо (pidref->fd >= 0)
                повернути RET_NERRNO(pidfd_send_signal(pidref->fd, sig, NULL, 0));

        якщо (pidref->pid > 0)
                повернути RET_NERRNO(kill(pidref->pid, sig));

        повернути -ESRCH;
}

ціле pidref_kill_and_sigcont(const PidRef *pidref, ціле sig) {
        ціле r;

        r = pidref_kill(pidref, sig);
        якщо (r < 0)
                повернути r;

        якщо (!IN_SET(sig, SIGCONT, SIGKILL))
                (void) pidref_kill(pidref, SIGCONT);

        повернути 0;
}

ціле pidref_sigqueue(const PidRef *pidref, ціле sig, ціле value) {

        якщо (!pidref)
                повернути -ESRCH;

        якщо (pidref_is_remote(pidref))
                повернути -EREMOTE;

        якщо (pidref->fd >= 0) {
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

                повернути RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        якщо (pidref->pid > 0)
                повернути RET_NERRNO(sigqueue(pidref->pid, sig, (const union sigval) { .sival_int = value }));

        повернути -ESRCH;
}

ціле pidref_verify(const PidRef *pidref) {
        ціле r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        якщо (!pidref_is_set(pidref))
                повернути -ESRCH;

        якщо (pidref_is_remote(pidref))
                повернути -EREMOTE;

        якщо (pidref->pid == 1)
                повернути 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        якщо (pidref->fd < 0)
                повернути 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        якщо (r < 0)
                повернути r;

        повернути 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        якщо (!pidref_is_set(pidref))
                повернути false;

        якщо (pidref_is_remote(pidref))
                повернути false;

        якщо (pidref->pid != getpid_cached())
                повернути false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        якщо (pidref->pid == 1)
                повернути true;

        /* Also compare pidfd ID if we can get it */
        якщо (pidref_acquire_pidfd_id(pidref) < 0)
                повернути true;

        uint64_t self_id;
        якщо (pidfd_get_inode_id_self_cached(&self_id) < 0)
                повернути true;

        повернути pidref->fd_id == self_id;
}

ціле pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        ціле r;

        assert(timeout > 0);

        якщо (!pidref_is_set(pidref))
                повернути -ESRCH;

        якщо (pidref_is_remote(pidref))
                повернути -EREMOTE;

        якщо (pidref->pid == 1 || pidref_is_self(pidref))
                повернути -ECHILD;

        якщо (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                повернути -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        для (;;) {
                якщо (poll_first) {
                        usec_t left;

                        якщо (ts == USEC_INFINITY)
                                left = USEC_INFINITY;
                        інакше {
                                left = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                якщо (left == 0)
                                        повернути -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, left);
                        якщо (r == 0)
                                повернути -ETIMEDOUT;
                        якщо (r == -EINTR)
                                continue;
                        якщо (r < 0)
                                повернути r;
                }

                siginfo_t si = {};

                якщо (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                інакше
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                якщо (r >= 0) {
                        якщо (ret_si)
                                *ret_si = si;
                        повернути 0;
                }
                якщо (r != -EINTR)
                        повернути r;
        }
}

bool pidref_is_automatic(const PidRef *pidref) {
        повернути pidref && pid_is_automatic(pidref->pid);
}

void pidref_hash_func(const PidRef *pidref, struct siphash *state) {
        assert(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

ціле pidref_compare_func(const PidRef *a, const PidRef *b) {
        ціле r;

        assert(a);
        assert(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        якщо (r != 0)
                повернути r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        якщо (r != 0)
                повернути r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        якщо (r != 0)
                повернути r;

        r = CMP(a->pid, b->pid);
        якщо (r != 0)
                повернути r;

        якщо (a->fd_id != 0 && b->fd_id != 0)
                повернути CMP(a->fd_id, b->fd_id);

        повернути 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

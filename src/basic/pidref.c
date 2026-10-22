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

цео pidref_acquire_pidfd_id(PidRef *pidref) {
        цео r;

        assert(pidref);

        ако (!pidref_is_set(pidref))
                врати -ESRCH;

        ако (pidref_is_remote(pidref))
                врати -EREMOTE;

        ако (pidref->fd < 0)
                врати -ENOMEDIUM;

        ако (pidref->fd_id > 0)
                врати 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        ако (r < 0) {
                ако (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                врати r;
        }

        врати 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        ако (a == b)
                врати true;

        ако (!pidref_is_set(a))
                врати !pidref_is_set(b);

        ако (!pidref_is_set(b))
                врати false;

        ако (a->pid != b->pid)
                врати false;

        ако (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                ако (!pidref_is_remote(b))
                        врати false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                ако (a->fd_id == 0 || b->fd_id == 0)
                        врати true;
        } инако {
                /* If the other side is remote, then this is not the same */
                ако (pidref_is_remote(b))
                        врати false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                ако (a->pid == 1)
                        врати true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                ако (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        врати true;
        }

        врати a->fd_id == b->fd_id;
}

цео pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        цео fd;

        assert(pidref);

        ако (pid < 0)
                врати -ESRCH;
        ако (pid == 0) {
                pid = getpid_cached();
                (void) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        ако (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                ако (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        врати log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        врати 0;
}

цео pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        цео r;

        assert(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        ако (r < 0)
                врати r;

        ако (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                ако (r < 0)
                        врати r;

                ако (n.fd_id != pidfd_id)
                        врати -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        врати 0;
}

цео pidref_set_pidstr_full(PidRef *pidref, const char *pid, unsigned flags) {
        pid_t nr;
        цео r;

        assert(pidref);

        r = parse_pid(pid, &nr);
        ако (r < 0)
                врати r;

        врати pidref_set_pid_full(pidref, nr, flags);
}

цео pidref_set_pidfd(PidRef *pidref, цео fd) {
        цео r;

        assert(pidref);

        ако (fd < 0)
                врати -EBADF;

        цео fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        ако (fd_copy < 0) {
                pid_t pid;

                ако (!ERRNO_IS_RESOURCE(errno))
                        врати -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                ако (r < 0)
                        врати r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                врати 0;
        }

        врати pidref_set_pidfd_consume(pidref, fd_copy);
}

цео pidref_set_pidfd_take(PidRef *pidref, цео fd) {
        pid_t pid;
        цео r;

        assert(pidref);

        ако (fd < 0)
                врати -EBADF;

        r = pidfd_get_pid(fd, &pid);
        ако (r < 0)
                врати r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        врати 0;
}

цео pidref_set_pidfd_consume(PidRef *pidref, цео fd) {
        цео r;

        r = pidref_set_pidfd_take(pidref, fd);
        ако (r < 0)
                safe_close(fd);

        врати r;
}

цео pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        цео r;

        assert(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        за (;;) {
                r = pidref_set_pid(&parent, ppid);
                ако (r < 0)
                        врати r;

                ако (parent.fd < 0) /* If pidfds are not available, then we are done */
                        break;

                pid_t now_ppid = getppid();
                ако (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        break;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        врати 0;
}

void pidref_done(PidRef *pidref) {
        assert(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        ако (!pidref)
                врати NULL;

        pidref_done(pidref);
        врати mfree(pidref);
}

цео pidref_copy(const PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        assert(ret);

        ако (pidref) {
                ако (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                инако ако (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        ако (copy.fd < 0) {
                                ако (!ERRNO_IS_RESOURCE(errno))
                                        врати -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        врати 0;
}

цео pidref_dup(const PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = NULL;
        цео r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        assert(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        ако (!dup_pidref)
                врати -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        ако (r < 0)
                врати r;

        *ret = TAKE_PTR(dup_pidref);
        врати 0;
}

цео pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = NULL;
        цео r;

        assert(ret);

        ако (pid < 0)
                врати -ESRCH;

        n = new(PidRef, 1);
        ако (!n)
                врати -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        ако (r < 0)
                врати r;

        *ret = TAKE_PTR(n);
        врати 0;
}

цео pidref_kill(const PidRef *pidref, цео sig) {

        ако (!pidref)
                врати -ESRCH;

        ако (pidref_is_remote(pidref))
                врати -EREMOTE;

        ако (pidref->fd >= 0)
                врати RET_NERRNO(pidfd_send_signal(pidref->fd, sig, NULL, 0));

        ако (pidref->pid > 0)
                врати RET_NERRNO(kill(pidref->pid, sig));

        врати -ESRCH;
}

цео pidref_kill_and_sigcont(const PidRef *pidref, цео sig) {
        цео r;

        r = pidref_kill(pidref, sig);
        ако (r < 0)
                врати r;

        ако (!IN_SET(sig, SIGCONT, SIGKILL))
                (void) pidref_kill(pidref, SIGCONT);

        врати 0;
}

цео pidref_sigqueue(const PidRef *pidref, цео sig, цео value) {

        ако (!pidref)
                врати -ESRCH;

        ако (pidref_is_remote(pidref))
                врати -EREMOTE;

        ако (pidref->fd >= 0) {
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

                врати RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        ако (pidref->pid > 0)
                врати RET_NERRNO(sigqueue(pidref->pid, sig, (const union sigval) { .sival_int = value }));

        врати -ESRCH;
}

цео pidref_verify(const PidRef *pidref) {
        цео r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        ако (!pidref_is_set(pidref))
                врати -ESRCH;

        ако (pidref_is_remote(pidref))
                врати -EREMOTE;

        ако (pidref->pid == 1)
                врати 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        ако (pidref->fd < 0)
                врати 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        ако (r < 0)
                врати r;

        врати 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        ако (!pidref_is_set(pidref))
                врати false;

        ако (pidref_is_remote(pidref))
                врати false;

        ако (pidref->pid != getpid_cached())
                врати false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        ако (pidref->pid == 1)
                врати true;

        /* Also compare pidfd ID if we can get it */
        ако (pidref_acquire_pidfd_id(pidref) < 0)
                врати true;

        uint64_t self_id;
        ако (pidfd_get_inode_id_self_cached(&self_id) < 0)
                врати true;

        врати pidref->fd_id == self_id;
}

цео pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        цео r;

        assert(timeout > 0);

        ако (!pidref_is_set(pidref))
                врати -ESRCH;

        ако (pidref_is_remote(pidref))
                врати -EREMOTE;

        ако (pidref->pid == 1 || pidref_is_self(pidref))
                врати -ECHILD;

        ако (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                врати -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        за (;;) {
                ако (poll_first) {
                        usec_t left;

                        ако (ts == USEC_INFINITY)
                                left = USEC_INFINITY;
                        инако {
                                left = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                ако (left == 0)
                                        врати -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, left);
                        ако (r == 0)
                                врати -ETIMEDOUT;
                        ако (r == -EINTR)
                                continue;
                        ако (r < 0)
                                врати r;
                }

                siginfo_t si = {};

                ако (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                инако
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                ако (r >= 0) {
                        ако (ret_si)
                                *ret_si = si;
                        врати 0;
                }
                ако (r != -EINTR)
                        врати r;
        }
}

bool pidref_is_automatic(const PidRef *pidref) {
        врати pidref && pid_is_automatic(pidref->pid);
}

void pidref_hash_func(const PidRef *pidref, struct siphash *state) {
        assert(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

цео pidref_compare_func(const PidRef *a, const PidRef *b) {
        цео r;

        assert(a);
        assert(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        ако (r != 0)
                врати r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        ако (r != 0)
                врати r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        ако (r != 0)
                врати r;

        r = CMP(a->pid, b->pid);
        ако (r != 0)
                врати r;

        ако (a->fd_id != 0 && b->fd_id != 0)
                врати CMP(a->fd_id, b->fd_id);

        врати 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

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

정수 pidref_acquire_pidfd_id(PidRef *pidref) {
        정수 r;

        assert(pidref);

        만약 (!pidref_is_set(pidref))
                반환 -ESRCH;

        만약 (pidref_is_remote(pidref))
                반환 -EREMOTE;

        만약 (pidref->fd < 0)
                반환 -ENOMEDIUM;

        만약 (pidref->fd_id > 0)
                반환 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        만약 (r < 0) {
                만약 (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                반환 r;
        }

        반환 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        만약 (a == b)
                반환 true;

        만약 (!pidref_is_set(a))
                반환 !pidref_is_set(b);

        만약 (!pidref_is_set(b))
                반환 false;

        만약 (a->pid != b->pid)
                반환 false;

        만약 (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                만약 (!pidref_is_remote(b))
                        반환 false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                만약 (a->fd_id == 0 || b->fd_id == 0)
                        반환 true;
        } 아니면 {
                /* If the other side is remote, then this is not the same */
                만약 (pidref_is_remote(b))
                        반환 false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                만약 (a->pid == 1)
                        반환 true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                만약 (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        반환 true;
        }

        반환 a->fd_id == b->fd_id;
}

정수 pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        정수 fd;

        assert(pidref);

        만약 (pid < 0)
                반환 -ESRCH;
        만약 (pid == 0) {
                pid = getpid_cached();
                (void) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        만약 (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                만약 (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        반환 log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        반환 0;
}

정수 pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        정수 r;

        assert(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        만약 (r < 0)
                반환 r;

        만약 (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                만약 (r < 0)
                        반환 r;

                만약 (n.fd_id != pidfd_id)
                        반환 -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        반환 0;
}

정수 pidref_set_pidstr_full(PidRef *pidref, const char *pid, unsigned flags) {
        pid_t nr;
        정수 r;

        assert(pidref);

        r = parse_pid(pid, &nr);
        만약 (r < 0)
                반환 r;

        반환 pidref_set_pid_full(pidref, nr, flags);
}

정수 pidref_set_pidfd(PidRef *pidref, 정수 fd) {
        정수 r;

        assert(pidref);

        만약 (fd < 0)
                반환 -EBADF;

        정수 fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        만약 (fd_copy < 0) {
                pid_t pid;

                만약 (!ERRNO_IS_RESOURCE(errno))
                        반환 -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                만약 (r < 0)
                        반환 r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                반환 0;
        }

        반환 pidref_set_pidfd_consume(pidref, fd_copy);
}

정수 pidref_set_pidfd_take(PidRef *pidref, 정수 fd) {
        pid_t pid;
        정수 r;

        assert(pidref);

        만약 (fd < 0)
                반환 -EBADF;

        r = pidfd_get_pid(fd, &pid);
        만약 (r < 0)
                반환 r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        반환 0;
}

정수 pidref_set_pidfd_consume(PidRef *pidref, 정수 fd) {
        정수 r;

        r = pidref_set_pidfd_take(pidref, fd);
        만약 (r < 0)
                safe_close(fd);

        반환 r;
}

정수 pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        정수 r;

        assert(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        위해 (;;) {
                r = pidref_set_pid(&parent, ppid);
                만약 (r < 0)
                        반환 r;

                만약 (parent.fd < 0) /* If pidfds are not available, then we are done */
                        break;

                pid_t now_ppid = getppid();
                만약 (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        break;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        반환 0;
}

void pidref_done(PidRef *pidref) {
        assert(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        만약 (!pidref)
                반환 NULL;

        pidref_done(pidref);
        반환 mfree(pidref);
}

정수 pidref_copy(const PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        assert(ret);

        만약 (pidref) {
                만약 (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                아니면 만약 (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        만약 (copy.fd < 0) {
                                만약 (!ERRNO_IS_RESOURCE(errno))
                                        반환 -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        반환 0;
}

정수 pidref_dup(const PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = NULL;
        정수 r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        assert(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        만약 (!dup_pidref)
                반환 -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        만약 (r < 0)
                반환 r;

        *ret = TAKE_PTR(dup_pidref);
        반환 0;
}

정수 pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = NULL;
        정수 r;

        assert(ret);

        만약 (pid < 0)
                반환 -ESRCH;

        n = new(PidRef, 1);
        만약 (!n)
                반환 -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        만약 (r < 0)
                반환 r;

        *ret = TAKE_PTR(n);
        반환 0;
}

정수 pidref_kill(const PidRef *pidref, 정수 sig) {

        만약 (!pidref)
                반환 -ESRCH;

        만약 (pidref_is_remote(pidref))
                반환 -EREMOTE;

        만약 (pidref->fd >= 0)
                반환 RET_NERRNO(pidfd_send_signal(pidref->fd, sig, NULL, 0));

        만약 (pidref->pid > 0)
                반환 RET_NERRNO(kill(pidref->pid, sig));

        반환 -ESRCH;
}

정수 pidref_kill_and_sigcont(const PidRef *pidref, 정수 sig) {
        정수 r;

        r = pidref_kill(pidref, sig);
        만약 (r < 0)
                반환 r;

        만약 (!IN_SET(sig, SIGCONT, SIGKILL))
                (void) pidref_kill(pidref, SIGCONT);

        반환 0;
}

정수 pidref_sigqueue(const PidRef *pidref, 정수 sig, 정수 value) {

        만약 (!pidref)
                반환 -ESRCH;

        만약 (pidref_is_remote(pidref))
                반환 -EREMOTE;

        만약 (pidref->fd >= 0) {
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

                반환 RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        만약 (pidref->pid > 0)
                반환 RET_NERRNO(sigqueue(pidref->pid, sig, (const union sigval) { .sival_int = value }));

        반환 -ESRCH;
}

정수 pidref_verify(const PidRef *pidref) {
        정수 r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        만약 (!pidref_is_set(pidref))
                반환 -ESRCH;

        만약 (pidref_is_remote(pidref))
                반환 -EREMOTE;

        만약 (pidref->pid == 1)
                반환 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        만약 (pidref->fd < 0)
                반환 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        만약 (r < 0)
                반환 r;

        반환 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        만약 (!pidref_is_set(pidref))
                반환 false;

        만약 (pidref_is_remote(pidref))
                반환 false;

        만약 (pidref->pid != getpid_cached())
                반환 false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        만약 (pidref->pid == 1)
                반환 true;

        /* Also compare pidfd ID if we can get it */
        만약 (pidref_acquire_pidfd_id(pidref) < 0)
                반환 true;

        uint64_t self_id;
        만약 (pidfd_get_inode_id_self_cached(&self_id) < 0)
                반환 true;

        반환 pidref->fd_id == self_id;
}

정수 pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        정수 r;

        assert(timeout > 0);

        만약 (!pidref_is_set(pidref))
                반환 -ESRCH;

        만약 (pidref_is_remote(pidref))
                반환 -EREMOTE;

        만약 (pidref->pid == 1 || pidref_is_self(pidref))
                반환 -ECHILD;

        만약 (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                반환 -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        위해 (;;) {
                만약 (poll_first) {
                        usec_t left;

                        만약 (ts == USEC_INFINITY)
                                left = USEC_INFINITY;
                        아니면 {
                                left = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                만약 (left == 0)
                                        반환 -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, left);
                        만약 (r == 0)
                                반환 -ETIMEDOUT;
                        만약 (r == -EINTR)
                                continue;
                        만약 (r < 0)
                                반환 r;
                }

                siginfo_t si = {};

                만약 (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                아니면
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                만약 (r >= 0) {
                        만약 (ret_si)
                                *ret_si = si;
                        반환 0;
                }
                만약 (r != -EINTR)
                        반환 r;
        }
}

bool pidref_is_automatic(const PidRef *pidref) {
        반환 pidref && pid_is_automatic(pidref->pid);
}

void pidref_hash_func(const PidRef *pidref, struct siphash *state) {
        assert(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

정수 pidref_compare_func(const PidRef *a, const PidRef *b) {
        정수 r;

        assert(a);
        assert(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        만약 (r != 0)
                반환 r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        만약 (r != 0)
                반환 r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        만약 (r != 0)
                반환 r;

        r = CMP(a->pid, b->pid);
        만약 (r != 0)
                반환 r;

        만약 (a->fd_id != 0 && b->fd_id != 0)
                반환 CMP(a->fd_id, b->fd_id);

        반환 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

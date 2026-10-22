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

số_nguyên pidref_acquire_pidfd_id(PidRef *pidref) {
        số_nguyên r;

        assert(pidref);

        nếu (!pidref_is_set(pidref))
                trả_về -ESRCH;

        nếu (pidref_is_remote(pidref))
                trả_về -EREMOTE;

        nếu (pidref->fd < 0)
                trả_về -ENOMEDIUM;

        nếu (pidref->fd_id > 0)
                trả_về 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        nếu (r < 0) {
                nếu (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                trả_về r;
        }

        trả_về 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        nếu (a == b)
                trả_về true;

        nếu (!pidref_is_set(a))
                trả_về !pidref_is_set(b);

        nếu (!pidref_is_set(b))
                trả_về false;

        nếu (a->pid != b->pid)
                trả_về false;

        nếu (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                nếu (!pidref_is_remote(b))
                        trả_về false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                nếu (a->fd_id == 0 || b->fd_id == 0)
                        trả_về true;
        } không_thì {
                /* If the other side is remote, then this is not the same */
                nếu (pidref_is_remote(b))
                        trả_về false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                nếu (a->pid == 1)
                        trả_về true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                nếu (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        trả_về true;
        }

        trả_về a->fd_id == b->fd_id;
}

số_nguyên pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        số_nguyên fd;

        assert(pidref);

        nếu (pid < 0)
                trả_về -ESRCH;
        nếu (pid == 0) {
                pid = getpid_cached();
                (void) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        nếu (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                nếu (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        trả_về log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        trả_về 0;
}

số_nguyên pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        số_nguyên r;

        assert(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        nếu (r < 0)
                trả_về r;

        nếu (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                nếu (r < 0)
                        trả_về r;

                nếu (n.fd_id != pidfd_id)
                        trả_về -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        trả_về 0;
}

số_nguyên pidref_set_pidstr_full(PidRef *pidref, const char *pid, unsigned flags) {
        pid_t nr;
        số_nguyên r;

        assert(pidref);

        r = parse_pid(pid, &nr);
        nếu (r < 0)
                trả_về r;

        trả_về pidref_set_pid_full(pidref, nr, flags);
}

số_nguyên pidref_set_pidfd(PidRef *pidref, số_nguyên fd) {
        số_nguyên r;

        assert(pidref);

        nếu (fd < 0)
                trả_về -EBADF;

        số_nguyên fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        nếu (fd_copy < 0) {
                pid_t pid;

                nếu (!ERRNO_IS_RESOURCE(errno))
                        trả_về -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                nếu (r < 0)
                        trả_về r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                trả_về 0;
        }

        trả_về pidref_set_pidfd_consume(pidref, fd_copy);
}

số_nguyên pidref_set_pidfd_take(PidRef *pidref, số_nguyên fd) {
        pid_t pid;
        số_nguyên r;

        assert(pidref);

        nếu (fd < 0)
                trả_về -EBADF;

        r = pidfd_get_pid(fd, &pid);
        nếu (r < 0)
                trả_về r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        trả_về 0;
}

số_nguyên pidref_set_pidfd_consume(PidRef *pidref, số_nguyên fd) {
        số_nguyên r;

        r = pidref_set_pidfd_take(pidref, fd);
        nếu (r < 0)
                safe_close(fd);

        trả_về r;
}

số_nguyên pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        số_nguyên r;

        assert(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        cho (;;) {
                r = pidref_set_pid(&parent, ppid);
                nếu (r < 0)
                        trả_về r;

                nếu (parent.fd < 0) /* If pidfds are not available, then we are done */
                        break;

                pid_t now_ppid = getppid();
                nếu (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        break;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        trả_về 0;
}

void pidref_done(PidRef *pidref) {
        assert(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        nếu (!pidref)
                trả_về NULL;

        pidref_done(pidref);
        trả_về mfree(pidref);
}

số_nguyên pidref_copy(const PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        assert(ret);

        nếu (pidref) {
                nếu (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                không_thì nếu (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        nếu (copy.fd < 0) {
                                nếu (!ERRNO_IS_RESOURCE(errno))
                                        trả_về -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        trả_về 0;
}

số_nguyên pidref_dup(const PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = NULL;
        số_nguyên r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        assert(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        nếu (!dup_pidref)
                trả_về -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        nếu (r < 0)
                trả_về r;

        *ret = TAKE_PTR(dup_pidref);
        trả_về 0;
}

số_nguyên pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = NULL;
        số_nguyên r;

        assert(ret);

        nếu (pid < 0)
                trả_về -ESRCH;

        n = new(PidRef, 1);
        nếu (!n)
                trả_về -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        nếu (r < 0)
                trả_về r;

        *ret = TAKE_PTR(n);
        trả_về 0;
}

số_nguyên pidref_kill(const PidRef *pidref, số_nguyên sig) {

        nếu (!pidref)
                trả_về -ESRCH;

        nếu (pidref_is_remote(pidref))
                trả_về -EREMOTE;

        nếu (pidref->fd >= 0)
                trả_về RET_NERRNO(pidfd_send_signal(pidref->fd, sig, NULL, 0));

        nếu (pidref->pid > 0)
                trả_về RET_NERRNO(kill(pidref->pid, sig));

        trả_về -ESRCH;
}

số_nguyên pidref_kill_and_sigcont(const PidRef *pidref, số_nguyên sig) {
        số_nguyên r;

        r = pidref_kill(pidref, sig);
        nếu (r < 0)
                trả_về r;

        nếu (!IN_SET(sig, SIGCONT, SIGKILL))
                (void) pidref_kill(pidref, SIGCONT);

        trả_về 0;
}

số_nguyên pidref_sigqueue(const PidRef *pidref, số_nguyên sig, số_nguyên value) {

        nếu (!pidref)
                trả_về -ESRCH;

        nếu (pidref_is_remote(pidref))
                trả_về -EREMOTE;

        nếu (pidref->fd >= 0) {
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

                trả_về RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        nếu (pidref->pid > 0)
                trả_về RET_NERRNO(sigqueue(pidref->pid, sig, (const union sigval) { .sival_int = value }));

        trả_về -ESRCH;
}

số_nguyên pidref_verify(const PidRef *pidref) {
        số_nguyên r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        nếu (!pidref_is_set(pidref))
                trả_về -ESRCH;

        nếu (pidref_is_remote(pidref))
                trả_về -EREMOTE;

        nếu (pidref->pid == 1)
                trả_về 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        nếu (pidref->fd < 0)
                trả_về 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        nếu (r < 0)
                trả_về r;

        trả_về 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        nếu (!pidref_is_set(pidref))
                trả_về false;

        nếu (pidref_is_remote(pidref))
                trả_về false;

        nếu (pidref->pid != getpid_cached())
                trả_về false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        nếu (pidref->pid == 1)
                trả_về true;

        /* Also compare pidfd ID if we can get it */
        nếu (pidref_acquire_pidfd_id(pidref) < 0)
                trả_về true;

        uint64_t self_id;
        nếu (pidfd_get_inode_id_self_cached(&self_id) < 0)
                trả_về true;

        trả_về pidref->fd_id == self_id;
}

số_nguyên pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        số_nguyên r;

        assert(timeout > 0);

        nếu (!pidref_is_set(pidref))
                trả_về -ESRCH;

        nếu (pidref_is_remote(pidref))
                trả_về -EREMOTE;

        nếu (pidref->pid == 1 || pidref_is_self(pidref))
                trả_về -ECHILD;

        nếu (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                trả_về -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        cho (;;) {
                nếu (poll_first) {
                        usec_t left;

                        nếu (ts == USEC_INFINITY)
                                left = USEC_INFINITY;
                        không_thì {
                                left = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                nếu (left == 0)
                                        trả_về -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, left);
                        nếu (r == 0)
                                trả_về -ETIMEDOUT;
                        nếu (r == -EINTR)
                                continue;
                        nếu (r < 0)
                                trả_về r;
                }

                siginfo_t si = {};

                nếu (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                không_thì
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                nếu (r >= 0) {
                        nếu (ret_si)
                                *ret_si = si;
                        trả_về 0;
                }
                nếu (r != -EINTR)
                        trả_về r;
        }
}

bool pidref_is_automatic(const PidRef *pidref) {
        trả_về pidref && pid_is_automatic(pidref->pid);
}

void pidref_hash_func(const PidRef *pidref, struct siphash *state) {
        assert(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

số_nguyên pidref_compare_func(const PidRef *a, const PidRef *b) {
        số_nguyên r;

        assert(a);
        assert(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        nếu (r != 0)
                trả_về r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        nếu (r != 0)
                trả_về r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        nếu (r != 0)
                trả_về r;

        r = CMP(a->pid, b->pid);
        nếu (r != 0)
                trả_về r;

        nếu (a->fd_id != 0 && b->fd_id != 0)
                trả_về CMP(a->fd_id, b->fd_id);

        trả_về 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

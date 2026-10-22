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

ཧྲིལ pidref_acquire_pidfd_id(PidRef *pidref) {
        ཧྲིལ r;

        assert(pidref);

        གལ་སྲིད (!pidref_is_set(pidref))
                ལོག -ESRCH;

        གལ་སྲིད (pidref_is_remote(pidref))
                ལོག -EREMOTE;

        གལ་སྲིད (pidref->fd < 0)
                ལོག -ENOMEDIUM;

        གལ་སྲིད (pidref->fd_id > 0)
                ལོག 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        གལ་སྲིད (r < 0) {
                གལ་སྲིད (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                ལོག r;
        }

        ལོག 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        གལ་སྲིད (a == b)
                ལོག true;

        གལ་སྲིད (!pidref_is_set(a))
                ལོག !pidref_is_set(b);

        གལ་སྲིད (!pidref_is_set(b))
                ལོག false;

        གལ་སྲིད (a->pid != b->pid)
                ལོག false;

        གལ་སྲིད (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                གལ་སྲིད (!pidref_is_remote(b))
                        ལོག false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                གལ་སྲིད (a->fd_id == 0 || b->fd_id == 0)
                        ལོག true;
        } དེ་མིན {
                /* If the other side is remote, then this is not the same */
                གལ་སྲིད (pidref_is_remote(b))
                        ལོག false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                གལ་སྲིད (a->pid == 1)
                        ལོག true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                གལ་སྲིད (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        ལོག true;
        }

        ལོག a->fd_id == b->fd_id;
}

ཧྲིལ pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        ཧྲིལ fd;

        assert(pidref);

        གལ་སྲིད (pid < 0)
                ལོག -ESRCH;
        གལ་སྲིད (pid == 0) {
                pid = getpid_cached();
                (void) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        གལ་སྲིད (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                གལ་སྲིད (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        ལོག log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        ལོག 0;
}

ཧྲིལ pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        ཧྲིལ r;

        assert(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                གལ་སྲིད (r < 0)
                        ལོག r;

                གལ་སྲིད (n.fd_id != pidfd_id)
                        ལོག -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        ལོག 0;
}

ཧྲིལ pidref_set_pidstr_full(PidRef *pidref, const var_na *pid, unsigned flags) {
        pid_t nr;
        ཧྲིལ r;

        assert(pidref);

        r = parse_pid(pid, &nr);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག pidref_set_pid_full(pidref, nr, flags);
}

ཧྲིལ pidref_set_pidfd(PidRef *pidref, ཧྲིལ fd) {
        ཧྲིལ r;

        assert(pidref);

        གལ་སྲིད (fd < 0)
                ལོག -EBADF;

        ཧྲིལ fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        གལ་སྲིད (fd_copy < 0) {
                pid_t pid;

                གལ་སྲིད (!ERRNO_IS_RESOURCE(errno))
                        ལོག -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                གལ་སྲིད (r < 0)
                        ལོག r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                ལོག 0;
        }

        ལོག pidref_set_pidfd_consume(pidref, fd_copy);
}

ཧྲིལ pidref_set_pidfd_take(PidRef *pidref, ཧྲིལ fd) {
        pid_t pid;
        ཧྲིལ r;

        assert(pidref);

        གལ་སྲིད (fd < 0)
                ལོག -EBADF;

        r = pidfd_get_pid(fd, &pid);
        གལ་སྲིད (r < 0)
                ལོག r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        ལོག 0;
}

ཧྲིལ pidref_set_pidfd_consume(PidRef *pidref, ཧྲིལ fd) {
        ཧྲིལ r;

        r = pidref_set_pidfd_take(pidref, fd);
        གལ་སྲིད (r < 0)
                safe_close(fd);

        ལོག r;
}

ཧྲིལ pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        ཧྲིལ r;

        assert(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        དོན་ལུ (;;) {
                r = pidref_set_pid(&parent, ppid);
                གལ་སྲིད (r < 0)
                        ལོག r;

                གལ་སྲིད (parent.fd < 0) /* If pidfds are not available, then we are done */
                        bha_nga;

                pid_t now_ppid = getppid();
                གལ་སྲིད (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        bha_nga;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        ལོག 0;
}

void pidref_done(PidRef *pidref) {
        assert(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        གལ་སྲིད (!pidref)
                ལོག NULL;

        pidref_done(pidref);
        ལོག mfree(pidref);
}

ཧྲིལ pidref_copy(const PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        assert(ret);

        གལ་སྲིད (pidref) {
                གལ་སྲིད (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                དེ་མིན གལ་སྲིད (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        གལ་སྲིད (copy.fd < 0) {
                                གལ་སྲིད (!ERRNO_IS_RESOURCE(errno))
                                        ལོག -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        ལོག 0;
}

ཧྲིལ pidref_dup(const PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = NULL;
        ཧྲིལ r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        assert(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        གལ་སྲིད (!dup_pidref)
                ལོག -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        གལ་སྲིད (r < 0)
                ལོག r;

        *ret = TAKE_PTR(dup_pidref);
        ལོག 0;
}

ཧྲིལ pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = NULL;
        ཧྲིལ r;

        assert(ret);

        གལ་སྲིད (pid < 0)
                ལོག -ESRCH;

        n = new(PidRef, 1);
        གལ་སྲིད (!n)
                ལོག -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        གལ་སྲིད (r < 0)
                ལོག r;

        *ret = TAKE_PTR(n);
        ལོག 0;
}

ཧྲིལ pidref_kill(const PidRef *pidref, ཧྲིལ sig) {

        གལ་སྲིད (!pidref)
                ལོག -ESRCH;

        གལ་སྲིད (pidref_is_remote(pidref))
                ལོག -EREMOTE;

        གལ་སྲིད (pidref->fd >= 0)
                ལོག RET_NERRNO(pidfd_send_signal(pidref->fd, sig, NULL, 0));

        གལ་སྲིད (pidref->pid > 0)
                ལོག RET_NERRNO(kill(pidref->pid, sig));

        ལོག -ESRCH;
}

ཧྲིལ pidref_kill_and_sigcont(const PidRef *pidref, ཧྲིལ sig) {
        ཧྲིལ r;

        r = pidref_kill(pidref, sig);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (!IN_SET(sig, SIGCONT, SIGKILL))
                (void) pidref_kill(pidref, SIGCONT);

        ལོག 0;
}

ཧྲིལ pidref_sigqueue(const PidRef *pidref, ཧྲིལ sig, ཧྲིལ value) {

        གལ་སྲིད (!pidref)
                ལོག -ESRCH;

        གལ་སྲིད (pidref_is_remote(pidref))
                ལོག -EREMOTE;

        གལ་སྲིད (pidref->fd >= 0) {
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

                ལོག RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        གལ་སྲིད (pidref->pid > 0)
                ལོག RET_NERRNO(sigqueue(pidref->pid, sig, (const sa_ngha sigval) { .sival_int = value }));

        ལོག -ESRCH;
}

ཧྲིལ pidref_verify(const PidRef *pidref) {
        ཧྲིལ r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        གལ་སྲིད (!pidref_is_set(pidref))
                ལོག -ESRCH;

        གལ་སྲིད (pidref_is_remote(pidref))
                ལོག -EREMOTE;

        གལ་སྲིད (pidref->pid == 1)
                ལོག 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        གལ་སྲིད (pidref->fd < 0)
                ལོག 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        གལ་སྲིད (!pidref_is_set(pidref))
                ལོག false;

        གལ་སྲིད (pidref_is_remote(pidref))
                ལོག false;

        གལ་སྲིད (pidref->pid != getpid_cached())
                ལོག false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        གལ་སྲིད (pidref->pid == 1)
                ལོག true;

        /* Also compare pidfd ID if we can get it */
        གལ་སྲིད (pidref_acquire_pidfd_id(pidref) < 0)
                ལོག true;

        uint64_t self_id;
        གལ་སྲིད (pidfd_get_inode_id_self_cached(&self_id) < 0)
                ལོག true;

        ལོག pidref->fd_id == self_id;
}

ཧྲིལ pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        ཧྲིལ r;

        assert(timeout > 0);

        གལ་སྲིད (!pidref_is_set(pidref))
                ལོག -ESRCH;

        གལ་སྲིད (pidref_is_remote(pidref))
                ལོག -EREMOTE;

        གལ་སྲིད (pidref->pid == 1 || pidref_is_self(pidref))
                ལོག -ECHILD;

        གལ་སྲིད (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                ལོག -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        དོན་ལུ (;;) {
                གལ་སྲིད (poll_first) {
                        usec_t left;

                        གལ་སྲིད (ts == USEC_INFINITY)
                                left = USEC_INFINITY;
                        དེ་མིན {
                                left = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                གལ་སྲིད (left == 0)
                                        ལོག -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, left);
                        གལ་སྲིད (r == 0)
                                ལོག -ETIMEDOUT;
                        གལ་སྲིད (r == -EINTR)
                                continue;
                        གལ་སྲིད (r < 0)
                                ལོག r;
                }

                siginfo_t si = {};

                གལ་སྲིད (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                དེ་མིན
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                གལ་སྲིད (r >= 0) {
                        གལ་སྲིད (ret_si)
                                *ret_si = si;
                        ལོག 0;
                }
                གལ་སྲིད (r != -EINTR)
                        ལོག r;
        }
}

bool pidref_is_automatic(const PidRef *pidref) {
        ལོག pidref && pid_is_automatic(pidref->pid);
}

void pidref_hash_func(const PidRef *pidref, sa_nracanaa siphash *state) {
        assert(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

ཧྲིལ pidref_compare_func(const PidRef *a, const PidRef *b) {
        ཧྲིལ r;

        assert(a);
        assert(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        གལ་སྲིད (r != 0)
                ལོག r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        གལ་སྲིད (r != 0)
                ལོག r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        གལ་སྲིད (r != 0)
                ལོག r;

        r = CMP(a->pid, b->pid);
        གལ་སྲིད (r != 0)
                ལོག r;

        གལ་སྲིད (a->fd_id != 0 && b->fd_id != 0)
                ལོག CMP(a->fd_id, b->fd_id);

        ལོག 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

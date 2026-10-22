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

ចំនួនគត់ pidref_acquire_pidfd_id(PidRef *pidref) {
        ចំនួនគត់ r;

        assert(pidref);

        ប្រសិនបើ (!pidref_is_set(pidref))
                ត្រឡប់ -ESRCH;

        ប្រសិនបើ (pidref_is_remote(pidref))
                ត្រឡប់ -EREMOTE;

        ប្រសិនបើ (pidref->fd < 0)
                ត្រឡប់ -ENOMEDIUM;

        ប្រសិនបើ (pidref->fd_id > 0)
                ត្រឡប់ 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        ប្រសិនបើ (r < 0) {
                ប្រសិនបើ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                ត្រឡប់ r;
        }

        ត្រឡប់ 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        ប្រសិនបើ (a == b)
                ត្រឡប់ true;

        ប្រសិនបើ (!pidref_is_set(a))
                ត្រឡប់ !pidref_is_set(b);

        ប្រសិនបើ (!pidref_is_set(b))
                ត្រឡប់ false;

        ប្រសិនបើ (a->pid != b->pid)
                ត្រឡប់ false;

        ប្រសិនបើ (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                ប្រសិនបើ (!pidref_is_remote(b))
                        ត្រឡប់ false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                ប្រសិនបើ (a->fd_id == 0 || b->fd_id == 0)
                        ត្រឡប់ true;
        } បើមិនដូច្នេះ {
                /* If the other side is remote, then this is not the same */
                ប្រសិនបើ (pidref_is_remote(b))
                        ត្រឡប់ false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                ប្រសិនបើ (a->pid == 1)
                        ត្រឡប់ true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                ប្រសិនបើ (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        ត្រឡប់ true;
        }

        ត្រឡប់ a->fd_id == b->fd_id;
}

ចំនួនគត់ pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        ចំនួនគត់ fd;

        assert(pidref);

        ប្រសិនបើ (pid < 0)
                ត្រឡប់ -ESRCH;
        ប្រសិនបើ (pid == 0) {
                pid = getpid_cached();
                (void) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        ប្រសិនបើ (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                ប្រសិនបើ (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        ត្រឡប់ log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        ត្រឡប់ 0;
}

ចំនួនគត់ pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        ចំនួនគត់ r;

        assert(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                ប្រសិនបើ (n.fd_id != pidfd_id)
                        ត្រឡប់ -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        ត្រឡប់ 0;
}

ចំនួនគត់ pidref_set_pidstr_full(PidRef *pidref, const var_na *pid, unsigned flags) {
        pid_t nr;
        ចំនួនគត់ r;

        assert(pidref);

        r = parse_pid(pid, &nr);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ pidref_set_pid_full(pidref, nr, flags);
}

ចំនួនគត់ pidref_set_pidfd(PidRef *pidref, ចំនួនគត់ fd) {
        ចំនួនគត់ r;

        assert(pidref);

        ប្រសិនបើ (fd < 0)
                ត្រឡប់ -EBADF;

        ចំនួនគត់ fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        ប្រសិនបើ (fd_copy < 0) {
                pid_t pid;

                ប្រសិនបើ (!ERRNO_IS_RESOURCE(errno))
                        ត្រឡប់ -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                ត្រឡប់ 0;
        }

        ត្រឡប់ pidref_set_pidfd_consume(pidref, fd_copy);
}

ចំនួនគត់ pidref_set_pidfd_take(PidRef *pidref, ចំនួនគត់ fd) {
        pid_t pid;
        ចំនួនគត់ r;

        assert(pidref);

        ប្រសិនបើ (fd < 0)
                ត្រឡប់ -EBADF;

        r = pidfd_get_pid(fd, &pid);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        ត្រឡប់ 0;
}

ចំនួនគត់ pidref_set_pidfd_consume(PidRef *pidref, ចំនួនគត់ fd) {
        ចំនួនគត់ r;

        r = pidref_set_pidfd_take(pidref, fd);
        ប្រសិនបើ (r < 0)
                safe_close(fd);

        ត្រឡប់ r;
}

ចំនួនគត់ pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        ចំនួនគត់ r;

        assert(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        សម្រាប់ (;;) {
                r = pidref_set_pid(&parent, ppid);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                ប្រសិនបើ (parent.fd < 0) /* If pidfds are not available, then we are done */
                        bha_nga;

                pid_t now_ppid = getppid();
                ប្រសិនបើ (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        bha_nga;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        ត្រឡប់ 0;
}

void pidref_done(PidRef *pidref) {
        assert(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        ប្រសិនបើ (!pidref)
                ត្រឡប់ NULL;

        pidref_done(pidref);
        ត្រឡប់ mfree(pidref);
}

ចំនួនគត់ pidref_copy(const PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        assert(ret);

        ប្រសិនបើ (pidref) {
                ប្រសិនបើ (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                បើមិនដូច្នេះ ប្រសិនបើ (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        ប្រសិនបើ (copy.fd < 0) {
                                ប្រសិនបើ (!ERRNO_IS_RESOURCE(errno))
                                        ត្រឡប់ -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        ត្រឡប់ 0;
}

ចំនួនគត់ pidref_dup(const PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = NULL;
        ចំនួនគត់ r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        assert(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        ប្រសិនបើ (!dup_pidref)
                ត្រឡប់ -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        *ret = TAKE_PTR(dup_pidref);
        ត្រឡប់ 0;
}

ចំនួនគត់ pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = NULL;
        ចំនួនគត់ r;

        assert(ret);

        ប្រសិនបើ (pid < 0)
                ត្រឡប់ -ESRCH;

        n = new(PidRef, 1);
        ប្រសិនបើ (!n)
                ត្រឡប់ -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        *ret = TAKE_PTR(n);
        ត្រឡប់ 0;
}

ចំនួនគត់ pidref_kill(const PidRef *pidref, ចំនួនគត់ sig) {

        ប្រសិនបើ (!pidref)
                ត្រឡប់ -ESRCH;

        ប្រសិនបើ (pidref_is_remote(pidref))
                ត្រឡប់ -EREMOTE;

        ប្រសិនបើ (pidref->fd >= 0)
                ត្រឡប់ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, NULL, 0));

        ប្រសិនបើ (pidref->pid > 0)
                ត្រឡប់ RET_NERRNO(kill(pidref->pid, sig));

        ត្រឡប់ -ESRCH;
}

ចំនួនគត់ pidref_kill_and_sigcont(const PidRef *pidref, ចំនួនគត់ sig) {
        ចំនួនគត់ r;

        r = pidref_kill(pidref, sig);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (!IN_SET(sig, SIGCONT, SIGKILL))
                (void) pidref_kill(pidref, SIGCONT);

        ត្រឡប់ 0;
}

ចំនួនគត់ pidref_sigqueue(const PidRef *pidref, ចំនួនគត់ sig, ចំនួនគត់ value) {

        ប្រសិនបើ (!pidref)
                ត្រឡប់ -ESRCH;

        ប្រសិនបើ (pidref_is_remote(pidref))
                ត្រឡប់ -EREMOTE;

        ប្រសិនបើ (pidref->fd >= 0) {
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

                ត្រឡប់ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        ប្រសិនបើ (pidref->pid > 0)
                ត្រឡប់ RET_NERRNO(sigqueue(pidref->pid, sig, (const sa_ngha sigval) { .sival_int = value }));

        ត្រឡប់ -ESRCH;
}

ចំនួនគត់ pidref_verify(const PidRef *pidref) {
        ចំនួនគត់ r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        ប្រសិនបើ (!pidref_is_set(pidref))
                ត្រឡប់ -ESRCH;

        ប្រសិនបើ (pidref_is_remote(pidref))
                ត្រឡប់ -EREMOTE;

        ប្រសិនបើ (pidref->pid == 1)
                ត្រឡប់ 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        ប្រសិនបើ (pidref->fd < 0)
                ត្រឡប់ 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        ប្រសិនបើ (!pidref_is_set(pidref))
                ត្រឡប់ false;

        ប្រសិនបើ (pidref_is_remote(pidref))
                ត្រឡប់ false;

        ប្រសិនបើ (pidref->pid != getpid_cached())
                ត្រឡប់ false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        ប្រសិនបើ (pidref->pid == 1)
                ត្រឡប់ true;

        /* Also compare pidfd ID if we can get it */
        ប្រសិនបើ (pidref_acquire_pidfd_id(pidref) < 0)
                ត្រឡប់ true;

        uint64_t self_id;
        ប្រសិនបើ (pidfd_get_inode_id_self_cached(&self_id) < 0)
                ត្រឡប់ true;

        ត្រឡប់ pidref->fd_id == self_id;
}

ចំនួនគត់ pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        ចំនួនគត់ r;

        assert(timeout > 0);

        ប្រសិនបើ (!pidref_is_set(pidref))
                ត្រឡប់ -ESRCH;

        ប្រសិនបើ (pidref_is_remote(pidref))
                ត្រឡប់ -EREMOTE;

        ប្រសិនបើ (pidref->pid == 1 || pidref_is_self(pidref))
                ត្រឡប់ -ECHILD;

        ប្រសិនបើ (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                ត្រឡប់ -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        សម្រាប់ (;;) {
                ប្រសិនបើ (poll_first) {
                        usec_t left;

                        ប្រសិនបើ (ts == USEC_INFINITY)
                                left = USEC_INFINITY;
                        បើមិនដូច្នេះ {
                                left = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                ប្រសិនបើ (left == 0)
                                        ត្រឡប់ -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, left);
                        ប្រសិនបើ (r == 0)
                                ត្រឡប់ -ETIMEDOUT;
                        ប្រសិនបើ (r == -EINTR)
                                continue;
                        ប្រសិនបើ (r < 0)
                                ត្រឡប់ r;
                }

                siginfo_t si = {};

                ប្រសិនបើ (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                បើមិនដូច្នេះ
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                ប្រសិនបើ (r >= 0) {
                        ប្រសិនបើ (ret_si)
                                *ret_si = si;
                        ត្រឡប់ 0;
                }
                ប្រសិនបើ (r != -EINTR)
                        ត្រឡប់ r;
        }
}

bool pidref_is_automatic(const PidRef *pidref) {
        ត្រឡប់ pidref && pid_is_automatic(pidref->pid);
}

void pidref_hash_func(const PidRef *pidref, sa_nracanaa siphash *state) {
        assert(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

ចំនួនគត់ pidref_compare_func(const PidRef *a, const PidRef *b) {
        ចំនួនគត់ r;

        assert(a);
        assert(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        ប្រសិនបើ (r != 0)
                ត្រឡប់ r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        ប្រសិនបើ (r != 0)
                ត្រឡប់ r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        ប្រសិនបើ (r != 0)
                ត្រឡប់ r;

        r = CMP(a->pid, b->pid);
        ប្រសិនបើ (r != 0)
                ត្រឡប់ r;

        ប្រសិនបើ (a->fd_id != 0 && b->fd_id != 0)
                ត្រឡប់ CMP(a->fd_id, b->fd_id);

        ត្រឡប់ 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

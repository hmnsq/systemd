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

පූර්ණ_සංඛ්‍යා pidref_acquire_pidfd_id(PidRef *pidref) {
        පූර්ණ_සංඛ්‍යා r;

        assert(pidref);

        නම් (!pidref_is_set(pidref))
                ආපසු -ESRCH;

        නම් (pidref_is_remote(pidref))
                ආපසු -EREMOTE;

        නම් (pidref->fd < 0)
                ආපසු -ENOMEDIUM;

        නම් (pidref->fd_id > 0)
                ආපසු 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        නම් (r < 0) {
                නම් (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                ආපසු r;
        }

        ආපසු 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        නම් (a == b)
                ආපසු true;

        නම් (!pidref_is_set(a))
                ආපසු !pidref_is_set(b);

        නම් (!pidref_is_set(b))
                ආපසු false;

        නම් (a->pid != b->pid)
                ආපසු false;

        නම් (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                නම් (!pidref_is_remote(b))
                        ආපසු false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                නම් (a->fd_id == 0 || b->fd_id == 0)
                        ආපසු true;
        } නැතහොත් {
                /* If the other side is remote, then this is not the same */
                නම් (pidref_is_remote(b))
                        ආපසු false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                නම් (a->pid == 1)
                        ආපසු true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                නම් (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        ආපසු true;
        }

        ආපසු a->fd_id == b->fd_id;
}

පූර්ණ_සංඛ්‍යා pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        පූර්ණ_සංඛ්‍යා fd;

        assert(pidref);

        නම් (pid < 0)
                ආපසු -ESRCH;
        නම් (pid == 0) {
                pid = getpid_cached();
                (void) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        නම් (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                නම් (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        ආපසු log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        පූර්ණ_සංඛ්‍යා r;

        assert(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        නම් (r < 0)
                ආපසු r;

        නම් (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                නම් (r < 0)
                        ආපසු r;

                නම් (n.fd_id != pidfd_id)
                        ආපසු -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා pidref_set_pidstr_full(PidRef *pidref, const var_na *pid, unsigned flags) {
        pid_t nr;
        පූර්ණ_සංඛ්‍යා r;

        assert(pidref);

        r = parse_pid(pid, &nr);
        නම් (r < 0)
                ආපසු r;

        ආපසු pidref_set_pid_full(pidref, nr, flags);
}

පූර්ණ_සංඛ්‍යා pidref_set_pidfd(PidRef *pidref, පූර්ණ_සංඛ්‍යා fd) {
        පූර්ණ_සංඛ්‍යා r;

        assert(pidref);

        නම් (fd < 0)
                ආපසු -EBADF;

        පූර්ණ_සංඛ්‍යා fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        නම් (fd_copy < 0) {
                pid_t pid;

                නම් (!ERRNO_IS_RESOURCE(errno))
                        ආපසු -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                නම් (r < 0)
                        ආපසු r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                ආපසු 0;
        }

        ආපසු pidref_set_pidfd_consume(pidref, fd_copy);
}

පූර්ණ_සංඛ්‍යා pidref_set_pidfd_take(PidRef *pidref, පූර්ණ_සංඛ්‍යා fd) {
        pid_t pid;
        පූර්ණ_සංඛ්‍යා r;

        assert(pidref);

        නම් (fd < 0)
                ආපසු -EBADF;

        r = pidfd_get_pid(fd, &pid);
        නම් (r < 0)
                ආපසු r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා pidref_set_pidfd_consume(PidRef *pidref, පූර්ණ_සංඛ්‍යා fd) {
        පූර්ණ_සංඛ්‍යා r;

        r = pidref_set_pidfd_take(pidref, fd);
        නම් (r < 0)
                safe_close(fd);

        ආපසු r;
}

පූර්ණ_සංඛ්‍යා pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        සඳහා (;;) {
                r = pidref_set_pid(&parent, ppid);
                නම් (r < 0)
                        ආපසු r;

                නම් (parent.fd < 0) /* If pidfds are not available, then we are done */
                        bha_nga;

                pid_t now_ppid = getppid();
                නම් (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        bha_nga;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        ආපසු 0;
}

void pidref_done(PidRef *pidref) {
        assert(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        නම් (!pidref)
                ආපසු NULL;

        pidref_done(pidref);
        ආපසු mfree(pidref);
}

පූර්ණ_සංඛ්‍යා pidref_copy(const PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        assert(ret);

        නම් (pidref) {
                නම් (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                නැතහොත් නම් (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        නම් (copy.fd < 0) {
                                නම් (!ERRNO_IS_RESOURCE(errno))
                                        ආපසු -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා pidref_dup(const PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = NULL;
        පූර්ණ_සංඛ්‍යා r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        assert(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        නම් (!dup_pidref)
                ආපසු -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        නම් (r < 0)
                ආපසු r;

        *ret = TAKE_PTR(dup_pidref);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        නම් (pid < 0)
                ආපසු -ESRCH;

        n = new(PidRef, 1);
        නම් (!n)
                ආපසු -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        නම් (r < 0)
                ආපසු r;

        *ret = TAKE_PTR(n);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා pidref_kill(const PidRef *pidref, පූර්ණ_සංඛ්‍යා sig) {

        නම් (!pidref)
                ආපසු -ESRCH;

        නම් (pidref_is_remote(pidref))
                ආපසු -EREMOTE;

        නම් (pidref->fd >= 0)
                ආපසු RET_NERRNO(pidfd_send_signal(pidref->fd, sig, NULL, 0));

        නම් (pidref->pid > 0)
                ආපසු RET_NERRNO(kill(pidref->pid, sig));

        ආපසු -ESRCH;
}

පූර්ණ_සංඛ්‍යා pidref_kill_and_sigcont(const PidRef *pidref, පූර්ණ_සංඛ්‍යා sig) {
        පූර්ණ_සංඛ්‍යා r;

        r = pidref_kill(pidref, sig);
        නම් (r < 0)
                ආපසු r;

        නම් (!IN_SET(sig, SIGCONT, SIGKILL))
                (void) pidref_kill(pidref, SIGCONT);

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා pidref_sigqueue(const PidRef *pidref, පූර්ණ_සංඛ්‍යා sig, පූර්ණ_සංඛ්‍යා value) {

        නම් (!pidref)
                ආපසු -ESRCH;

        නම් (pidref_is_remote(pidref))
                ආපසු -EREMOTE;

        නම් (pidref->fd >= 0) {
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

                ආපසු RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        නම් (pidref->pid > 0)
                ආපසු RET_NERRNO(sigqueue(pidref->pid, sig, (const sa_ngha sigval) { .sival_int = value }));

        ආපසු -ESRCH;
}

පූර්ණ_සංඛ්‍යා pidref_verify(const PidRef *pidref) {
        පූර්ණ_සංඛ්‍යා r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        නම් (!pidref_is_set(pidref))
                ආපසු -ESRCH;

        නම් (pidref_is_remote(pidref))
                ආපසු -EREMOTE;

        නම් (pidref->pid == 1)
                ආපසු 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        නම් (pidref->fd < 0)
                ආපසු 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        නම් (r < 0)
                ආපසු r;

        ආපසු 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        නම් (!pidref_is_set(pidref))
                ආපසු false;

        නම් (pidref_is_remote(pidref))
                ආපසු false;

        නම් (pidref->pid != getpid_cached())
                ආපසු false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        නම් (pidref->pid == 1)
                ආපසු true;

        /* Also compare pidfd ID if we can get it */
        නම් (pidref_acquire_pidfd_id(pidref) < 0)
                ආපසු true;

        uint64_t self_id;
        නම් (pidfd_get_inode_id_self_cached(&self_id) < 0)
                ආපසු true;

        ආපසු pidref->fd_id == self_id;
}

පූර්ණ_සංඛ්‍යා pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        පූර්ණ_සංඛ්‍යා r;

        assert(timeout > 0);

        නම් (!pidref_is_set(pidref))
                ආපසු -ESRCH;

        නම් (pidref_is_remote(pidref))
                ආපසු -EREMOTE;

        නම් (pidref->pid == 1 || pidref_is_self(pidref))
                ආපසු -ECHILD;

        නම් (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                ආපසු -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        සඳහා (;;) {
                නම් (poll_first) {
                        usec_t left;

                        නම් (ts == USEC_INFINITY)
                                left = USEC_INFINITY;
                        නැතහොත් {
                                left = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                නම් (left == 0)
                                        ආපසු -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, left);
                        නම් (r == 0)
                                ආපසු -ETIMEDOUT;
                        නම් (r == -EINTR)
                                continue;
                        නම් (r < 0)
                                ආපසු r;
                }

                siginfo_t si = {};

                නම් (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                නැතහොත්
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                නම් (r >= 0) {
                        නම් (ret_si)
                                *ret_si = si;
                        ආපසු 0;
                }
                නම් (r != -EINTR)
                        ආපසු r;
        }
}

bool pidref_is_automatic(const PidRef *pidref) {
        ආපසු pidref && pid_is_automatic(pidref->pid);
}

void pidref_hash_func(const PidRef *pidref, sa_nracanaa siphash *state) {
        assert(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

පූර්ණ_සංඛ්‍යා pidref_compare_func(const PidRef *a, const PidRef *b) {
        පූර්ණ_සංඛ්‍යා r;

        assert(a);
        assert(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        නම් (r != 0)
                ආපසු r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        නම් (r != 0)
                ආපසු r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        නම් (r != 0)
                ආපසු r;

        r = CMP(a->pid, b->pid);
        නම් (r != 0)
                ආපසු r;

        නම් (a->fd_id != 0 && b->fd_id != 0)
                ආපසු CMP(a->fd_id, b->fd_id);

        ආපසු 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

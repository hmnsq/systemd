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

ຈຳນວນເຕັມ pidref_acquire_pidfd_id(PidRef *pidref) {
        ຈຳນວນເຕັມ r;

        assert(pidref);

        ຖ້າ (!pidref_is_set(pidref))
                ສົ່ງຄືນ -ESRCH;

        ຖ້າ (pidref_is_remote(pidref))
                ສົ່ງຄືນ -EREMOTE;

        ຖ້າ (pidref->fd < 0)
                ສົ່ງຄືນ -ENOMEDIUM;

        ຖ້າ (pidref->fd_id > 0)
                ສົ່ງຄືນ 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        ຖ້າ (r < 0) {
                ຖ້າ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                ສົ່ງຄືນ r;
        }

        ສົ່ງຄືນ 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        ຖ້າ (a == b)
                ສົ່ງຄືນ true;

        ຖ້າ (!pidref_is_set(a))
                ສົ່ງຄືນ !pidref_is_set(b);

        ຖ້າ (!pidref_is_set(b))
                ສົ່ງຄືນ false;

        ຖ້າ (a->pid != b->pid)
                ສົ່ງຄືນ false;

        ຖ້າ (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                ຖ້າ (!pidref_is_remote(b))
                        ສົ່ງຄືນ false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                ຖ້າ (a->fd_id == 0 || b->fd_id == 0)
                        ສົ່ງຄືນ true;
        } ບໍ່ດັ່ງນັ້ນ {
                /* If the other side is remote, then this is not the same */
                ຖ້າ (pidref_is_remote(b))
                        ສົ່ງຄືນ false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                ຖ້າ (a->pid == 1)
                        ສົ່ງຄືນ true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                ຖ້າ (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        ສົ່ງຄືນ true;
        }

        ສົ່ງຄືນ a->fd_id == b->fd_id;
}

ຈຳນວນເຕັມ pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        ຈຳນວນເຕັມ fd;

        assert(pidref);

        ຖ້າ (pid < 0)
                ສົ່ງຄືນ -ESRCH;
        ຖ້າ (pid == 0) {
                pid = getpid_cached();
                (void) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        ຖ້າ (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                ຖ້າ (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        ສົ່ງຄືນ log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        ຈຳນວນເຕັມ r;

        assert(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                ຖ້າ (n.fd_id != pidfd_id)
                        ສົ່ງຄືນ -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ pidref_set_pidstr_full(PidRef *pidref, const var_na *pid, unsigned flags) {
        pid_t nr;
        ຈຳນວນເຕັມ r;

        assert(pidref);

        r = parse_pid(pid, &nr);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ pidref_set_pid_full(pidref, nr, flags);
}

ຈຳນວນເຕັມ pidref_set_pidfd(PidRef *pidref, ຈຳນວນເຕັມ fd) {
        ຈຳນວນເຕັມ r;

        assert(pidref);

        ຖ້າ (fd < 0)
                ສົ່ງຄືນ -EBADF;

        ຈຳນວນເຕັມ fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        ຖ້າ (fd_copy < 0) {
                pid_t pid;

                ຖ້າ (!ERRNO_IS_RESOURCE(errno))
                        ສົ່ງຄືນ -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                ສົ່ງຄືນ 0;
        }

        ສົ່ງຄືນ pidref_set_pidfd_consume(pidref, fd_copy);
}

ຈຳນວນເຕັມ pidref_set_pidfd_take(PidRef *pidref, ຈຳນວນເຕັມ fd) {
        pid_t pid;
        ຈຳນວນເຕັມ r;

        assert(pidref);

        ຖ້າ (fd < 0)
                ສົ່ງຄືນ -EBADF;

        r = pidfd_get_pid(fd, &pid);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ pidref_set_pidfd_consume(PidRef *pidref, ຈຳນວນເຕັມ fd) {
        ຈຳນວນເຕັມ r;

        r = pidref_set_pidfd_take(pidref, fd);
        ຖ້າ (r < 0)
                safe_close(fd);

        ສົ່ງຄືນ r;
}

ຈຳນວນເຕັມ pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        ຈຳນວນເຕັມ r;

        assert(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        ສຳລັບ (;;) {
                r = pidref_set_pid(&parent, ppid);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                ຖ້າ (parent.fd < 0) /* If pidfds are not available, then we are done */
                        bha_nga;

                pid_t now_ppid = getppid();
                ຖ້າ (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        bha_nga;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        ສົ່ງຄືນ 0;
}

void pidref_done(PidRef *pidref) {
        assert(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        ຖ້າ (!pidref)
                ສົ່ງຄືນ NULL;

        pidref_done(pidref);
        ສົ່ງຄືນ mfree(pidref);
}

ຈຳນວນເຕັມ pidref_copy(const PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        assert(ret);

        ຖ້າ (pidref) {
                ຖ້າ (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                ບໍ່ດັ່ງນັ້ນ ຖ້າ (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        ຖ້າ (copy.fd < 0) {
                                ຖ້າ (!ERRNO_IS_RESOURCE(errno))
                                        ສົ່ງຄືນ -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ pidref_dup(const PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = NULL;
        ຈຳນວນເຕັມ r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        assert(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        ຖ້າ (!dup_pidref)
                ສົ່ງຄືນ -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        *ret = TAKE_PTR(dup_pidref);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = NULL;
        ຈຳນວນເຕັມ r;

        assert(ret);

        ຖ້າ (pid < 0)
                ສົ່ງຄືນ -ESRCH;

        n = new(PidRef, 1);
        ຖ້າ (!n)
                ສົ່ງຄືນ -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        *ret = TAKE_PTR(n);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ pidref_kill(const PidRef *pidref, ຈຳນວນເຕັມ sig) {

        ຖ້າ (!pidref)
                ສົ່ງຄືນ -ESRCH;

        ຖ້າ (pidref_is_remote(pidref))
                ສົ່ງຄືນ -EREMOTE;

        ຖ້າ (pidref->fd >= 0)
                ສົ່ງຄືນ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, NULL, 0));

        ຖ້າ (pidref->pid > 0)
                ສົ່ງຄືນ RET_NERRNO(kill(pidref->pid, sig));

        ສົ່ງຄືນ -ESRCH;
}

ຈຳນວນເຕັມ pidref_kill_and_sigcont(const PidRef *pidref, ຈຳນວນເຕັມ sig) {
        ຈຳນວນເຕັມ r;

        r = pidref_kill(pidref, sig);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (!IN_SET(sig, SIGCONT, SIGKILL))
                (void) pidref_kill(pidref, SIGCONT);

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ pidref_sigqueue(const PidRef *pidref, ຈຳນວນເຕັມ sig, ຈຳນວນເຕັມ value) {

        ຖ້າ (!pidref)
                ສົ່ງຄືນ -ESRCH;

        ຖ້າ (pidref_is_remote(pidref))
                ສົ່ງຄືນ -EREMOTE;

        ຖ້າ (pidref->fd >= 0) {
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

                ສົ່ງຄືນ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        ຖ້າ (pidref->pid > 0)
                ສົ່ງຄືນ RET_NERRNO(sigqueue(pidref->pid, sig, (const sa_ngha sigval) { .sival_int = value }));

        ສົ່ງຄືນ -ESRCH;
}

ຈຳນວນເຕັມ pidref_verify(const PidRef *pidref) {
        ຈຳນວນເຕັມ r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        ຖ້າ (!pidref_is_set(pidref))
                ສົ່ງຄືນ -ESRCH;

        ຖ້າ (pidref_is_remote(pidref))
                ສົ່ງຄືນ -EREMOTE;

        ຖ້າ (pidref->pid == 1)
                ສົ່ງຄືນ 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        ຖ້າ (pidref->fd < 0)
                ສົ່ງຄືນ 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        ຖ້າ (!pidref_is_set(pidref))
                ສົ່ງຄືນ false;

        ຖ້າ (pidref_is_remote(pidref))
                ສົ່ງຄືນ false;

        ຖ້າ (pidref->pid != getpid_cached())
                ສົ່ງຄືນ false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        ຖ້າ (pidref->pid == 1)
                ສົ່ງຄືນ true;

        /* Also compare pidfd ID if we can get it */
        ຖ້າ (pidref_acquire_pidfd_id(pidref) < 0)
                ສົ່ງຄືນ true;

        uint64_t self_id;
        ຖ້າ (pidfd_get_inode_id_self_cached(&self_id) < 0)
                ສົ່ງຄືນ true;

        ສົ່ງຄືນ pidref->fd_id == self_id;
}

ຈຳນວນເຕັມ pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        ຈຳນວນເຕັມ r;

        assert(timeout > 0);

        ຖ້າ (!pidref_is_set(pidref))
                ສົ່ງຄືນ -ESRCH;

        ຖ້າ (pidref_is_remote(pidref))
                ສົ່ງຄືນ -EREMOTE;

        ຖ້າ (pidref->pid == 1 || pidref_is_self(pidref))
                ສົ່ງຄືນ -ECHILD;

        ຖ້າ (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                ສົ່ງຄືນ -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        ສຳລັບ (;;) {
                ຖ້າ (poll_first) {
                        usec_t left;

                        ຖ້າ (ts == USEC_INFINITY)
                                left = USEC_INFINITY;
                        ບໍ່ດັ່ງນັ້ນ {
                                left = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                ຖ້າ (left == 0)
                                        ສົ່ງຄືນ -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, left);
                        ຖ້າ (r == 0)
                                ສົ່ງຄືນ -ETIMEDOUT;
                        ຖ້າ (r == -EINTR)
                                continue;
                        ຖ້າ (r < 0)
                                ສົ່ງຄືນ r;
                }

                siginfo_t si = {};

                ຖ້າ (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                ບໍ່ດັ່ງນັ້ນ
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                ຖ້າ (r >= 0) {
                        ຖ້າ (ret_si)
                                *ret_si = si;
                        ສົ່ງຄືນ 0;
                }
                ຖ້າ (r != -EINTR)
                        ສົ່ງຄືນ r;
        }
}

bool pidref_is_automatic(const PidRef *pidref) {
        ສົ່ງຄືນ pidref && pid_is_automatic(pidref->pid);
}

void pidref_hash_func(const PidRef *pidref, sa_nracanaa siphash *state) {
        assert(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

ຈຳນວນເຕັມ pidref_compare_func(const PidRef *a, const PidRef *b) {
        ຈຳນວນເຕັມ r;

        assert(a);
        assert(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        ຖ້າ (r != 0)
                ສົ່ງຄືນ r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        ຖ້າ (r != 0)
                ສົ່ງຄືນ r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        ຖ້າ (r != 0)
                ສົ່ງຄືນ r;

        r = CMP(a->pid, b->pid);
        ຖ້າ (r != 0)
                ສົ່ງຄືນ r;

        ຖ້າ (a->fd_id != 0 && b->fd_id != 0)
                ສົ່ງຄືນ CMP(a->fd_id, b->fd_id);

        ສົ່ງຄືນ 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

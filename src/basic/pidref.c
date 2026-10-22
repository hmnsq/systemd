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

puur_naa_nka pidref_acquire_pidfd_id(PidRef *pidref) {
        puur_naa_nka r;

        assert(pidref);

        ᰀᰧ (!pidref_is_set(pidref))
                ᰀᰧᰌ -ESRCH;

        ᰀᰧ (pidref_is_remote(pidref))
                ᰀᰧᰌ -EREMOTE;

        ᰀᰧ (pidref->fd < 0)
                ᰀᰧᰌ -ENOMEDIUM;

        ᰀᰧ (pidref->fd_id > 0)
                ᰀᰧᰌ 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        ᰀᰧ (r < 0) {
                ᰀᰧ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                ᰀᰧᰌ r;
        }

        ᰀᰧᰌ 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        ᰀᰧ (a == b)
                ᰀᰧᰌ true;

        ᰀᰧ (!pidref_is_set(a))
                ᰀᰧᰌ !pidref_is_set(b);

        ᰀᰧ (!pidref_is_set(b))
                ᰀᰧᰌ false;

        ᰀᰧ (a->pid != b->pid)
                ᰀᰧᰌ false;

        ᰀᰧ (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                ᰀᰧ (!pidref_is_remote(b))
                        ᰀᰧᰌ false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                ᰀᰧ (a->fd_id == 0 || b->fd_id == 0)
                        ᰀᰧᰌ true;
        } ᰀᰧᰀ {
                /* If the other side is remote, then this is not the same */
                ᰀᰧ (pidref_is_remote(b))
                        ᰀᰧᰌ false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                ᰀᰧ (a->pid == 1)
                        ᰀᰧᰌ true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                ᰀᰧ (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        ᰀᰧᰌ true;
        }

        ᰀᰧᰌ a->fd_id == b->fd_id;
}

puur_naa_nka pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        puur_naa_nka fd;

        assert(pidref);

        ᰀᰧ (pid < 0)
                ᰀᰧᰌ -ESRCH;
        ᰀᰧ (pid == 0) {
                pid = getpid_cached();
                (void) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        ᰀᰧ (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                ᰀᰧ (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        ᰀᰧᰌ log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        ᰀᰧᰌ 0;
}

puur_naa_nka pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        puur_naa_nka r;

        assert(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;

                ᰀᰧ (n.fd_id != pidfd_id)
                        ᰀᰧᰌ -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        ᰀᰧᰌ 0;
}

puur_naa_nka pidref_set_pidstr_full(PidRef *pidref, const var_na *pid, unsigned flags) {
        pid_t nr;
        puur_naa_nka r;

        assert(pidref);

        r = parse_pid(pid, &nr);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧᰌ pidref_set_pid_full(pidref, nr, flags);
}

puur_naa_nka pidref_set_pidfd(PidRef *pidref, puur_naa_nka fd) {
        puur_naa_nka r;

        assert(pidref);

        ᰀᰧ (fd < 0)
                ᰀᰧᰌ -EBADF;

        puur_naa_nka fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        ᰀᰧ (fd_copy < 0) {
                pid_t pid;

                ᰀᰧ (!ERRNO_IS_RESOURCE(errno))
                        ᰀᰧᰌ -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                ᰀᰧᰌ 0;
        }

        ᰀᰧᰌ pidref_set_pidfd_consume(pidref, fd_copy);
}

puur_naa_nka pidref_set_pidfd_take(PidRef *pidref, puur_naa_nka fd) {
        pid_t pid;
        puur_naa_nka r;

        assert(pidref);

        ᰀᰧ (fd < 0)
                ᰀᰧᰌ -EBADF;

        r = pidfd_get_pid(fd, &pid);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        ᰀᰧᰌ 0;
}

puur_naa_nka pidref_set_pidfd_consume(PidRef *pidref, puur_naa_nka fd) {
        puur_naa_nka r;

        r = pidref_set_pidfd_take(pidref, fd);
        ᰀᰧ (r < 0)
                safe_close(fd);

        ᰀᰧᰌ r;
}

puur_naa_nka pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        puur_naa_nka r;

        assert(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        for (;;) {
                r = pidref_set_pid(&parent, ppid);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;

                ᰀᰧ (parent.fd < 0) /* If pidfds are not available, then we are done */
                        bha_nga;

                pid_t now_ppid = getppid();
                ᰀᰧ (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        bha_nga;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        ᰀᰧᰌ 0;
}

void pidref_done(PidRef *pidref) {
        assert(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        ᰀᰧ (!pidref)
                ᰀᰧᰌ NULL;

        pidref_done(pidref);
        ᰀᰧᰌ mfree(pidref);
}

puur_naa_nka pidref_copy(const PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        assert(ret);

        ᰀᰧ (pidref) {
                ᰀᰧ (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                ᰀᰧᰀ ᰀᰧ (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        ᰀᰧ (copy.fd < 0) {
                                ᰀᰧ (!ERRNO_IS_RESOURCE(errno))
                                        ᰀᰧᰌ -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        ᰀᰧᰌ 0;
}

puur_naa_nka pidref_dup(const PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = NULL;
        puur_naa_nka r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        assert(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        ᰀᰧ (!dup_pidref)
                ᰀᰧᰌ -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        *ret = TAKE_PTR(dup_pidref);
        ᰀᰧᰌ 0;
}

puur_naa_nka pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = NULL;
        puur_naa_nka r;

        assert(ret);

        ᰀᰧ (pid < 0)
                ᰀᰧᰌ -ESRCH;

        n = new(PidRef, 1);
        ᰀᰧ (!n)
                ᰀᰧᰌ -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        *ret = TAKE_PTR(n);
        ᰀᰧᰌ 0;
}

puur_naa_nka pidref_kill(const PidRef *pidref, puur_naa_nka sig) {

        ᰀᰧ (!pidref)
                ᰀᰧᰌ -ESRCH;

        ᰀᰧ (pidref_is_remote(pidref))
                ᰀᰧᰌ -EREMOTE;

        ᰀᰧ (pidref->fd >= 0)
                ᰀᰧᰌ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, NULL, 0));

        ᰀᰧ (pidref->pid > 0)
                ᰀᰧᰌ RET_NERRNO(kill(pidref->pid, sig));

        ᰀᰧᰌ -ESRCH;
}

puur_naa_nka pidref_kill_and_sigcont(const PidRef *pidref, puur_naa_nka sig) {
        puur_naa_nka r;

        r = pidref_kill(pidref, sig);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (!IN_SET(sig, SIGCONT, SIGKILL))
                (void) pidref_kill(pidref, SIGCONT);

        ᰀᰧᰌ 0;
}

puur_naa_nka pidref_sigqueue(const PidRef *pidref, puur_naa_nka sig, puur_naa_nka value) {

        ᰀᰧ (!pidref)
                ᰀᰧᰌ -ESRCH;

        ᰀᰧ (pidref_is_remote(pidref))
                ᰀᰧᰌ -EREMOTE;

        ᰀᰧ (pidref->fd >= 0) {
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

                ᰀᰧᰌ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        ᰀᰧ (pidref->pid > 0)
                ᰀᰧᰌ RET_NERRNO(sigqueue(pidref->pid, sig, (const sa_ngha sigval) { .sival_int = value }));

        ᰀᰧᰌ -ESRCH;
}

puur_naa_nka pidref_verify(const PidRef *pidref) {
        puur_naa_nka r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        ᰀᰧ (!pidref_is_set(pidref))
                ᰀᰧᰌ -ESRCH;

        ᰀᰧ (pidref_is_remote(pidref))
                ᰀᰧᰌ -EREMOTE;

        ᰀᰧ (pidref->pid == 1)
                ᰀᰧᰌ 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        ᰀᰧ (pidref->fd < 0)
                ᰀᰧᰌ 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧᰌ 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        ᰀᰧ (!pidref_is_set(pidref))
                ᰀᰧᰌ false;

        ᰀᰧ (pidref_is_remote(pidref))
                ᰀᰧᰌ false;

        ᰀᰧ (pidref->pid != getpid_cached())
                ᰀᰧᰌ false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        ᰀᰧ (pidref->pid == 1)
                ᰀᰧᰌ true;

        /* Also compare pidfd ID if we can get it */
        ᰀᰧ (pidref_acquire_pidfd_id(pidref) < 0)
                ᰀᰧᰌ true;

        uint64_t self_id;
        ᰀᰧ (pidfd_get_inode_id_self_cached(&self_id) < 0)
                ᰀᰧᰌ true;

        ᰀᰧᰌ pidref->fd_id == self_id;
}

puur_naa_nka pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        puur_naa_nka r;

        assert(timeout > 0);

        ᰀᰧ (!pidref_is_set(pidref))
                ᰀᰧᰌ -ESRCH;

        ᰀᰧ (pidref_is_remote(pidref))
                ᰀᰧᰌ -EREMOTE;

        ᰀᰧ (pidref->pid == 1 || pidref_is_self(pidref))
                ᰀᰧᰌ -ECHILD;

        ᰀᰧ (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                ᰀᰧᰌ -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        for (;;) {
                ᰀᰧ (poll_first) {
                        usec_t left;

                        ᰀᰧ (ts == USEC_INFINITY)
                                left = USEC_INFINITY;
                        ᰀᰧᰀ {
                                left = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                ᰀᰧ (left == 0)
                                        ᰀᰧᰌ -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, left);
                        ᰀᰧ (r == 0)
                                ᰀᰧᰌ -ETIMEDOUT;
                        ᰀᰧ (r == -EINTR)
                                continue;
                        ᰀᰧ (r < 0)
                                ᰀᰧᰌ r;
                }

                siginfo_t si = {};

                ᰀᰧ (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                ᰀᰧᰀ
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                ᰀᰧ (r >= 0) {
                        ᰀᰧ (ret_si)
                                *ret_si = si;
                        ᰀᰧᰌ 0;
                }
                ᰀᰧ (r != -EINTR)
                        ᰀᰧᰌ r;
        }
}

bool pidref_is_automatic(const PidRef *pidref) {
        ᰀᰧᰌ pidref && pid_is_automatic(pidref->pid);
}

void pidref_hash_func(const PidRef *pidref, sa_nracanaa siphash *state) {
        assert(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

puur_naa_nka pidref_compare_func(const PidRef *a, const PidRef *b) {
        puur_naa_nka r;

        assert(a);
        assert(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        ᰀᰧ (r != 0)
                ᰀᰧᰌ r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        ᰀᰧ (r != 0)
                ᰀᰧᰌ r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        ᰀᰧ (r != 0)
                ᰀᰧᰌ r;

        r = CMP(a->pid, b->pid);
        ᰀᰧ (r != 0)
                ᰀᰧᰌ r;

        ᰀᰧ (a->fd_id != 0 && b->fd_id != 0)
                ᰀᰧᰌ CMP(a->fd_id, b->fd_id);

        ᰀᰧᰌ 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

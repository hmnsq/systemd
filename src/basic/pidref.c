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

        ᬬᬾᬦ᭄ (!pidref_is_set(pidref))
                ᬯᬮᬶ -ESRCH;

        ᬬᬾᬦ᭄ (pidref_is_remote(pidref))
                ᬯᬮᬶ -EREMOTE;

        ᬬᬾᬦ᭄ (pidref->fd < 0)
                ᬯᬮᬶ -ENOMEDIUM;

        ᬬᬾᬦ᭄ (pidref->fd_id > 0)
                ᬯᬮᬶ 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        ᬬᬾᬦ᭄ (r < 0) {
                ᬬᬾᬦ᭄ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                ᬯᬮᬶ r;
        }

        ᬯᬮᬶ 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        ᬬᬾᬦ᭄ (a == b)
                ᬯᬮᬶ true;

        ᬬᬾᬦ᭄ (!pidref_is_set(a))
                ᬯᬮᬶ !pidref_is_set(b);

        ᬬᬾᬦ᭄ (!pidref_is_set(b))
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (a->pid != b->pid)
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                ᬬᬾᬦ᭄ (!pidref_is_remote(b))
                        ᬯᬮᬶ false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                ᬬᬾᬦ᭄ (a->fd_id == 0 || b->fd_id == 0)
                        ᬯᬮᬶ true;
        } ᬮᬶᬬ {
                /* If the other side is remote, then this is not the same */
                ᬬᬾᬦ᭄ (pidref_is_remote(b))
                        ᬯᬮᬶ false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                ᬬᬾᬦ᭄ (a->pid == 1)
                        ᬯᬮᬶ true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                ᬬᬾᬦ᭄ (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        ᬯᬮᬶ true;
        }

        ᬯᬮᬶ a->fd_id == b->fd_id;
}

puur_naa_nka pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        puur_naa_nka fd;

        assert(pidref);

        ᬬᬾᬦ᭄ (pid < 0)
                ᬯᬮᬶ -ESRCH;
        ᬬᬾᬦ᭄ (pid == 0) {
                pid = getpid_cached();
                (void) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        ᬬᬾᬦ᭄ (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                ᬬᬾᬦ᭄ (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        ᬯᬮᬶ log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        ᬯᬮᬶ 0;
}

puur_naa_nka pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        puur_naa_nka r;

        assert(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬬᬾᬦ᭄ (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                ᬬᬾᬦ᭄ (n.fd_id != pidfd_id)
                        ᬯᬮᬶ -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        ᬯᬮᬶ 0;
}

puur_naa_nka pidref_set_pidstr_full(PidRef *pidref, const var_na *pid, unsigned flags) {
        pid_t nr;
        puur_naa_nka r;

        assert(pidref);

        r = parse_pid(pid, &nr);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬯᬮᬶ pidref_set_pid_full(pidref, nr, flags);
}

puur_naa_nka pidref_set_pidfd(PidRef *pidref, puur_naa_nka fd) {
        puur_naa_nka r;

        assert(pidref);

        ᬬᬾᬦ᭄ (fd < 0)
                ᬯᬮᬶ -EBADF;

        puur_naa_nka fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        ᬬᬾᬦ᭄ (fd_copy < 0) {
                pid_t pid;

                ᬬᬾᬦ᭄ (!ERRNO_IS_RESOURCE(errno))
                        ᬯᬮᬶ -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                ᬯᬮᬶ 0;
        }

        ᬯᬮᬶ pidref_set_pidfd_consume(pidref, fd_copy);
}

puur_naa_nka pidref_set_pidfd_take(PidRef *pidref, puur_naa_nka fd) {
        pid_t pid;
        puur_naa_nka r;

        assert(pidref);

        ᬬᬾᬦ᭄ (fd < 0)
                ᬯᬮᬶ -EBADF;

        r = pidfd_get_pid(fd, &pid);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        ᬯᬮᬶ 0;
}

puur_naa_nka pidref_set_pidfd_consume(PidRef *pidref, puur_naa_nka fd) {
        puur_naa_nka r;

        r = pidref_set_pidfd_take(pidref, fd);
        ᬬᬾᬦ᭄ (r < 0)
                safe_close(fd);

        ᬯᬮᬶ r;
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
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                ᬬᬾᬦ᭄ (parent.fd < 0) /* If pidfds are not available, then we are done */
                        bha_nga;

                pid_t now_ppid = getppid();
                ᬬᬾᬦ᭄ (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        bha_nga;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        ᬯᬮᬶ 0;
}

void pidref_done(PidRef *pidref) {
        assert(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        ᬬᬾᬦ᭄ (!pidref)
                ᬯᬮᬶ NULL;

        pidref_done(pidref);
        ᬯᬮᬶ mfree(pidref);
}

puur_naa_nka pidref_copy(const PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        assert(ret);

        ᬬᬾᬦ᭄ (pidref) {
                ᬬᬾᬦ᭄ (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                ᬮᬶᬬ ᬬᬾᬦ᭄ (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        ᬬᬾᬦ᭄ (copy.fd < 0) {
                                ᬬᬾᬦ᭄ (!ERRNO_IS_RESOURCE(errno))
                                        ᬯᬮᬶ -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        ᬯᬮᬶ 0;
}

puur_naa_nka pidref_dup(const PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = NULL;
        puur_naa_nka r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        assert(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        ᬬᬾᬦ᭄ (!dup_pidref)
                ᬯᬮᬶ -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        *ret = TAKE_PTR(dup_pidref);
        ᬯᬮᬶ 0;
}

puur_naa_nka pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = NULL;
        puur_naa_nka r;

        assert(ret);

        ᬬᬾᬦ᭄ (pid < 0)
                ᬯᬮᬶ -ESRCH;

        n = new(PidRef, 1);
        ᬬᬾᬦ᭄ (!n)
                ᬯᬮᬶ -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        *ret = TAKE_PTR(n);
        ᬯᬮᬶ 0;
}

puur_naa_nka pidref_kill(const PidRef *pidref, puur_naa_nka sig) {

        ᬬᬾᬦ᭄ (!pidref)
                ᬯᬮᬶ -ESRCH;

        ᬬᬾᬦ᭄ (pidref_is_remote(pidref))
                ᬯᬮᬶ -EREMOTE;

        ᬬᬾᬦ᭄ (pidref->fd >= 0)
                ᬯᬮᬶ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, NULL, 0));

        ᬬᬾᬦ᭄ (pidref->pid > 0)
                ᬯᬮᬶ RET_NERRNO(kill(pidref->pid, sig));

        ᬯᬮᬶ -ESRCH;
}

puur_naa_nka pidref_kill_and_sigcont(const PidRef *pidref, puur_naa_nka sig) {
        puur_naa_nka r;

        r = pidref_kill(pidref, sig);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬬᬾᬦ᭄ (!IN_SET(sig, SIGCONT, SIGKILL))
                (void) pidref_kill(pidref, SIGCONT);

        ᬯᬮᬶ 0;
}

puur_naa_nka pidref_sigqueue(const PidRef *pidref, puur_naa_nka sig, puur_naa_nka value) {

        ᬬᬾᬦ᭄ (!pidref)
                ᬯᬮᬶ -ESRCH;

        ᬬᬾᬦ᭄ (pidref_is_remote(pidref))
                ᬯᬮᬶ -EREMOTE;

        ᬬᬾᬦ᭄ (pidref->fd >= 0) {
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

                ᬯᬮᬶ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        ᬬᬾᬦ᭄ (pidref->pid > 0)
                ᬯᬮᬶ RET_NERRNO(sigqueue(pidref->pid, sig, (const sa_ngha sigval) { .sival_int = value }));

        ᬯᬮᬶ -ESRCH;
}

puur_naa_nka pidref_verify(const PidRef *pidref) {
        puur_naa_nka r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        ᬬᬾᬦ᭄ (!pidref_is_set(pidref))
                ᬯᬮᬶ -ESRCH;

        ᬬᬾᬦ᭄ (pidref_is_remote(pidref))
                ᬯᬮᬶ -EREMOTE;

        ᬬᬾᬦ᭄ (pidref->pid == 1)
                ᬯᬮᬶ 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        ᬬᬾᬦ᭄ (pidref->fd < 0)
                ᬯᬮᬶ 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬯᬮᬶ 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        ᬬᬾᬦ᭄ (!pidref_is_set(pidref))
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (pidref_is_remote(pidref))
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (pidref->pid != getpid_cached())
                ᬯᬮᬶ false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        ᬬᬾᬦ᭄ (pidref->pid == 1)
                ᬯᬮᬶ true;

        /* Also compare pidfd ID if we can get it */
        ᬬᬾᬦ᭄ (pidref_acquire_pidfd_id(pidref) < 0)
                ᬯᬮᬶ true;

        uint64_t self_id;
        ᬬᬾᬦ᭄ (pidfd_get_inode_id_self_cached(&self_id) < 0)
                ᬯᬮᬶ true;

        ᬯᬮᬶ pidref->fd_id == self_id;
}

puur_naa_nka pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        puur_naa_nka r;

        assert(timeout > 0);

        ᬬᬾᬦ᭄ (!pidref_is_set(pidref))
                ᬯᬮᬶ -ESRCH;

        ᬬᬾᬦ᭄ (pidref_is_remote(pidref))
                ᬯᬮᬶ -EREMOTE;

        ᬬᬾᬦ᭄ (pidref->pid == 1 || pidref_is_self(pidref))
                ᬯᬮᬶ -ECHILD;

        ᬬᬾᬦ᭄ (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                ᬯᬮᬶ -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        for (;;) {
                ᬬᬾᬦ᭄ (poll_first) {
                        usec_t left;

                        ᬬᬾᬦ᭄ (ts == USEC_INFINITY)
                                left = USEC_INFINITY;
                        ᬮᬶᬬ {
                                left = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                ᬬᬾᬦ᭄ (left == 0)
                                        ᬯᬮᬶ -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, left);
                        ᬬᬾᬦ᭄ (r == 0)
                                ᬯᬮᬶ -ETIMEDOUT;
                        ᬬᬾᬦ᭄ (r == -EINTR)
                                continue;
                        ᬬᬾᬦ᭄ (r < 0)
                                ᬯᬮᬶ r;
                }

                siginfo_t si = {};

                ᬬᬾᬦ᭄ (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                ᬮᬶᬬ
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                ᬬᬾᬦ᭄ (r >= 0) {
                        ᬬᬾᬦ᭄ (ret_si)
                                *ret_si = si;
                        ᬯᬮᬶ 0;
                }
                ᬬᬾᬦ᭄ (r != -EINTR)
                        ᬯᬮᬶ r;
        }
}

bool pidref_is_automatic(const PidRef *pidref) {
        ᬯᬮᬶ pidref && pid_is_automatic(pidref->pid);
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
        ᬬᬾᬦ᭄ (r != 0)
                ᬯᬮᬶ r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        ᬬᬾᬦ᭄ (r != 0)
                ᬯᬮᬶ r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        ᬬᬾᬦ᭄ (r != 0)
                ᬯᬮᬶ r;

        r = CMP(a->pid, b->pid);
        ᬬᬾᬦ᭄ (r != 0)
                ᬯᬮᬶ r;

        ᬬᬾᬦ᭄ (a->fd_id != 0 && b->fd_id != 0)
                ᬯᬮᬶ CMP(a->fd_id, b->fd_id);

        ᬯᬮᬶ 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

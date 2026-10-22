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

ကိန်းပြည့် pidref_acquire_pidfd_id(PidRef *pidref) {
        ကိန်းပြည့် r;

        assert(pidref);

        အကယ်၍ (!pidref_is_set(pidref))
                ပြန်ပေး -ESRCH;

        အကယ်၍ (pidref_is_remote(pidref))
                ပြန်ပေး -EREMOTE;

        အကယ်၍ (pidref->fd < 0)
                ပြန်ပေး -ENOMEDIUM;

        အကယ်၍ (pidref->fd_id > 0)
                ပြန်ပေး 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        အကယ်၍ (r < 0) {
                အကယ်၍ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                ပြန်ပေး r;
        }

        ပြန်ပေး 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        အကယ်၍ (a == b)
                ပြန်ပေး true;

        အကယ်၍ (!pidref_is_set(a))
                ပြန်ပေး !pidref_is_set(b);

        အကယ်၍ (!pidref_is_set(b))
                ပြန်ပေး false;

        အကယ်၍ (a->pid != b->pid)
                ပြန်ပေး false;

        အကယ်၍ (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                အကယ်၍ (!pidref_is_remote(b))
                        ပြန်ပေး false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                အကယ်၍ (a->fd_id == 0 || b->fd_id == 0)
                        ပြန်ပေး true;
        } မဟုတ်ပါက {
                /* If the other side is remote, then this is not the same */
                အကယ်၍ (pidref_is_remote(b))
                        ပြန်ပေး false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                အကယ်၍ (a->pid == 1)
                        ပြန်ပေး true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                အကယ်၍ (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        ပြန်ပေး true;
        }

        ပြန်ပေး a->fd_id == b->fd_id;
}

ကိန်းပြည့် pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        ကိန်းပြည့် fd;

        assert(pidref);

        အကယ်၍ (pid < 0)
                ပြန်ပေး -ESRCH;
        အကယ်၍ (pid == 0) {
                pid = getpid_cached();
                (void) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        အကယ်၍ (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                အကယ်၍ (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        ပြန်ပေး log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        ပြန်ပေး 0;
}

ကိန်းပြည့် pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        ကိန်းပြည့် r;

        assert(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (n.fd_id != pidfd_id)
                        ပြန်ပေး -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        ပြန်ပေး 0;
}

ကိန်းပြည့် pidref_set_pidstr_full(PidRef *pidref, const var_na *pid, unsigned flags) {
        pid_t nr;
        ကိန်းပြည့် r;

        assert(pidref);

        r = parse_pid(pid, &nr);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး pidref_set_pid_full(pidref, nr, flags);
}

ကိန်းပြည့် pidref_set_pidfd(PidRef *pidref, ကိန်းပြည့် fd) {
        ကိန်းပြည့် r;

        assert(pidref);

        အကယ်၍ (fd < 0)
                ပြန်ပေး -EBADF;

        ကိန်းပြည့် fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        အကယ်၍ (fd_copy < 0) {
                pid_t pid;

                အကယ်၍ (!ERRNO_IS_RESOURCE(errno))
                        ပြန်ပေး -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                ပြန်ပေး 0;
        }

        ပြန်ပေး pidref_set_pidfd_consume(pidref, fd_copy);
}

ကိန်းပြည့် pidref_set_pidfd_take(PidRef *pidref, ကိန်းပြည့် fd) {
        pid_t pid;
        ကိန်းပြည့် r;

        assert(pidref);

        အကယ်၍ (fd < 0)
                ပြန်ပေး -EBADF;

        r = pidfd_get_pid(fd, &pid);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        ပြန်ပေး 0;
}

ကိန်းပြည့် pidref_set_pidfd_consume(PidRef *pidref, ကိန်းပြည့် fd) {
        ကိန်းပြည့် r;

        r = pidref_set_pidfd_take(pidref, fd);
        အကယ်၍ (r < 0)
                safe_close(fd);

        ပြန်ပေး r;
}

ကိန်းပြည့် pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        ကိန်းပြည့် r;

        assert(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        အတွက် (;;) {
                r = pidref_set_pid(&parent, ppid);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (parent.fd < 0) /* If pidfds are not available, then we are done */
                        bha_nga;

                pid_t now_ppid = getppid();
                အကယ်၍ (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        bha_nga;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        ပြန်ပေး 0;
}

void pidref_done(PidRef *pidref) {
        assert(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        အကယ်၍ (!pidref)
                ပြန်ပေး NULL;

        pidref_done(pidref);
        ပြန်ပေး mfree(pidref);
}

ကိန်းပြည့် pidref_copy(const PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        assert(ret);

        အကယ်၍ (pidref) {
                အကယ်၍ (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                မဟုတ်ပါက အကယ်၍ (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        အကယ်၍ (copy.fd < 0) {
                                အကယ်၍ (!ERRNO_IS_RESOURCE(errno))
                                        ပြန်ပေး -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        ပြန်ပေး 0;
}

ကိန်းပြည့် pidref_dup(const PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = NULL;
        ကိန်းပြည့် r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        assert(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        အကယ်၍ (!dup_pidref)
                ပြန်ပေး -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        *ret = TAKE_PTR(dup_pidref);
        ပြန်ပေး 0;
}

ကိန်းပြည့် pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = NULL;
        ကိန်းပြည့် r;

        assert(ret);

        အကယ်၍ (pid < 0)
                ပြန်ပေး -ESRCH;

        n = new(PidRef, 1);
        အကယ်၍ (!n)
                ပြန်ပေး -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        *ret = TAKE_PTR(n);
        ပြန်ပေး 0;
}

ကိန်းပြည့် pidref_kill(const PidRef *pidref, ကိန်းပြည့် sig) {

        အကယ်၍ (!pidref)
                ပြန်ပေး -ESRCH;

        အကယ်၍ (pidref_is_remote(pidref))
                ပြန်ပေး -EREMOTE;

        အကယ်၍ (pidref->fd >= 0)
                ပြန်ပေး RET_NERRNO(pidfd_send_signal(pidref->fd, sig, NULL, 0));

        အကယ်၍ (pidref->pid > 0)
                ပြန်ပေး RET_NERRNO(kill(pidref->pid, sig));

        ပြန်ပေး -ESRCH;
}

ကိန်းပြည့် pidref_kill_and_sigcont(const PidRef *pidref, ကိန်းပြည့် sig) {
        ကိန်းပြည့် r;

        r = pidref_kill(pidref, sig);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (!IN_SET(sig, SIGCONT, SIGKILL))
                (void) pidref_kill(pidref, SIGCONT);

        ပြန်ပေး 0;
}

ကိန်းပြည့် pidref_sigqueue(const PidRef *pidref, ကိန်းပြည့် sig, ကိန်းပြည့် value) {

        အကယ်၍ (!pidref)
                ပြန်ပေး -ESRCH;

        အကယ်၍ (pidref_is_remote(pidref))
                ပြန်ပေး -EREMOTE;

        အကယ်၍ (pidref->fd >= 0) {
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

                ပြန်ပေး RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        အကယ်၍ (pidref->pid > 0)
                ပြန်ပေး RET_NERRNO(sigqueue(pidref->pid, sig, (const sa_ngha sigval) { .sival_int = value }));

        ပြန်ပေး -ESRCH;
}

ကိန်းပြည့် pidref_verify(const PidRef *pidref) {
        ကိန်းပြည့် r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        အကယ်၍ (!pidref_is_set(pidref))
                ပြန်ပေး -ESRCH;

        အကယ်၍ (pidref_is_remote(pidref))
                ပြန်ပေး -EREMOTE;

        အကယ်၍ (pidref->pid == 1)
                ပြန်ပေး 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        အကယ်၍ (pidref->fd < 0)
                ပြန်ပေး 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        အကယ်၍ (!pidref_is_set(pidref))
                ပြန်ပေး false;

        အကယ်၍ (pidref_is_remote(pidref))
                ပြန်ပေး false;

        အကယ်၍ (pidref->pid != getpid_cached())
                ပြန်ပေး false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        အကယ်၍ (pidref->pid == 1)
                ပြန်ပေး true;

        /* Also compare pidfd ID if we can get it */
        အကယ်၍ (pidref_acquire_pidfd_id(pidref) < 0)
                ပြန်ပေး true;

        uint64_t self_id;
        အကယ်၍ (pidfd_get_inode_id_self_cached(&self_id) < 0)
                ပြန်ပေး true;

        ပြန်ပေး pidref->fd_id == self_id;
}

ကိန်းပြည့် pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        ကိန်းပြည့် r;

        assert(timeout > 0);

        အကယ်၍ (!pidref_is_set(pidref))
                ပြန်ပေး -ESRCH;

        အကယ်၍ (pidref_is_remote(pidref))
                ပြန်ပေး -EREMOTE;

        အကယ်၍ (pidref->pid == 1 || pidref_is_self(pidref))
                ပြန်ပေး -ECHILD;

        အကယ်၍ (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                ပြန်ပေး -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        အတွက် (;;) {
                အကယ်၍ (poll_first) {
                        usec_t left;

                        အကယ်၍ (ts == USEC_INFINITY)
                                left = USEC_INFINITY;
                        မဟုတ်ပါက {
                                left = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                အကယ်၍ (left == 0)
                                        ပြန်ပေး -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, left);
                        အကယ်၍ (r == 0)
                                ပြန်ပေး -ETIMEDOUT;
                        အကယ်၍ (r == -EINTR)
                                continue;
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;
                }

                siginfo_t si = {};

                အကယ်၍ (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                မဟုတ်ပါက
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                အကယ်၍ (r >= 0) {
                        အကယ်၍ (ret_si)
                                *ret_si = si;
                        ပြန်ပေး 0;
                }
                အကယ်၍ (r != -EINTR)
                        ပြန်ပေး r;
        }
}

bool pidref_is_automatic(const PidRef *pidref) {
        ပြန်ပေး pidref && pid_is_automatic(pidref->pid);
}

void pidref_hash_func(const PidRef *pidref, sa_nracanaa siphash *state) {
        assert(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

ကိန်းပြည့် pidref_compare_func(const PidRef *a, const PidRef *b) {
        ကိန်းပြည့် r;

        assert(a);
        assert(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        r = CMP(a->pid, b->pid);
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        အကယ်၍ (a->fd_id != 0 && b->fd_id != 0)
                ပြန်ပေး CMP(a->fd_id, b->fd_id);

        ပြန်ပေး 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

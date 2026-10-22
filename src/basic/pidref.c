/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <poll.h>
#અન્તર્ભાવ <sys/wait.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fiber-ops.h"
#અન્તર્ભાવ "format-util.h"
#અન્તર્ભાવ "hash-funcs.h"
#અન્તર્ભાવ "io-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "pidfd-util.h"
#અન્તર્ભાવ "pidref.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "siphash24.h"
#અન્તર્ભાવ "time-util.h"

પૂર્ણાંક pidref_acquire_pidfd_id(PidRef *pidref) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pidref);

        જો (!pidref_is_set(pidref))
                પાછા -ESRCH;

        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        જો (pidref->fd < 0)
                પાછા -ENOMEDIUM;

        જો (pidref->fd_id > 0)
                પાછા 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        જો (r < 0) {
                જો (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                પાછા r;
        }

        પાછા 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        જો (a == b)
                પાછા true;

        જો (!pidref_is_set(a))
                પાછા !pidref_is_set(b);

        જો (!pidref_is_set(b))
                પાછા false;

        જો (a->pid != b->pid)
                પાછા false;

        જો (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                જો (!pidref_is_remote(b))
                        પાછા false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                જો (a->fd_id == 0 || b->fd_id == 0)
                        પાછા true;
        } નહીંતર {
                /* If the other side is remote, then this is not the same */
                જો (pidref_is_remote(b))
                        પાછા false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                જો (a->pid == 1)
                        પાછા true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                જો (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        પાછા true;
        }

        પાછા a->fd_id == b->fd_id;
}

પૂર્ણાંક pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        પૂર્ણાંક fd;

        પ્રતિજ્ઞા(pidref);

        જો (pid < 0)
                પાછા -ESRCH;
        જો (pid == 0) {
                pid = getpid_cached();
                (શૂન્ય) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        જો (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                જો (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        પાછા log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        પાછા 0;
}

પૂર્ણાંક pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        જો (r < 0)
                પાછા r;

        જો (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                જો (r < 0)
                        પાછા r;

                જો (n.fd_id != pidfd_id)
                        પાછા -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        પાછા 0;
}

પૂર્ણાંક pidref_set_pidstr_full(PidRef *pidref, અચલ અક્ષર *pid, unsigned flags) {
        pid_t nr;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pidref);

        r = parse_pid(pid, &nr);
        જો (r < 0)
                પાછા r;

        પાછા pidref_set_pid_full(pidref, nr, flags);
}

પૂર્ણાંક pidref_set_pidfd(PidRef *pidref, પૂર્ણાંક fd) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pidref);

        જો (fd < 0)
                પાછા -EBADF;

        પૂર્ણાંક fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        જો (fd_copy < 0) {
                pid_t pid;

                જો (!ERRNO_IS_RESOURCE(errno))
                        પાછા -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                જો (r < 0)
                        પાછા r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                પાછા 0;
        }

        પાછા pidref_set_pidfd_consume(pidref, fd_copy);
}

પૂર્ણાંક pidref_set_pidfd_take(PidRef *pidref, પૂર્ણાંક fd) {
        pid_t pid;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pidref);

        જો (fd < 0)
                પાછા -EBADF;

        r = pidfd_get_pid(fd, &pid);
        જો (r < 0)
                પાછા r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        પાછા 0;
}

પૂર્ણાંક pidref_set_pidfd_consume(PidRef *pidref, પૂર્ણાંક fd) {
        પૂર્ણાંક r;

        r = pidref_set_pidfd_take(pidref, fd);
        જો (r < 0)
                safe_close(fd);

        પાછા r;
}

પૂર્ણાંક pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        માટે (;;) {
                r = pidref_set_pid(&parent, ppid);
                જો (r < 0)
                        પાછા r;

                જો (parent.fd < 0) /* If pidfds are not available, then we are done */
                        તોડો;

                pid_t now_ppid = getppid();
                જો (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        તોડો;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        પાછા 0;
}

શૂન્ય pidref_done(PidRef *pidref) {
        પ્રતિજ્ઞા(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        જો (!pidref)
                પાછા ખાલી;

        pidref_done(pidref);
        પાછા mfree(pidref);
}

પૂર્ણાંક pidref_copy(અચલ PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        પ્રતિજ્ઞા(ret);

        જો (pidref) {
                જો (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                નહીંતર જો (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        જો (copy.fd < 0) {
                                જો (!ERRNO_IS_RESOURCE(errno))
                                        પાછા -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        પાછા 0;
}

પૂર્ણાંક pidref_dup(અચલ PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = ખાલી;
        પૂર્ણાંક r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        પ્રતિજ્ઞા(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        જો (!dup_pidref)
                પાછા -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        જો (r < 0)
                પાછા r;

        *ret = TAKE_PTR(dup_pidref);
        પાછા 0;
}

પૂર્ણાંક pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        જો (pid < 0)
                પાછા -ESRCH;

        n = new(PidRef, 1);
        જો (!n)
                પાછા -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        જો (r < 0)
                પાછા r;

        *ret = TAKE_PTR(n);
        પાછા 0;
}

પૂર્ણાંક pidref_kill(અચલ PidRef *pidref, પૂર્ણાંક sig) {

        જો (!pidref)
                પાછા -ESRCH;

        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        જો (pidref->fd >= 0)
                પાછા RET_NERRNO(pidfd_send_signal(pidref->fd, sig, ખાલી, 0));

        જો (pidref->pid > 0)
                પાછા RET_NERRNO(kill(pidref->pid, sig));

        પાછા -ESRCH;
}

પૂર્ણાંક pidref_kill_and_sigcont(અચલ PidRef *pidref, પૂર્ણાંક sig) {
        પૂર્ણાંક r;

        r = pidref_kill(pidref, sig);
        જો (r < 0)
                પાછા r;

        જો (!IN_SET(sig, SIGCONT, SIGKILL))
                (શૂન્ય) pidref_kill(pidref, SIGCONT);

        પાછા 0;
}

પૂર્ણાંક pidref_sigqueue(અચલ PidRef *pidref, પૂર્ણાંક sig, પૂર્ણાંક કિંમત) {

        જો (!pidref)
                પાછા -ESRCH;

        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        જો (pidref->fd >= 0) {
                siginfo_t si;

                /* We can't use structured initialization here, since the structure contains various unions
                 * and these fields lie in overlapping (carefully aligned) unions that LLVM is allergic to
                 * allow assignments to */
                zero(si);
                si.si_signo = sig;
                si.si_code = SI_QUEUE;
                si.si_pid = getpid_cached();
                si.si_uid = getuid();
                si.si_value.sival_int = કિંમત;

                પાછા RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        જો (pidref->pid > 0)
                પાછા RET_NERRNO(sigqueue(pidref->pid, sig, (અચલ સંઘ sigval) { .sival_int = કિંમત }));

        પાછા -ESRCH;
}

પૂર્ણાંક pidref_verify(અચલ PidRef *pidref) {
        પૂર્ણાંક r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        જો (!pidref_is_set(pidref))
                પાછા -ESRCH;

        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        જો (pidref->pid == 1)
                પાછા 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        જો (pidref->fd < 0)
                પાછા 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        જો (r < 0)
                પાછા r;

        પાછા 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        જો (!pidref_is_set(pidref))
                પાછા false;

        જો (pidref_is_remote(pidref))
                પાછા false;

        જો (pidref->pid != getpid_cached())
                પાછા false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        જો (pidref->pid == 1)
                પાછા true;

        /* Also compare pidfd ID if we can get it */
        જો (pidref_acquire_pidfd_id(pidref) < 0)
                પાછા true;

        uint64_t self_id;
        જો (pidfd_get_inode_id_self_cached(&self_id) < 0)
                પાછા true;

        પાછા pidref->fd_id == self_id;
}

પૂર્ણાંક pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(timeout > 0);

        જો (!pidref_is_set(pidref))
                પાછા -ESRCH;

        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        જો (pidref->pid == 1 || pidref_is_self(pidref))
                પાછા -ECHILD;

        જો (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                પાછા -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        માટે (;;) {
                જો (poll_first) {
                        usec_t ડાબું;

                        જો (ts == USEC_INFINITY)
                                ડાબું = USEC_INFINITY;
                        નહીંતર {
                                ડાબું = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                જો (ડાબું == 0)
                                        પાછા -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, ડાબું);
                        જો (r == 0)
                                પાછા -ETIMEDOUT;
                        જો (r == -EINTR)
                                ચાલુ;
                        જો (r < 0)
                                પાછા r;
                }

                siginfo_t si = {};

                જો (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                નહીંતર
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                જો (r >= 0) {
                        જો (ret_si)
                                *ret_si = si;
                        પાછા 0;
                }
                જો (r != -EINTR)
                        પાછા r;
        }
}

bool pidref_is_automatic(અચલ PidRef *pidref) {
        પાછા pidref && pid_is_automatic(pidref->pid);
}

શૂન્ય pidref_hash_func(અચલ PidRef *pidref, સંરચના siphash *state) {
        પ્રતિજ્ઞા(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

પૂર્ણાંક pidref_compare_func(અચલ PidRef *a, અચલ PidRef *b) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        જો (r != 0)
                પાછા r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        જો (r != 0)
                પાછા r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        જો (r != 0)
                પાછા r;

        r = CMP(a->pid, b->pid);
        જો (r != 0)
                પાછા r;

        જો (a->fd_id != 0 && b->fd_id != 0)
                પાછા CMP(a->fd_id, b->fd_id);

        પાછા 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_ચાવી_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

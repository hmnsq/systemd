/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <poll.h>
#అన్తర్భావ <sys/wait.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fiber-ops.h"
#అన్తర్భావ "format-util.h"
#అన్తర్భావ "hash-funcs.h"
#అన్తర్భావ "io-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "pidfd-util.h"
#అన్తర్భావ "pidref.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "siphash24.h"
#అన్తర్భావ "time-util.h"

పూర్ణాంకం pidref_acquire_pidfd_id(PidRef *pidref) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pidref);

        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        ఒకవేళ (pidref->fd < 0)
                తిరిగి -ENOMEDIUM;

        ఒకవేళ (pidref->fd_id > 0)
                తిరిగి 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        ఒకవేళ (r < 0) {
                ఒకవేళ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                తిరిగి r;
        }

        తిరిగి 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        ఒకవేళ (a == b)
                తిరిగి true;

        ఒకవేళ (!pidref_is_సమితి(a))
                తిరిగి !pidref_is_సమితి(b);

        ఒకవేళ (!pidref_is_సమితి(b))
                తిరిగి false;

        ఒకవేళ (a->pid != b->pid)
                తిరిగి false;

        ఒకవేళ (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                ఒకవేళ (!pidref_is_remote(b))
                        తిరిగి false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                ఒకవేళ (a->fd_id == 0 || b->fd_id == 0)
                        తిరిగి true;
        } లేకపోతే {
                /* If the other side is remote, then this is not the same */
                ఒకవేళ (pidref_is_remote(b))
                        తిరిగి false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                ఒకవేళ (a->pid == 1)
                        తిరిగి true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                ఒకవేళ (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        తిరిగి true;
        }

        తిరిగి a->fd_id == b->fd_id;
}

పూర్ణాంకం pidref_సమితి_pid_నిండిన(PidRef *pidref, pid_t pid, గుర్తులేని flags) {
        uint64_t pidfdid = 0;
        పూర్ణాంకం fd;

        ప్రతిజ్ఞా(pidref);

        ఒకవేళ (pid < 0)
                తిరిగి -ESRCH;
        ఒకవేళ (pid == 0) {
                pid = getpid_cached();
                (శూన్య) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        ఒకవేళ (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                ఒకవేళ (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_సమితి(flags, PIDFD_THREAD))))
                        తిరిగి log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        తిరిగి 0;
}

పూర్ణాంకం pidref_సమితి_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pidref);

        _cleanup_(pidref_పూర్తి) PidRef n = PIDREF_NULL;
        r = pidref_సమితి_pid(&n, pid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (n.fd_id != pidfd_id)
                        తిరిగి -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        తిరిగి 0;
}

పూర్ణాంకం pidref_సమితి_pidstr_నిండిన(PidRef *pidref, స్థిరరాశి అక్షరం *pid, గుర్తులేని flags) {
        pid_t nr;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pidref);

        r = parse_pid(pid, &nr);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి pidref_సమితి_pid_నిండిన(pidref, nr, flags);
}

పూర్ణాంకం pidref_సమితి_pidfd(PidRef *pidref, పూర్ణాంకం fd) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pidref);

        ఒకవేళ (fd < 0)
                తిరిగి -EBADF;

        పూర్ణాంకం fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        ఒకవేళ (fd_copy < 0) {
                pid_t pid;

                ఒకవేళ (!ERRNO_IS_RESOURCE(errno))
                        తిరిగి -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                తిరిగి 0;
        }

        తిరిగి pidref_సమితి_pidfd_consume(pidref, fd_copy);
}

పూర్ణాంకం pidref_సమితి_pidfd_take(PidRef *pidref, పూర్ణాంకం fd) {
        pid_t pid;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pidref);

        ఒకవేళ (fd < 0)
                తిరిగి -EBADF;

        r = pidfd_get_pid(fd, &pid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        తిరిగి 0;
}

పూర్ణాంకం pidref_సమితి_pidfd_consume(PidRef *pidref, పూర్ణాంకం fd) {
        పూర్ణాంకం r;

        r = pidref_సమితి_pidfd_take(pidref, fd);
        ఒకవేళ (r < 0)
                సురక్షిత_close(fd);

        తిరిగి r;
}

పూర్ణాంకం pidref_సమితి_మాతృ(PidRef *ret) {
        _cleanup_(pidref_పూర్తి) PidRef మాతృ = PIDREF_NULL;
        pid_t ppid;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        కొరకు (;;) {
                r = pidref_సమితి_pid(&మాతృ, ppid);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (మాతృ.fd < 0) /* If pidfds are not available, then we are done */
                        ఆపు;

                pid_t now_ppid = getppid();
                ఒకవేళ (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        ఆపు;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_పూర్తి(&మాతృ);
        }

        *ret = TAKE_PIDREF(మాతృ);
        తిరిగి 0;
}

శూన్య pidref_పూర్తి(PidRef *pidref) {
        ప్రతిజ్ఞా(pidref);

        *pidref = (PidRef) {
                .fd = సురక్షిత_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        ఒకవేళ (!pidref)
                తిరిగి ఖాళీ;

        pidref_పూర్తి(pidref);
        తిరిగి mfree(pidref);
}

పూర్ణాంకం pidref_copy(స్థిరరాశి PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_పూర్తి) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        ప్రతిజ్ఞా(ret);

        ఒకవేళ (pidref) {
                ఒకవేళ (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                లేకపోతే ఒకవేళ (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        ఒకవేళ (copy.fd < 0) {
                                ఒకవేళ (!ERRNO_IS_RESOURCE(errno))
                                        తిరిగి -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        తిరిగి 0;
}

పూర్ణాంకం pidref_dup(స్థిరరాశి PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = ఖాళీ;
        పూర్ణాంకం r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        ప్రతిజ్ఞా(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        ఒకవేళ (!dup_pidref)
                తిరిగి -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = TAKE_PTR(dup_pidref);
        తిరిగి 0;
}

పూర్ణాంకం pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        ఒకవేళ (pid < 0)
                తిరిగి -ESRCH;

        n = new(PidRef, 1);
        ఒకవేళ (!n)
                తిరిగి -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_సమితి_pid(n, pid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = TAKE_PTR(n);
        తిరిగి 0;
}

పూర్ణాంకం pidref_kill(స్థిరరాశి PidRef *pidref, పూర్ణాంకం sig) {

        ఒకవేళ (!pidref)
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        ఒకవేళ (pidref->fd >= 0)
                తిరిగి RET_NERRNO(pidfd_send_signal(pidref->fd, sig, ఖాళీ, 0));

        ఒకవేళ (pidref->pid > 0)
                తిరిగి RET_NERRNO(kill(pidref->pid, sig));

        తిరిగి -ESRCH;
}

పూర్ణాంకం pidref_kill_and_sigcont(స్థిరరాశి PidRef *pidref, పూర్ణాంకం sig) {
        పూర్ణాంకం r;

        r = pidref_kill(pidref, sig);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (!లోపల_సమితి(sig, SIGCONT, SIGKILL))
                (శూన్య) pidref_kill(pidref, SIGCONT);

        తిరిగి 0;
}

పూర్ణాంకం pidref_sigqueue(స్థిరరాశి PidRef *pidref, పూర్ణాంకం sig, పూర్ణాంకం విలువ) {

        ఒకవేళ (!pidref)
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        ఒకవేళ (pidref->fd >= 0) {
                siginfo_t si;

                /* We can't use structured initialization here, since the structure contains various unions
                 * and these fields lie in overlapping (carefully aligned) unions that LLVM is allergic to
                 * allow assignments to */
                zero(si);
                si.si_signo = sig;
                si.si_code = SI_QUEUE;
                si.si_pid = getpid_cached();
                si.si_uid = getuid();
                si.si_value.sival_int = విలువ;

                తిరిగి RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        ఒకవేళ (pidref->pid > 0)
                తిరిగి RET_NERRNO(sigqueue(pidref->pid, sig, (స్థిరరాశి సంఘం sigval) { .sival_int = విలువ }));

        తిరిగి -ESRCH;
}

పూర్ణాంకం pidref_verify(స్థిరరాశి PidRef *pidref) {
        పూర్ణాంకం r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        ఒకవేళ (pidref->pid == 1)
                తిరిగి 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        ఒకవేళ (pidref->fd < 0)
                తిరిగి 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి false;

        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి false;

        ఒకవేళ (pidref->pid != getpid_cached())
                తిరిగి false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        ఒకవేళ (pidref->pid == 1)
                తిరిగి true;

        /* Also compare pidfd ID if we can get it */
        ఒకవేళ (pidref_acquire_pidfd_id(pidref) < 0)
                తిరిగి true;

        uint64_t self_id;
        ఒకవేళ (pidfd_get_inode_id_self_cached(&self_id) < 0)
                తిరిగి true;

        తిరిగి pidref->fd_id == self_id;
}

పూర్ణాంకం pidref_wait_for_terminate_నిండిన(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(timeout > 0);

        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        ఒకవేళ (pidref->pid == 1 || pidref_is_self(pidref))
                తిరిగి -ECHILD;

        ఒకవేళ (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_సమితి()))
                తిరిగి -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_కూడు(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_మొదటి = ts != USEC_INFINITY || fiber_ops_is_సమితి();

        కొరకు (;;) {
                ఒకవేళ (poll_మొదటి) {
                        usec_t ఎడమ;

                        ఒకవేళ (ts == USEC_INFINITY)
                                ఎడమ = USEC_INFINITY;
                        లేకపోతే {
                                ఎడమ = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                ఒకవేళ (ఎడమ == 0)
                                        తిరిగి -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, ఎడమ);
                        ఒకవేళ (r == 0)
                                తిరిగి -ETIMEDOUT;
                        ఒకవేళ (r == -EINTR)
                                కొనసాగు;
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                siginfo_t si = {};

                ఒకవేళ (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                లేకపోతే
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                ఒకవేళ (r >= 0) {
                        ఒకవేళ (ret_si)
                                *ret_si = si;
                        తిరిగి 0;
                }
                ఒకవేళ (r != -EINTR)
                        తిరిగి r;
        }
}

bool pidref_is_automatic(స్థిరరాశి PidRef *pidref) {
        తిరిగి pidref && pid_is_automatic(pidref->pid);
}

శూన్య pidref_hash_func(స్థిరరాశి PidRef *pidref, నిర్మాణం siphash *state) {
        ప్రతిజ్ఞా(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

పూర్ణాంకం pidref_compare_func(స్థిరరాశి PidRef *a, స్థిరరాశి PidRef *b) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        r = CMP(pidref_is_సమితి(a), pidref_is_సమితి(b));
        ఒకవేళ (r != 0)
                తిరిగి r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        ఒకవేళ (r != 0)
                తిరిగి r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        ఒకవేళ (r != 0)
                తిరిగి r;

        r = CMP(a->pid, b->pid);
        ఒకవేళ (r != 0)
                తిరిగి r;

        ఒకవేళ (a->fd_id != 0 && b->fd_id != 0)
                తిరిగి CMP(a->fd_id, b->fd_id);

        తిరిగి 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_కీలకం_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

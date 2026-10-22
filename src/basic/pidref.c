/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <poll.h>
#അന്തര്ഭാവ <sys/wait.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fiber-ops.h"
#അന്തര്ഭാവ "format-util.h"
#അന്തര്ഭാവ "hash-funcs.h"
#അന്തര്ഭാവ "io-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "pidfd-util.h"
#അന്തര്ഭാവ "pidref.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "siphash24.h"
#അന്തര്ഭാവ "time-util.h"

പൂർണ്ണസംഖ്യ pidref_acquire_pidfd_id(PidRef *pidref) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pidref);

        എങ്കിൽ (!pidref_is_set(pidref))
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;

        എങ്കിൽ (pidref->fd < 0)
                തിരികെ -ENOMEDIUM;

        എങ്കിൽ (pidref->fd_id > 0)
                തിരികെ 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        എങ്കിൽ (r < 0) {
                എങ്കിൽ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                തിരികെ r;
        }

        തിരികെ 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        എങ്കിൽ (a == b)
                തിരികെ true;

        എങ്കിൽ (!pidref_is_set(a))
                തിരികെ !pidref_is_set(b);

        എങ്കിൽ (!pidref_is_set(b))
                തിരികെ false;

        എങ്കിൽ (a->pid != b->pid)
                തിരികെ false;

        എങ്കിൽ (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                എങ്കിൽ (!pidref_is_remote(b))
                        തിരികെ false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                എങ്കിൽ (a->fd_id == 0 || b->fd_id == 0)
                        തിരികെ true;
        } അല്ലെങ്കിൽ {
                /* If the other side is remote, then this is not the same */
                എങ്കിൽ (pidref_is_remote(b))
                        തിരികെ false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                എങ്കിൽ (a->pid == 1)
                        തിരികെ true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                എങ്കിൽ (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        തിരികെ true;
        }

        തിരികെ a->fd_id == b->fd_id;
}

പൂർണ്ണസംഖ്യ pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        പൂർണ്ണസംഖ്യ fd;

        പ്രതിജ്ഞാ(pidref);

        എങ്കിൽ (pid < 0)
                തിരികെ -ESRCH;
        എങ്കിൽ (pid == 0) {
                pid = getpid_cached();
                (ശൂന്യം) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        എങ്കിൽ (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                എങ്കിൽ (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        തിരികെ log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (n.fd_id != pidfd_id)
                        തിരികെ -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidref_set_pidstr_full(PidRef *pidref, സ്ഥിരം അക്ഷരം *pid, unsigned flags) {
        pid_t nr;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pidref);

        r = parse_pid(pid, &nr);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ pidref_set_pid_full(pidref, nr, flags);
}

പൂർണ്ണസംഖ്യ pidref_set_pidfd(PidRef *pidref, പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pidref);

        എങ്കിൽ (fd < 0)
                തിരികെ -EBADF;

        പൂർണ്ണസംഖ്യ fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        എങ്കിൽ (fd_copy < 0) {
                pid_t pid;

                എങ്കിൽ (!ERRNO_IS_RESOURCE(errno))
                        തിരികെ -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                തിരികെ 0;
        }

        തിരികെ pidref_set_pidfd_consume(pidref, fd_copy);
}

പൂർണ്ണസംഖ്യ pidref_set_pidfd_take(PidRef *pidref, പൂർണ്ണസംഖ്യ fd) {
        pid_t pid;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pidref);

        എങ്കിൽ (fd < 0)
                തിരികെ -EBADF;

        r = pidfd_get_pid(fd, &pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidref_set_pidfd_consume(PidRef *pidref, പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ r;

        r = pidref_set_pidfd_take(pidref, fd);
        എങ്കിൽ (r < 0)
                safe_close(fd);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        വേണ്ടി (;;) {
                r = pidref_set_pid(&parent, ppid);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (parent.fd < 0) /* If pidfds are not available, then we are done */
                        നിർത്തുക;

                pid_t now_ppid = getppid();
                എങ്കിൽ (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        നിർത്തുക;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        തിരികെ 0;
}

ശൂന്യം pidref_done(PidRef *pidref) {
        പ്രതിജ്ഞാ(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        എങ്കിൽ (!pidref)
                തിരികെ ശൂന്യ;

        pidref_done(pidref);
        തിരികെ mfree(pidref);
}

പൂർണ്ണസംഖ്യ pidref_copy(സ്ഥിരം PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (pidref) {
                എങ്കിൽ (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                അല്ലെങ്കിൽ എങ്കിൽ (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        എങ്കിൽ (copy.fd < 0) {
                                എങ്കിൽ (!ERRNO_IS_RESOURCE(errno))
                                        തിരികെ -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidref_dup(സ്ഥിരം PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        പ്രതിജ്ഞാ(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        എങ്കിൽ (!dup_pidref)
                തിരികെ -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = TAKE_PTR(dup_pidref);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (pid < 0)
                തിരികെ -ESRCH;

        n = new(PidRef, 1);
        എങ്കിൽ (!n)
                തിരികെ -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = TAKE_PTR(n);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidref_kill(സ്ഥിരം PidRef *pidref, പൂർണ്ണസംഖ്യ sig) {

        എങ്കിൽ (!pidref)
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;

        എങ്കിൽ (pidref->fd >= 0)
                തിരികെ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, ശൂന്യ, 0));

        എങ്കിൽ (pidref->pid > 0)
                തിരികെ RET_NERRNO(kill(pidref->pid, sig));

        തിരികെ -ESRCH;
}

പൂർണ്ണസംഖ്യ pidref_kill_and_sigcont(സ്ഥിരം PidRef *pidref, പൂർണ്ണസംഖ്യ sig) {
        പൂർണ്ണസംഖ്യ r;

        r = pidref_kill(pidref, sig);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (!IN_SET(sig, SIGCONT, SIGKILL))
                (ശൂന്യം) pidref_kill(pidref, SIGCONT);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidref_sigqueue(സ്ഥിരം PidRef *pidref, പൂർണ്ണസംഖ്യ sig, പൂർണ്ണസംഖ്യ മൂല്യം) {

        എങ്കിൽ (!pidref)
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;

        എങ്കിൽ (pidref->fd >= 0) {
                siginfo_t si;

                /* We can't use structured initialization here, since the structure contains various unions
                 * and these fields lie in overlapping (carefully aligned) unions that LLVM is allergic to
                 * allow assignments to */
                zero(si);
                si.si_signo = sig;
                si.si_code = SI_QUEUE;
                si.si_pid = getpid_cached();
                si.si_uid = getuid();
                si.si_value.sival_int = മൂല്യം;

                തിരികെ RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        എങ്കിൽ (pidref->pid > 0)
                തിരികെ RET_NERRNO(sigqueue(pidref->pid, sig, (സ്ഥിരം സംഘം sigval) { .sival_int = മൂല്യം }));

        തിരികെ -ESRCH;
}

പൂർണ്ണസംഖ്യ pidref_verify(സ്ഥിരം PidRef *pidref) {
        പൂർണ്ണസംഖ്യ r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        എങ്കിൽ (!pidref_is_set(pidref))
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;

        എങ്കിൽ (pidref->pid == 1)
                തിരികെ 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        എങ്കിൽ (pidref->fd < 0)
                തിരികെ 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        എങ്കിൽ (!pidref_is_set(pidref))
                തിരികെ false;

        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ false;

        എങ്കിൽ (pidref->pid != getpid_cached())
                തിരികെ false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        എങ്കിൽ (pidref->pid == 1)
                തിരികെ true;

        /* Also compare pidfd ID if we can get it */
        എങ്കിൽ (pidref_acquire_pidfd_id(pidref) < 0)
                തിരികെ true;

        uint64_t self_id;
        എങ്കിൽ (pidfd_get_inode_id_self_cached(&self_id) < 0)
                തിരികെ true;

        തിരികെ pidref->fd_id == self_id;
}

പൂർണ്ണസംഖ്യ pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(timeout > 0);

        എങ്കിൽ (!pidref_is_set(pidref))
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;

        എങ്കിൽ (pidref->pid == 1 || pidref_is_self(pidref))
                തിരികെ -ECHILD;

        എങ്കിൽ (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                തിരികെ -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        വേണ്ടി (;;) {
                എങ്കിൽ (poll_first) {
                        usec_t ഇടത്;

                        എങ്കിൽ (ts == USEC_INFINITY)
                                ഇടത് = USEC_INFINITY;
                        അല്ലെങ്കിൽ {
                                ഇടത് = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                എങ്കിൽ (ഇടത് == 0)
                                        തിരികെ -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, ഇടത്);
                        എങ്കിൽ (r == 0)
                                തിരികെ -ETIMEDOUT;
                        എങ്കിൽ (r == -EINTR)
                                തുടരുക;
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

                siginfo_t si = {};

                എങ്കിൽ (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                അല്ലെങ്കിൽ
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                എങ്കിൽ (r >= 0) {
                        എങ്കിൽ (ret_si)
                                *ret_si = si;
                        തിരികെ 0;
                }
                എങ്കിൽ (r != -EINTR)
                        തിരികെ r;
        }
}

bool pidref_is_automatic(സ്ഥിരം PidRef *pidref) {
        തിരികെ pidref && pid_is_automatic(pidref->pid);
}

ശൂന്യം pidref_hash_func(സ്ഥിരം PidRef *pidref, ഘടന siphash *state) {
        പ്രതിജ്ഞാ(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

പൂർണ്ണസംഖ്യ pidref_compare_func(സ്ഥിരം PidRef *a, സ്ഥിരം PidRef *b) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        എങ്കിൽ (r != 0)
                തിരികെ r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        എങ്കിൽ (r != 0)
                തിരികെ r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        എങ്കിൽ (r != 0)
                തിരികെ r;

        r = CMP(a->pid, b->pid);
        എങ്കിൽ (r != 0)
                തിരികെ r;

        എങ്കിൽ (a->fd_id != 0 && b->fd_id != 0)
                തിരികെ CMP(a->fd_id, b->fd_id);

        തിരികെ 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <poll.h>
#অন্তর্ভাব <sys/wait.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fiber-ops.h"
#অন্তর্ভাব "format-util.h"
#অন্তর্ভাব "hash-funcs.h"
#অন্তর্ভাব "io-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "pidfd-util.h"
#অন্তর্ভাব "pidref.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "siphash24.h"
#অন্তর্ভাব "time-util.h"

পূর্ণসংখ্যা pidref_acquire_pidfd_id(PidRef *pidref) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(pidref);

        যদি (!pidref_is_set(pidref))
                ফেরত -ESRCH;

        যদি (pidref_is_remote(pidref))
                ফেরত -EREMOTE;

        যদি (pidref->fd < 0)
                ফেরত -ENOMEDIUM;

        যদি (pidref->fd_id > 0)
                ফেরত 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        যদি (r < 0) {
                যদি (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                ফেরত r;
        }

        ফেরত 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        যদি (a == b)
                ফেরত true;

        যদি (!pidref_is_set(a))
                ফেরত !pidref_is_set(b);

        যদি (!pidref_is_set(b))
                ফেরত false;

        যদি (a->pid != b->pid)
                ফেরত false;

        যদি (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                যদি (!pidref_is_remote(b))
                        ফেরত false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                যদি (a->fd_id == 0 || b->fd_id == 0)
                        ফেরত true;
        } অন্যথা {
                /* If the other side is remote, then this is not the same */
                যদি (pidref_is_remote(b))
                        ফেরত false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                যদি (a->pid == 1)
                        ফেরত true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                যদি (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        ফেরত true;
        }

        ফেরত a->fd_id == b->fd_id;
}

পূর্ণসংখ্যা pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        পূর্ণসংখ্যা fd;

        প্রতিজ্ঞা(pidref);

        যদি (pid < 0)
                ফেরত -ESRCH;
        যদি (pid == 0) {
                pid = getpid_cached();
                (শূন্য) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        যদি (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                যদি (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        ফেরত log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        ফেরত 0;
}

পূর্ণসংখ্যা pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        যদি (r < 0)
                ফেরত r;

        যদি (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                যদি (r < 0)
                        ফেরত r;

                যদি (n.fd_id != pidfd_id)
                        ফেরত -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        ফেরত 0;
}

পূর্ণসংখ্যা pidref_set_pidstr_full(PidRef *pidref, ধ্রুবক অক্ষর *pid, unsigned flags) {
        pid_t nr;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(pidref);

        r = parse_pid(pid, &nr);
        যদি (r < 0)
                ফেরত r;

        ফেরত pidref_set_pid_full(pidref, nr, flags);
}

পূর্ণসংখ্যা pidref_set_pidfd(PidRef *pidref, পূর্ণসংখ্যা fd) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(pidref);

        যদি (fd < 0)
                ফেরত -EBADF;

        পূর্ণসংখ্যা fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        যদি (fd_copy < 0) {
                pid_t pid;

                যদি (!ERRNO_IS_RESOURCE(errno))
                        ফেরত -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                যদি (r < 0)
                        ফেরত r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                ফেরত 0;
        }

        ফেরত pidref_set_pidfd_consume(pidref, fd_copy);
}

পূর্ণসংখ্যা pidref_set_pidfd_take(PidRef *pidref, পূর্ণসংখ্যা fd) {
        pid_t pid;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(pidref);

        যদি (fd < 0)
                ফেরত -EBADF;

        r = pidfd_get_pid(fd, &pid);
        যদি (r < 0)
                ফেরত r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        ফেরত 0;
}

পূর্ণসংখ্যা pidref_set_pidfd_consume(PidRef *pidref, পূর্ণসংখ্যা fd) {
        পূর্ণসংখ্যা r;

        r = pidref_set_pidfd_take(pidref, fd);
        যদি (r < 0)
                safe_close(fd);

        ফেরত r;
}

পূর্ণসংখ্যা pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        জন্য (;;) {
                r = pidref_set_pid(&parent, ppid);
                যদি (r < 0)
                        ফেরত r;

                যদি (parent.fd < 0) /* If pidfds are not available, then we are done */
                        বিরতি;

                pid_t now_ppid = getppid();
                যদি (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        বিরতি;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        ফেরত 0;
}

শূন্য pidref_done(PidRef *pidref) {
        প্রতিজ্ঞা(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        যদি (!pidref)
                ফেরত ফাঁকা;

        pidref_done(pidref);
        ফেরত mfree(pidref);
}

পূর্ণসংখ্যা pidref_copy(ধ্রুবক PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        প্রতিজ্ঞা(ret);

        যদি (pidref) {
                যদি (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                অন্যথা যদি (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        যদি (copy.fd < 0) {
                                যদি (!ERRNO_IS_RESOURCE(errno))
                                        ফেরত -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        ফেরত 0;
}

পূর্ণসংখ্যা pidref_dup(ধ্রুবক PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = ফাঁকা;
        পূর্ণসংখ্যা r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        প্রতিজ্ঞা(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        যদি (!dup_pidref)
                ফেরত -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        যদি (r < 0)
                ফেরত r;

        *ret = TAKE_PTR(dup_pidref);
        ফেরত 0;
}

পূর্ণসংখ্যা pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        যদি (pid < 0)
                ফেরত -ESRCH;

        n = new(PidRef, 1);
        যদি (!n)
                ফেরত -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        যদি (r < 0)
                ফেরত r;

        *ret = TAKE_PTR(n);
        ফেরত 0;
}

পূর্ণসংখ্যা pidref_kill(ধ্রুবক PidRef *pidref, পূর্ণসংখ্যা sig) {

        যদি (!pidref)
                ফেরত -ESRCH;

        যদি (pidref_is_remote(pidref))
                ফেরত -EREMOTE;

        যদি (pidref->fd >= 0)
                ফেরত RET_NERRNO(pidfd_send_signal(pidref->fd, sig, ফাঁকা, 0));

        যদি (pidref->pid > 0)
                ফেরত RET_NERRNO(kill(pidref->pid, sig));

        ফেরত -ESRCH;
}

পূর্ণসংখ্যা pidref_kill_and_sigcont(ধ্রুবক PidRef *pidref, পূর্ণসংখ্যা sig) {
        পূর্ণসংখ্যা r;

        r = pidref_kill(pidref, sig);
        যদি (r < 0)
                ফেরত r;

        যদি (!IN_SET(sig, SIGCONT, SIGKILL))
                (শূন্য) pidref_kill(pidref, SIGCONT);

        ফেরত 0;
}

পূর্ণসংখ্যা pidref_sigqueue(ধ্রুবক PidRef *pidref, পূর্ণসংখ্যা sig, পূর্ণসংখ্যা মান) {

        যদি (!pidref)
                ফেরত -ESRCH;

        যদি (pidref_is_remote(pidref))
                ফেরত -EREMOTE;

        যদি (pidref->fd >= 0) {
                siginfo_t si;

                /* We can't use structured initialization here, since the structure contains various unions
                 * and these fields lie in overlapping (carefully aligned) unions that LLVM is allergic to
                 * allow assignments to */
                zero(si);
                si.si_signo = sig;
                si.si_code = SI_QUEUE;
                si.si_pid = getpid_cached();
                si.si_uid = getuid();
                si.si_value.sival_int = মান;

                ফেরত RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        যদি (pidref->pid > 0)
                ফেরত RET_NERRNO(sigqueue(pidref->pid, sig, (ধ্রুবক সংঘ sigval) { .sival_int = মান }));

        ফেরত -ESRCH;
}

পূর্ণসংখ্যা pidref_verify(ধ্রুবক PidRef *pidref) {
        পূর্ণসংখ্যা r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        যদি (!pidref_is_set(pidref))
                ফেরত -ESRCH;

        যদি (pidref_is_remote(pidref))
                ফেরত -EREMOTE;

        যদি (pidref->pid == 1)
                ফেরত 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        যদি (pidref->fd < 0)
                ফেরত 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        যদি (r < 0)
                ফেরত r;

        ফেরত 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        যদি (!pidref_is_set(pidref))
                ফেরত false;

        যদি (pidref_is_remote(pidref))
                ফেরত false;

        যদি (pidref->pid != getpid_cached())
                ফেরত false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        যদি (pidref->pid == 1)
                ফেরত true;

        /* Also compare pidfd ID if we can get it */
        যদি (pidref_acquire_pidfd_id(pidref) < 0)
                ফেরত true;

        uint64_t self_id;
        যদি (pidfd_get_inode_id_self_cached(&self_id) < 0)
                ফেরত true;

        ফেরত pidref->fd_id == self_id;
}

পূর্ণসংখ্যা pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(timeout > 0);

        যদি (!pidref_is_set(pidref))
                ফেরত -ESRCH;

        যদি (pidref_is_remote(pidref))
                ফেরত -EREMOTE;

        যদি (pidref->pid == 1 || pidref_is_self(pidref))
                ফেরত -ECHILD;

        যদি (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                ফেরত -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        জন্য (;;) {
                যদি (poll_first) {
                        usec_t বাম;

                        যদি (ts == USEC_INFINITY)
                                বাম = USEC_INFINITY;
                        অন্যথা {
                                বাম = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                যদি (বাম == 0)
                                        ফেরত -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, বাম);
                        যদি (r == 0)
                                ফেরত -ETIMEDOUT;
                        যদি (r == -EINTR)
                                চালু;
                        যদি (r < 0)
                                ফেরত r;
                }

                siginfo_t si = {};

                যদি (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                অন্যথা
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                যদি (r >= 0) {
                        যদি (ret_si)
                                *ret_si = si;
                        ফেরত 0;
                }
                যদি (r != -EINTR)
                        ফেরত r;
        }
}

bool pidref_is_automatic(ধ্রুবক PidRef *pidref) {
        ফেরত pidref && pid_is_automatic(pidref->pid);
}

শূন্য pidref_hash_func(ধ্রুবক PidRef *pidref, কাঠামো siphash *state) {
        প্রতিজ্ঞা(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

পূর্ণসংখ্যা pidref_compare_func(ধ্রুবক PidRef *a, ধ্রুবক PidRef *b) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        যদি (r != 0)
                ফেরত r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        যদি (r != 0)
                ফেরত r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        যদি (r != 0)
                ফেরত r;

        r = CMP(a->pid, b->pid);
        যদি (r != 0)
                ফেরত r;

        যদি (a->fd_id != 0 && b->fd_id != 0)
                ফেরত CMP(a->fd_id, b->fd_id);

        ফেরত 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

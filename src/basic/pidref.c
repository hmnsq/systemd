/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <poll.h>
#अन्तर्भाव <sys/wait.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fiber-ops.h"
#अन्तर्भाव "format-util.h"
#अन्तर्भाव "hash-funcs.h"
#अन्तर्भाव "io-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "pidfd-util.h"
#अन्तर्भाव "pidref.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "siphash24.h"
#अन्तर्भाव "time-util.h"

पूर्णाङ्क pidref_acquire_pidfd_id(PidRef *pidref) {
        पूर्णाङ्क r;

        प्रतिज्ञा(pidref);

        सचे (!pidref_is_set(pidref))
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pidref))
                पटिनिवत्त -EREMOTE;

        सचे (pidref->fd < 0)
                पटिनिवत्त -ENOMEDIUM;

        सचे (pidref->fd_id > 0)
                पटिनिवत्त 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        सचे (r < 0) {
                सचे (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                पटिनिवत्त r;
        }

        पटिनिवत्त 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        सचे (a == b)
                पटिनिवत्त true;

        सचे (!pidref_is_set(a))
                पटिनिवत्त !pidref_is_set(b);

        सचे (!pidref_is_set(b))
                पटिनिवत्त false;

        सचे (a->pid != b->pid)
                पटिनिवत्त false;

        सचे (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                सचे (!pidref_is_remote(b))
                        पटिनिवत्त false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                सचे (a->fd_id == 0 || b->fd_id == 0)
                        पटिनिवत्त true;
        } अञ्ञथा {
                /* If the other side is remote, then this is not the same */
                सचे (pidref_is_remote(b))
                        पटिनिवत्त false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                सचे (a->pid == 1)
                        पटिनिवत्त true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                सचे (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        पटिनिवत्त true;
        }

        पटिनिवत्त a->fd_id == b->fd_id;
}

पूर्णाङ्क pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        पूर्णाङ्क fd;

        प्रतिज्ञा(pidref);

        सचे (pid < 0)
                पटिनिवत्त -ESRCH;
        सचे (pid == 0) {
                pid = getpid_cached();
                (शून्य) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        सचे (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                सचे (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        पटिनिवत्त log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        पटिनिवत्त 0;
}

पूर्णाङ्क pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        पूर्णाङ्क r;

        प्रतिज्ञा(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (n.fd_id != pidfd_id)
                        पटिनिवत्त -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        पटिनिवत्त 0;
}

पूर्णाङ्क pidref_set_pidstr_full(PidRef *pidref, ध्रुव वर्ण *pid, unsigned flags) {
        pid_t nr;
        पूर्णाङ्क r;

        प्रतिज्ञा(pidref);

        r = parse_pid(pid, &nr);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त pidref_set_pid_full(pidref, nr, flags);
}

पूर्णाङ्क pidref_set_pidfd(PidRef *pidref, पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        प्रतिज्ञा(pidref);

        सचे (fd < 0)
                पटिनिवत्त -EBADF;

        पूर्णाङ्क fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        सचे (fd_copy < 0) {
                pid_t pid;

                सचे (!ERRNO_IS_RESOURCE(errno))
                        पटिनिवत्त -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                सचे (r < 0)
                        पटिनिवत्त r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                पटिनिवत्त 0;
        }

        पटिनिवत्त pidref_set_pidfd_consume(pidref, fd_copy);
}

पूर्णाङ्क pidref_set_pidfd_take(PidRef *pidref, पूर्णाङ्क fd) {
        pid_t pid;
        पूर्णाङ्क r;

        प्रतिज्ञा(pidref);

        सचे (fd < 0)
                पटिनिवत्त -EBADF;

        r = pidfd_get_pid(fd, &pid);
        सचे (r < 0)
                पटिनिवत्त r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        पटिनिवत्त 0;
}

पूर्णाङ्क pidref_set_pidfd_consume(PidRef *pidref, पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        r = pidref_set_pidfd_take(pidref, fd);
        सचे (r < 0)
                safe_close(fd);

        पटिनिवत्त r;
}

पूर्णाङ्क pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        अत्थाय (;;) {
                r = pidref_set_pid(&parent, ppid);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (parent.fd < 0) /* If pidfds are not available, then we are done */
                        भङ्ग;

                pid_t now_ppid = getppid();
                सचे (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        भङ्ग;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        पटिनिवत्त 0;
}

शून्य pidref_done(PidRef *pidref) {
        प्रतिज्ञा(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        सचे (!pidref)
                पटिनिवत्त रिक्त;

        pidref_done(pidref);
        पटिनिवत्त mfree(pidref);
}

पूर्णाङ्क pidref_copy(ध्रुव PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        प्रतिज्ञा(ret);

        सचे (pidref) {
                सचे (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                अञ्ञथा सचे (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        सचे (copy.fd < 0) {
                                सचे (!ERRNO_IS_RESOURCE(errno))
                                        पटिनिवत्त -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        पटिनिवत्त 0;
}

पूर्णाङ्क pidref_dup(ध्रुव PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = रिक्त;
        पूर्णाङ्क r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        प्रतिज्ञा(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        सचे (!dup_pidref)
                पटिनिवत्त -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        सचे (r < 0)
                पटिनिवत्त r;

        *ret = TAKE_PTR(dup_pidref);
        पटिनिवत्त 0;
}

पूर्णाङ्क pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        सचे (pid < 0)
                पटिनिवत्त -ESRCH;

        n = new(PidRef, 1);
        सचे (!n)
                पटिनिवत्त -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        सचे (r < 0)
                पटिनिवत्त r;

        *ret = TAKE_PTR(n);
        पटिनिवत्त 0;
}

पूर्णाङ्क pidref_kill(ध्रुव PidRef *pidref, पूर्णाङ्क sig) {

        सचे (!pidref)
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pidref))
                पटिनिवत्त -EREMOTE;

        सचे (pidref->fd >= 0)
                पटिनिवत्त RET_NERRNO(pidfd_send_signal(pidref->fd, sig, रिक्त, 0));

        सचे (pidref->pid > 0)
                पटिनिवत्त RET_NERRNO(kill(pidref->pid, sig));

        पटिनिवत्त -ESRCH;
}

पूर्णाङ्क pidref_kill_and_sigcont(ध्रुव PidRef *pidref, पूर्णाङ्क sig) {
        पूर्णाङ्क r;

        r = pidref_kill(pidref, sig);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (!IN_SET(sig, SIGCONT, SIGKILL))
                (शून्य) pidref_kill(pidref, SIGCONT);

        पटिनिवत्त 0;
}

पूर्णाङ्क pidref_sigqueue(ध्रुव PidRef *pidref, पूर्णाङ्क sig, पूर्णाङ्क मानम्) {

        सचे (!pidref)
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pidref))
                पटिनिवत्त -EREMOTE;

        सचे (pidref->fd >= 0) {
                siginfo_t si;

                /* We can't use structured initialization here, since the structure contains various unions
                 * and these fields lie in overlapping (carefully aligned) unions that LLVM is allergic to
                 * allow assignments to */
                zero(si);
                si.si_signo = sig;
                si.si_code = SI_QUEUE;
                si.si_pid = getpid_cached();
                si.si_uid = getuid();
                si.si_value.sival_int = मानम्;

                पटिनिवत्त RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        सचे (pidref->pid > 0)
                पटिनिवत्त RET_NERRNO(sigqueue(pidref->pid, sig, (ध्रुव सङ्घ sigval) { .sival_int = मानम् }));

        पटिनिवत्त -ESRCH;
}

पूर्णाङ्क pidref_verify(ध्रुव PidRef *pidref) {
        पूर्णाङ्क r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        सचे (!pidref_is_set(pidref))
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pidref))
                पटिनिवत्त -EREMOTE;

        सचे (pidref->pid == 1)
                पटिनिवत्त 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        सचे (pidref->fd < 0)
                पटिनिवत्त 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        सचे (!pidref_is_set(pidref))
                पटिनिवत्त false;

        सचे (pidref_is_remote(pidref))
                पटिनिवत्त false;

        सचे (pidref->pid != getpid_cached())
                पटिनिवत्त false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        सचे (pidref->pid == 1)
                पटिनिवत्त true;

        /* Also compare pidfd ID if we can get it */
        सचे (pidref_acquire_pidfd_id(pidref) < 0)
                पटिनिवत्त true;

        uint64_t self_id;
        सचे (pidfd_get_inode_id_self_cached(&self_id) < 0)
                पटिनिवत्त true;

        पटिनिवत्त pidref->fd_id == self_id;
}

पूर्णाङ्क pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        पूर्णाङ्क r;

        प्रतिज्ञा(timeout > 0);

        सचे (!pidref_is_set(pidref))
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pidref))
                पटिनिवत्त -EREMOTE;

        सचे (pidref->pid == 1 || pidref_is_self(pidref))
                पटिनिवत्त -ECHILD;

        सचे (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                पटिनिवत्त -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        अत्थाय (;;) {
                सचे (poll_first) {
                        usec_t वामः;

                        सचे (ts == USEC_INFINITY)
                                वामः = USEC_INFINITY;
                        अञ्ञथा {
                                वामः = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                सचे (वामः == 0)
                                        पटिनिवत्त -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, वामः);
                        सचे (r == 0)
                                पटिनिवत्त -ETIMEDOUT;
                        सचे (r == -EINTR)
                                अनुवर्त;
                        सचे (r < 0)
                                पटिनिवत्त r;
                }

                siginfo_t si = {};

                सचे (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                अञ्ञथा
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                सचे (r >= 0) {
                        सचे (ret_si)
                                *ret_si = si;
                        पटिनिवत्त 0;
                }
                सचे (r != -EINTR)
                        पटिनिवत्त r;
        }
}

bool pidref_is_automatic(ध्रुव PidRef *pidref) {
        पटिनिवत्त pidref && pid_is_automatic(pidref->pid);
}

शून्य pidref_hash_func(ध्रुव PidRef *pidref, संरचना siphash *state) {
        प्रतिज्ञा(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

पूर्णाङ्क pidref_compare_func(ध्रुव PidRef *a, ध्रुव PidRef *b) {
        पूर्णाङ्क r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        सचे (r != 0)
                पटिनिवत्त r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        सचे (r != 0)
                पटिनिवत्त r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        सचे (r != 0)
                पटिनिवत्त r;

        r = CMP(a->pid, b->pid);
        सचे (r != 0)
                पटिनिवत्त r;

        सचे (a->fd_id != 0 && b->fd_id != 0)
                पटिनिवत्त CMP(a->fd_id, b->fd_id);

        पटिनिवत्त 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_कुञ्चिका_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

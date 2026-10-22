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

        जइ (!pidref_is_set(pidref))
                प्रत्यागम -ESRCH;

        जइ (pidref_is_remote(pidref))
                प्रत्यागम -EREMOTE;

        जइ (pidref->fd < 0)
                प्रत्यागम -ENOMEDIUM;

        जइ (pidref->fd_id > 0)
                प्रत्यागम 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        जइ (r < 0) {
                जइ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                प्रत्यागम r;
        }

        प्रत्यागम 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        जइ (a == b)
                प्रत्यागम true;

        जइ (!pidref_is_set(a))
                प्रत्यागम !pidref_is_set(b);

        जइ (!pidref_is_set(b))
                प्रत्यागम false;

        जइ (a->pid != b->pid)
                प्रत्यागम false;

        जइ (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                जइ (!pidref_is_remote(b))
                        प्रत्यागम false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                जइ (a->fd_id == 0 || b->fd_id == 0)
                        प्रत्यागम true;
        } अण्णहा {
                /* If the other side is remote, then this is not the same */
                जइ (pidref_is_remote(b))
                        प्रत्यागम false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                जइ (a->pid == 1)
                        प्रत्यागम true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                जइ (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        प्रत्यागम true;
        }

        प्रत्यागम a->fd_id == b->fd_id;
}

पूर्णाङ्क pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        पूर्णाङ्क fd;

        प्रतिज्ञा(pidref);

        जइ (pid < 0)
                प्रत्यागम -ESRCH;
        जइ (pid == 0) {
                pid = getpid_cached();
                (शून्य) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        जइ (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                जइ (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        प्रत्यागम log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        प्रत्यागम 0;
}

पूर्णाङ्क pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        पूर्णाङ्क r;

        प्रतिज्ञा(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                जइ (r < 0)
                        प्रत्यागम r;

                जइ (n.fd_id != pidfd_id)
                        प्रत्यागम -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        प्रत्यागम 0;
}

पूर्णाङ्क pidref_set_pidstr_full(PidRef *pidref, ध्रुव वर्ण *pid, unsigned flags) {
        pid_t nr;
        पूर्णाङ्क r;

        प्रतिज्ञा(pidref);

        r = parse_pid(pid, &nr);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम pidref_set_pid_full(pidref, nr, flags);
}

पूर्णाङ्क pidref_set_pidfd(PidRef *pidref, पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        प्रतिज्ञा(pidref);

        जइ (fd < 0)
                प्रत्यागम -EBADF;

        पूर्णाङ्क fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        जइ (fd_copy < 0) {
                pid_t pid;

                जइ (!ERRNO_IS_RESOURCE(errno))
                        प्रत्यागम -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                जइ (r < 0)
                        प्रत्यागम r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                प्रत्यागम 0;
        }

        प्रत्यागम pidref_set_pidfd_consume(pidref, fd_copy);
}

पूर्णाङ्क pidref_set_pidfd_take(PidRef *pidref, पूर्णाङ्क fd) {
        pid_t pid;
        पूर्णाङ्क r;

        प्रतिज्ञा(pidref);

        जइ (fd < 0)
                प्रत्यागम -EBADF;

        r = pidfd_get_pid(fd, &pid);
        जइ (r < 0)
                प्रत्यागम r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        प्रत्यागम 0;
}

पूर्णाङ्क pidref_set_pidfd_consume(PidRef *pidref, पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        r = pidref_set_pidfd_take(pidref, fd);
        जइ (r < 0)
                safe_close(fd);

        प्रत्यागम r;
}

पूर्णाङ्क pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        प्रति (;;) {
                r = pidref_set_pid(&parent, ppid);
                जइ (r < 0)
                        प्रत्यागम r;

                जइ (parent.fd < 0) /* If pidfds are not available, then we are done */
                        भङ्ग;

                pid_t now_ppid = getppid();
                जइ (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        भङ्ग;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        प्रत्यागम 0;
}

शून्य pidref_done(PidRef *pidref) {
        प्रतिज्ञा(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        जइ (!pidref)
                प्रत्यागम रिक्त;

        pidref_done(pidref);
        प्रत्यागम mfree(pidref);
}

पूर्णाङ्क pidref_copy(ध्रुव PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        प्रतिज्ञा(ret);

        जइ (pidref) {
                जइ (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                अण्णहा जइ (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        जइ (copy.fd < 0) {
                                जइ (!ERRNO_IS_RESOURCE(errno))
                                        प्रत्यागम -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        प्रत्यागम 0;
}

पूर्णाङ्क pidref_dup(ध्रुव PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = रिक्त;
        पूर्णाङ्क r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        प्रतिज्ञा(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        जइ (!dup_pidref)
                प्रत्यागम -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        जइ (r < 0)
                प्रत्यागम r;

        *ret = TAKE_PTR(dup_pidref);
        प्रत्यागम 0;
}

पूर्णाङ्क pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        जइ (pid < 0)
                प्रत्यागम -ESRCH;

        n = new(PidRef, 1);
        जइ (!n)
                प्रत्यागम -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        जइ (r < 0)
                प्रत्यागम r;

        *ret = TAKE_PTR(n);
        प्रत्यागम 0;
}

पूर्णाङ्क pidref_kill(ध्रुव PidRef *pidref, पूर्णाङ्क sig) {

        जइ (!pidref)
                प्रत्यागम -ESRCH;

        जइ (pidref_is_remote(pidref))
                प्रत्यागम -EREMOTE;

        जइ (pidref->fd >= 0)
                प्रत्यागम RET_NERRNO(pidfd_send_signal(pidref->fd, sig, रिक्त, 0));

        जइ (pidref->pid > 0)
                प्रत्यागम RET_NERRNO(kill(pidref->pid, sig));

        प्रत्यागम -ESRCH;
}

पूर्णाङ्क pidref_kill_and_sigcont(ध्रुव PidRef *pidref, पूर्णाङ्क sig) {
        पूर्णाङ्क r;

        r = pidref_kill(pidref, sig);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (!IN_SET(sig, SIGCONT, SIGKILL))
                (शून्य) pidref_kill(pidref, SIGCONT);

        प्रत्यागम 0;
}

पूर्णाङ्क pidref_sigqueue(ध्रुव PidRef *pidref, पूर्णाङ्क sig, पूर्णाङ्क मानम्) {

        जइ (!pidref)
                प्रत्यागम -ESRCH;

        जइ (pidref_is_remote(pidref))
                प्रत्यागम -EREMOTE;

        जइ (pidref->fd >= 0) {
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

                प्रत्यागम RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        जइ (pidref->pid > 0)
                प्रत्यागम RET_NERRNO(sigqueue(pidref->pid, sig, (ध्रुव सङ्घ sigval) { .sival_int = मानम् }));

        प्रत्यागम -ESRCH;
}

पूर्णाङ्क pidref_verify(ध्रुव PidRef *pidref) {
        पूर्णाङ्क r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        जइ (!pidref_is_set(pidref))
                प्रत्यागम -ESRCH;

        जइ (pidref_is_remote(pidref))
                प्रत्यागम -EREMOTE;

        जइ (pidref->pid == 1)
                प्रत्यागम 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        जइ (pidref->fd < 0)
                प्रत्यागम 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        जइ (!pidref_is_set(pidref))
                प्रत्यागम false;

        जइ (pidref_is_remote(pidref))
                प्रत्यागम false;

        जइ (pidref->pid != getpid_cached())
                प्रत्यागम false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        जइ (pidref->pid == 1)
                प्रत्यागम true;

        /* Also compare pidfd ID if we can get it */
        जइ (pidref_acquire_pidfd_id(pidref) < 0)
                प्रत्यागम true;

        uint64_t self_id;
        जइ (pidfd_get_inode_id_self_cached(&self_id) < 0)
                प्रत्यागम true;

        प्रत्यागम pidref->fd_id == self_id;
}

पूर्णाङ्क pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        पूर्णाङ्क r;

        प्रतिज्ञा(timeout > 0);

        जइ (!pidref_is_set(pidref))
                प्रत्यागम -ESRCH;

        जइ (pidref_is_remote(pidref))
                प्रत्यागम -EREMOTE;

        जइ (pidref->pid == 1 || pidref_is_self(pidref))
                प्रत्यागम -ECHILD;

        जइ (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                प्रत्यागम -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        प्रति (;;) {
                जइ (poll_first) {
                        usec_t वामः;

                        जइ (ts == USEC_INFINITY)
                                वामः = USEC_INFINITY;
                        अण्णहा {
                                वामः = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                जइ (वामः == 0)
                                        प्रत्यागम -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, वामः);
                        जइ (r == 0)
                                प्रत्यागम -ETIMEDOUT;
                        जइ (r == -EINTR)
                                अनुवर्त;
                        जइ (r < 0)
                                प्रत्यागम r;
                }

                siginfo_t si = {};

                जइ (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                अण्णहा
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                जइ (r >= 0) {
                        जइ (ret_si)
                                *ret_si = si;
                        प्रत्यागम 0;
                }
                जइ (r != -EINTR)
                        प्रत्यागम r;
        }
}

bool pidref_is_automatic(ध्रुव PidRef *pidref) {
        प्रत्यागम pidref && pid_is_automatic(pidref->pid);
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
        जइ (r != 0)
                प्रत्यागम r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        जइ (r != 0)
                प्रत्यागम r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        जइ (r != 0)
                प्रत्यागम r;

        r = CMP(a->pid, b->pid);
        जइ (r != 0)
                प्रत्यागम r;

        जइ (a->fd_id != 0 && b->fd_id != 0)
                प्रत्यागम CMP(a->fd_id, b->fd_id);

        प्रत्यागम 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_कुञ्चिका_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

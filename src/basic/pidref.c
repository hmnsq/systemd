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

पूर्णांक pidref_acquire_pidfd_id(PidRef *pidref) {
        पूर्णांक r;

        प्रतिज्ञा(pidref);

        जर (!pidref_is_set(pidref))
                परत -ESRCH;

        जर (pidref_is_remote(pidref))
                परत -EREMOTE;

        जर (pidref->fd < 0)
                परत -ENOMEDIUM;

        जर (pidref->fd_id > 0)
                परत 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        जर (r < 0) {
                जर (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                परत r;
        }

        परत 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        जर (a == b)
                परत true;

        जर (!pidref_is_set(a))
                परत !pidref_is_set(b);

        जर (!pidref_is_set(b))
                परत false;

        जर (a->pid != b->pid)
                परत false;

        जर (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                जर (!pidref_is_remote(b))
                        परत false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                जर (a->fd_id == 0 || b->fd_id == 0)
                        परत true;
        } अन्यथा {
                /* If the other side is remote, then this is not the same */
                जर (pidref_is_remote(b))
                        परत false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                जर (a->pid == 1)
                        परत true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                जर (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        परत true;
        }

        परत a->fd_id == b->fd_id;
}

पूर्णांक pidref_set_pid_full(PidRef *pidref, pid_t pid, अचिन्हित flags) {
        uint64_t pidfdid = 0;
        पूर्णांक fd;

        प्रतिज्ञा(pidref);

        जर (pid < 0)
                परत -ESRCH;
        जर (pid == 0) {
                pid = getpid_cached();
                (शून्य) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        जर (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                जर (!(ERRNO_IS_RESOURCE(त्रुटि_सं) || (त्रुटि_सं == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        परत log_debug_errno(त्रुटि_सं, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        परत 0;
}

पूर्णांक pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        पूर्णांक r;

        प्रतिज्ञा(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        जर (r < 0)
                परत r;

        जर (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                जर (r < 0)
                        परत r;

                जर (n.fd_id != pidfd_id)
                        परत -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        परत 0;
}

पूर्णांक pidref_set_pidstr_full(PidRef *pidref, स्थिरांक अक्षर *pid, अचिन्हित flags) {
        pid_t nr;
        पूर्णांक r;

        प्रतिज्ञा(pidref);

        r = parse_pid(pid, &nr);
        जर (r < 0)
                परत r;

        परत pidref_set_pid_full(pidref, nr, flags);
}

पूर्णांक pidref_set_pidfd(PidRef *pidref, पूर्णांक fd) {
        पूर्णांक r;

        प्रतिज्ञा(pidref);

        जर (fd < 0)
                परत -EBADF;

        पूर्णांक fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        जर (fd_copy < 0) {
                pid_t pid;

                जर (!ERRNO_IS_RESOURCE(त्रुटि_सं))
                        परत -त्रुटि_सं;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                जर (r < 0)
                        परत r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                परत 0;
        }

        परत pidref_set_pidfd_consume(pidref, fd_copy);
}

पूर्णांक pidref_set_pidfd_take(PidRef *pidref, पूर्णांक fd) {
        pid_t pid;
        पूर्णांक r;

        प्रतिज्ञा(pidref);

        जर (fd < 0)
                परत -EBADF;

        r = pidfd_get_pid(fd, &pid);
        जर (r < 0)
                परत r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        परत 0;
}

पूर्णांक pidref_set_pidfd_consume(PidRef *pidref, पूर्णांक fd) {
        पूर्णांक r;

        r = pidref_set_pidfd_take(pidref, fd);
        जर (r < 0)
                safe_close(fd);

        परत r;
}

पूर्णांक pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        साठी (;;) {
                r = pidref_set_pid(&parent, ppid);
                जर (r < 0)
                        परत r;

                जर (parent.fd < 0) /* If pidfds are not available, then we are done */
                        थांबा;

                pid_t now_ppid = getppid();
                जर (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        थांबा;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        परत 0;
}

शून्य pidref_done(PidRef *pidref) {
        प्रतिज्ञा(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        जर (!pidref)
                परत रिक्त;

        pidref_done(pidref);
        परत mfree(pidref);
}

पूर्णांक pidref_copy(स्थिरांक PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        प्रतिज्ञा(ret);

        जर (pidref) {
                जर (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                अन्यथा जर (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        जर (copy.fd < 0) {
                                जर (!ERRNO_IS_RESOURCE(त्रुटि_सं))
                                        परत -त्रुटि_सं;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        परत 0;
}

पूर्णांक pidref_dup(स्थिरांक PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = रिक्त;
        पूर्णांक r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        प्रतिज्ञा(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        जर (!dup_pidref)
                परत -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        जर (r < 0)
                परत r;

        *ret = TAKE_PTR(dup_pidref);
        परत 0;
}

पूर्णांक pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        जर (pid < 0)
                परत -ESRCH;

        n = new(PidRef, 1);
        जर (!n)
                परत -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        जर (r < 0)
                परत r;

        *ret = TAKE_PTR(n);
        परत 0;
}

पूर्णांक pidref_kill(स्थिरांक PidRef *pidref, पूर्णांक sig) {

        जर (!pidref)
                परत -ESRCH;

        जर (pidref_is_remote(pidref))
                परत -EREMOTE;

        जर (pidref->fd >= 0)
                परत RET_NERRNO(pidfd_send_signal(pidref->fd, sig, रिक्त, 0));

        जर (pidref->pid > 0)
                परत RET_NERRNO(समाप्त(pidref->pid, sig));

        परत -ESRCH;
}

पूर्णांक pidref_kill_and_sigcont(स्थिरांक PidRef *pidref, पूर्णांक sig) {
        पूर्णांक r;

        r = pidref_kill(pidref, sig);
        जर (r < 0)
                परत r;

        जर (!IN_SET(sig, SIGCONT, SIGKILL))
                (शून्य) pidref_kill(pidref, SIGCONT);

        परत 0;
}

पूर्णांक pidref_sigqueue(स्थिरांक PidRef *pidref, पूर्णांक sig, पूर्णांक मूल्य) {

        जर (!pidref)
                परत -ESRCH;

        जर (pidref_is_remote(pidref))
                परत -EREMOTE;

        जर (pidref->fd >= 0) {
                siginfo_t si;

                /* We can't use structured initialization here, since the structure contains various unions
                 * and these fields lie in overlapping (carefully aligned) unions that LLVM is allergic to
                 * allow assignments to */
                zero(si);
                si.si_signo = sig;
                si.si_code = SI_QUEUE;
                si.si_pid = getpid_cached();
                si.si_uid = getuid();
                si.si_value.sival_int = मूल्य;

                परत RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        जर (pidref->pid > 0)
                परत RET_NERRNO(sigqueue(pidref->pid, sig, (स्थिरांक संघ sigval) { .sival_int = मूल्य }));

        परत -ESRCH;
}

पूर्णांक pidref_verify(स्थिरांक PidRef *pidref) {
        पूर्णांक r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        जर (!pidref_is_set(pidref))
                परत -ESRCH;

        जर (pidref_is_remote(pidref))
                परत -EREMOTE;

        जर (pidref->pid == 1)
                परत 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        जर (pidref->fd < 0)
                परत 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        जर (r < 0)
                परत r;

        परत 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        जर (!pidref_is_set(pidref))
                परत false;

        जर (pidref_is_remote(pidref))
                परत false;

        जर (pidref->pid != getpid_cached())
                परत false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        जर (pidref->pid == 1)
                परत true;

        /* Also compare pidfd ID if we can get it */
        जर (pidref_acquire_pidfd_id(pidref) < 0)
                परत true;

        uint64_t self_id;
        जर (pidfd_get_inode_id_self_cached(&self_id) < 0)
                परत true;

        परत pidref->fd_id == self_id;
}

पूर्णांक pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        पूर्णांक r;

        प्रतिज्ञा(timeout > 0);

        जर (!pidref_is_set(pidref))
                परत -ESRCH;

        जर (pidref_is_remote(pidref))
                परत -EREMOTE;

        जर (pidref->pid == 1 || pidref_is_self(pidref))
                परत -ECHILD;

        जर (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                परत -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        साठी (;;) {
                जर (poll_first) {
                        usec_t डावा;

                        जर (ts == USEC_INFINITY)
                                डावा = USEC_INFINITY;
                        अन्यथा {
                                डावा = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                जर (डावा == 0)
                                        परत -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, डावा);
                        जर (r == 0)
                                परत -ETIMEDOUT;
                        जर (r == -EINTR)
                                सुरूठेवा;
                        जर (r < 0)
                                परत r;
                }

                siginfo_t si = {};

                जर (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                अन्यथा
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                जर (r >= 0) {
                        जर (ret_si)
                                *ret_si = si;
                        परत 0;
                }
                जर (r != -EINTR)
                        परत r;
        }
}

bool pidref_is_automatic(स्थिरांक PidRef *pidref) {
        परत pidref && pid_is_automatic(pidref->pid);
}

शून्य pidref_hash_func(स्थिरांक PidRef *pidref, संरचना siphash *state) {
        प्रतिज्ञा(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

पूर्णांक pidref_compare_func(स्थिरांक PidRef *a, स्थिरांक PidRef *b) {
        पूर्णांक r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        जर (r != 0)
                परत r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        जर (r != 0)
                परत r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        जर (r != 0)
                परत r;

        r = CMP(a->pid, b->pid);
        जर (r != 0)
                परत r;

        जर (a->fd_id != 0 && b->fd_id != 0)
                परत CMP(a->fd_id, b->fd_id);

        परत 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_किल्ली_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

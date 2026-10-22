/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <poll.h>
#समावेश <sys/wait.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "fiber-ops.h"
#समावेश "format-util.h"
#समावेश "hash-funcs.h"
#समावेश "io-util.h"
#समावेश "log.h"
#समावेश "parse-util.h"
#समावेश "pidfd-util.h"
#समावेश "pidref.h"
#समावेश "process-util.h"
#समावेश "siphash24.h"
#समावेश "time-util.h"

पूर्णांक pidref_acquire_pidfd_id(PidRef *pidref) {
        पूर्णांक r;

        निश्चित(pidref);

        अगर (!pidref_is_set(pidref))
                वापस -ESRCH;

        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;

        अगर (pidref->fd < 0)
                वापस -ENOMEDIUM;

        अगर (pidref->fd_id > 0)
                वापस 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        अगर (r < 0) {
                अगर (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                वापस r;
        }

        वापस 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        अगर (a == b)
                वापस true;

        अगर (!pidref_is_set(a))
                वापस !pidref_is_set(b);

        अगर (!pidref_is_set(b))
                वापस false;

        अगर (a->pid != b->pid)
                वापस false;

        अगर (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                अगर (!pidref_is_remote(b))
                        वापस false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                अगर (a->fd_id == 0 || b->fd_id == 0)
                        वापस true;
        } अन्यथा {
                /* If the other side is remote, then this is not the same */
                अगर (pidref_is_remote(b))
                        वापस false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                अगर (a->pid == 1)
                        वापस true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                अगर (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        वापस true;
        }

        वापस a->fd_id == b->fd_id;
}

पूर्णांक pidref_set_pid_full(PidRef *pidref, pid_t pid, अचिन्हित flags) {
        uint64_t pidfdid = 0;
        पूर्णांक fd;

        निश्चित(pidref);

        अगर (pid < 0)
                वापस -ESRCH;
        अगर (pid == 0) {
                pid = getpid_cached();
                (व्योम) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        अगर (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                अगर (!(ERRNO_IS_RESOURCE(त्रुटि_सं) || (त्रुटि_सं == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        वापस log_debug_errno(त्रुटि_सं, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        वापस 0;
}

पूर्णांक pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        पूर्णांक r;

        निश्चित(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        अगर (r < 0)
                वापस r;

        अगर (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                अगर (r < 0)
                        वापस r;

                अगर (n.fd_id != pidfd_id)
                        वापस -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        वापस 0;
}

पूर्णांक pidref_set_pidstr_full(PidRef *pidref, स्थिर अक्षर *pid, अचिन्हित flags) {
        pid_t nr;
        पूर्णांक r;

        निश्चित(pidref);

        r = parse_pid(pid, &nr);
        अगर (r < 0)
                वापस r;

        वापस pidref_set_pid_full(pidref, nr, flags);
}

पूर्णांक pidref_set_pidfd(PidRef *pidref, पूर्णांक fd) {
        पूर्णांक r;

        निश्चित(pidref);

        अगर (fd < 0)
                वापस -EBADF;

        पूर्णांक fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        अगर (fd_copy < 0) {
                pid_t pid;

                अगर (!ERRNO_IS_RESOURCE(त्रुटि_सं))
                        वापस -त्रुटि_सं;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                अगर (r < 0)
                        वापस r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                वापस 0;
        }

        वापस pidref_set_pidfd_consume(pidref, fd_copy);
}

पूर्णांक pidref_set_pidfd_take(PidRef *pidref, पूर्णांक fd) {
        pid_t pid;
        पूर्णांक r;

        निश्चित(pidref);

        अगर (fd < 0)
                वापस -EBADF;

        r = pidfd_get_pid(fd, &pid);
        अगर (r < 0)
                वापस r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        वापस 0;
}

पूर्णांक pidref_set_pidfd_consume(PidRef *pidref, पूर्णांक fd) {
        पूर्णांक r;

        r = pidref_set_pidfd_take(pidref, fd);
        अगर (r < 0)
                safe_close(fd);

        वापस r;
}

पूर्णांक pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        पूर्णांक r;

        निश्चित(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        क्रम (;;) {
                r = pidref_set_pid(&parent, ppid);
                अगर (r < 0)
                        वापस r;

                अगर (parent.fd < 0) /* If pidfds are not available, then we are done */
                        अवरोध;

                pid_t now_ppid = getppid();
                अगर (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        अवरोध;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        वापस 0;
}

व्योम pidref_done(PidRef *pidref) {
        निश्चित(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        अगर (!pidref)
                वापस शून्य;

        pidref_done(pidref);
        वापस mfree(pidref);
}

पूर्णांक pidref_copy(स्थिर PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        निश्चित(ret);

        अगर (pidref) {
                अगर (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                अन्यथा अगर (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        अगर (copy.fd < 0) {
                                अगर (!ERRNO_IS_RESOURCE(त्रुटि_सं))
                                        वापस -त्रुटि_सं;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        वापस 0;
}

पूर्णांक pidref_dup(स्थिर PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = शून्य;
        पूर्णांक r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        निश्चित(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        अगर (!dup_pidref)
                वापस -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        अगर (r < 0)
                वापस r;

        *ret = TAKE_PTR(dup_pidref);
        वापस 0;
}

पूर्णांक pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        अगर (pid < 0)
                वापस -ESRCH;

        n = new(PidRef, 1);
        अगर (!n)
                वापस -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        अगर (r < 0)
                वापस r;

        *ret = TAKE_PTR(n);
        वापस 0;
}

पूर्णांक pidref_kill(स्थिर PidRef *pidref, पूर्णांक sig) {

        अगर (!pidref)
                वापस -ESRCH;

        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;

        अगर (pidref->fd >= 0)
                वापस RET_NERRNO(pidfd_send_signal(pidref->fd, sig, शून्य, 0));

        अगर (pidref->pid > 0)
                वापस RET_NERRNO(समाप्त(pidref->pid, sig));

        वापस -ESRCH;
}

पूर्णांक pidref_kill_and_sigcont(स्थिर PidRef *pidref, पूर्णांक sig) {
        पूर्णांक r;

        r = pidref_kill(pidref, sig);
        अगर (r < 0)
                वापस r;

        अगर (!IN_SET(sig, SIGCONT, SIGKILL))
                (व्योम) pidref_kill(pidref, SIGCONT);

        वापस 0;
}

पूर्णांक pidref_sigqueue(स्थिर PidRef *pidref, पूर्णांक sig, पूर्णांक मूल्य) {

        अगर (!pidref)
                वापस -ESRCH;

        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;

        अगर (pidref->fd >= 0) {
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

                वापस RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        अगर (pidref->pid > 0)
                वापस RET_NERRNO(sigqueue(pidref->pid, sig, (स्थिर जोड़ sigval) { .sival_int = मूल्य }));

        वापस -ESRCH;
}

पूर्णांक pidref_verify(स्थिर PidRef *pidref) {
        पूर्णांक r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        अगर (!pidref_is_set(pidref))
                वापस -ESRCH;

        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;

        अगर (pidref->pid == 1)
                वापस 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        अगर (pidref->fd < 0)
                वापस 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        अगर (r < 0)
                वापस r;

        वापस 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        अगर (!pidref_is_set(pidref))
                वापस false;

        अगर (pidref_is_remote(pidref))
                वापस false;

        अगर (pidref->pid != getpid_cached())
                वापस false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        अगर (pidref->pid == 1)
                वापस true;

        /* Also compare pidfd ID if we can get it */
        अगर (pidref_acquire_pidfd_id(pidref) < 0)
                वापस true;

        uint64_t self_id;
        अगर (pidfd_get_inode_id_self_cached(&self_id) < 0)
                वापस true;

        वापस pidref->fd_id == self_id;
}

पूर्णांक pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        पूर्णांक r;

        निश्चित(timeout > 0);

        अगर (!pidref_is_set(pidref))
                वापस -ESRCH;

        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;

        अगर (pidref->pid == 1 || pidref_is_self(pidref))
                वापस -ECHILD;

        अगर (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                वापस -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        क्रम (;;) {
                अगर (poll_first) {
                        usec_t बायाँ;

                        अगर (ts == USEC_INFINITY)
                                बायाँ = USEC_INFINITY;
                        अन्यथा {
                                बायाँ = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                अगर (बायाँ == 0)
                                        वापस -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, बायाँ);
                        अगर (r == 0)
                                वापस -ETIMEDOUT;
                        अगर (r == -EINTR)
                                जारी;
                        अगर (r < 0)
                                वापस r;
                }

                siginfo_t si = {};

                अगर (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                अन्यथा
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                अगर (r >= 0) {
                        अगर (ret_si)
                                *ret_si = si;
                        वापस 0;
                }
                अगर (r != -EINTR)
                        वापस r;
        }
}

bool pidref_is_automatic(स्थिर PidRef *pidref) {
        वापस pidref && pid_is_automatic(pidref->pid);
}

व्योम pidref_hash_func(स्थिर PidRef *pidref, काष्ठा siphash *state) {
        निश्चित(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

पूर्णांक pidref_compare_func(स्थिर PidRef *a, स्थिर PidRef *b) {
        पूर्णांक r;

        निश्चित(a);
        निश्चित(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        अगर (r != 0)
                वापस r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        अगर (r != 0)
                वापस r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        अगर (r != 0)
                वापस r;

        r = CMP(a->pid, b->pid);
        अगर (r != 0)
                वापस r;

        अगर (a->fd_id != 0 && b->fd_id != 0)
                वापस CMP(a->fd_id, b->fd_id);

        वापस 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <poll.h>
#அந்தர்பாவ <sys/wait.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fiber-ops.h"
#அந்தர்பாவ "format-util.h"
#அந்தர்பாவ "hash-funcs.h"
#அந்தர்பாவ "io-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "pidfd-util.h"
#அந்தர்பாவ "pidref.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "siphash24.h"
#அந்தர்பாவ "time-util.h"

முழு pidref_acquire_pidfd_id(PidRef *pidref) {
        முழு r;

        ப்ரதிஜ்ஞா(pidref);

        எனில் (!pidref_is_set(pidref))
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        எனில் (pidref->fd < 0)
                திரும்பு -ENOMEDIUM;

        எனில் (pidref->fd_id > 0)
                திரும்பு 0;

        r = pidfd_get_inode_id(pidref->fd, &pidref->fd_id);
        எனில் (r < 0) {
                எனில் (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        log_debug_errno(r, "Failed to get inode number of pidfd for pid " PID_FMT ": %m",
                                        pidref->pid);
                திரும்பு r;
        }

        திரும்பு 0;
}

bool pidref_equal(PidRef *a, PidRef *b) {

        /* If this is the very same structure, it definitely refers to the same process */
        எனில் (a == b)
                திரும்பு true;

        எனில் (!pidref_is_set(a))
                திரும்பு !pidref_is_set(b);

        எனில் (!pidref_is_set(b))
                திரும்பு false;

        எனில் (a->pid != b->pid)
                திரும்பு false;

        எனில் (pidref_is_remote(a)) {
                /* If one is remote and the other isn't, they are not the same */
                எனில் (!pidref_is_remote(b))
                        திரும்பு false;

                /* If both are remote, compare fd IDs if we have both, otherwise don't bother, and cut things short */
                எனில் (a->fd_id == 0 || b->fd_id == 0)
                        திரும்பு true;
        } இல்லையெனில் {
                /* If the other side is remote, then this is not the same */
                எனில் (pidref_is_remote(b))
                        திரும்பு false;

                /* PID1 cannot exit, hence it cannot change pidfs ids, hence no point in comparing them, we
                 * can shortcut things */
                எனில் (a->pid == 1)
                        திரும்பு true;

                /* Try to compare pidfds using their inode numbers. This way we can ensure that we
                 * don't spuriously consider two PidRefs equal if the pid has been reused once. Note
                 * that we ignore all errors here, not only EOPNOTSUPP, as fstat() might fail due to
                 * many reasons. */
                எனில் (pidref_acquire_pidfd_id(a) < 0 || pidref_acquire_pidfd_id(b) < 0)
                        திரும்பு true;
        }

        திரும்பு a->fd_id == b->fd_id;
}

முழு pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags) {
        uint64_t pidfdid = 0;
        முழு fd;

        ப்ரதிஜ்ஞா(pidref);

        எனில் (pid < 0)
                திரும்பு -ESRCH;
        எனில் (pid == 0) {
                pid = getpid_cached();
                (வெறுமை) pidfd_get_inode_id_self_cached(&pidfdid);
        }

        fd = pidfd_open(pid, flags);
        எனில் (fd < 0) {
                /* Graceful fallback in case the kernel is out of fds.
                 * The PIDFD_THREAD flag is supported since kernel v6.9 (64bef697d33b75fc06c5789b3f8108680271529f). */
                எனில் (!(ERRNO_IS_RESOURCE(errno) || (errno == EINVAL && FLAGS_SET(flags, PIDFD_THREAD))))
                        திரும்பு log_debug_errno(errno, "Failed to open pidfd for pid " PID_FMT ": %m", pid);

                fd = -EBADF;
        }

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
                .fd_id = pidfdid,
        };

        திரும்பு 0;
}

முழு pidref_set_pid_and_pidfd_id(
                PidRef *pidref,
                pid_t pid,
                uint64_t pidfd_id) {

        முழு r;

        ப்ரதிஜ்ஞா(pidref);

        _cleanup_(pidref_done) PidRef n = PIDREF_NULL;
        r = pidref_set_pid(&n, pid);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (pidfd_id > 0) {
                r = pidref_acquire_pidfd_id(&n);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (n.fd_id != pidfd_id)
                        திரும்பு -ESRCH;
        }

        *pidref = TAKE_PIDREF(n);
        திரும்பு 0;
}

முழு pidref_set_pidstr_full(PidRef *pidref, மாறிலி எழுத்து *pid, unsigned flags) {
        pid_t nr;
        முழு r;

        ப்ரதிஜ்ஞா(pidref);

        r = parse_pid(pid, &nr);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு pidref_set_pid_full(pidref, nr, flags);
}

முழு pidref_set_pidfd(PidRef *pidref, முழு fd) {
        முழு r;

        ப்ரதிஜ்ஞா(pidref);

        எனில் (fd < 0)
                திரும்பு -EBADF;

        முழு fd_copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        எனில் (fd_copy < 0) {
                pid_t pid;

                எனில் (!ERRNO_IS_RESOURCE(errno))
                        திரும்பு -errno;

                /* Graceful fallback if we are out of fds */
                r = pidfd_get_pid(fd, &pid);
                எனில் (r < 0)
                        திரும்பு r;

                *pidref = PIDREF_MAKE_FROM_PID(pid);
                திரும்பு 0;
        }

        திரும்பு pidref_set_pidfd_consume(pidref, fd_copy);
}

முழு pidref_set_pidfd_take(PidRef *pidref, முழு fd) {
        pid_t pid;
        முழு r;

        ப்ரதிஜ்ஞா(pidref);

        எனில் (fd < 0)
                திரும்பு -EBADF;

        r = pidfd_get_pid(fd, &pid);
        எனில் (r < 0)
                திரும்பு r;

        *pidref = (PidRef) {
                .fd = fd,
                .pid = pid,
        };

        திரும்பு 0;
}

முழு pidref_set_pidfd_consume(PidRef *pidref, முழு fd) {
        முழு r;

        r = pidref_set_pidfd_take(pidref, fd);
        எனில் (r < 0)
                safe_close(fd);

        திரும்பு r;
}

முழு pidref_set_parent(PidRef *ret) {
        _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;
        pid_t ppid;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        /* Acquires a pidref to our parent process. Deals with the fact that parent processes might exit, and
         * we get reparented to other processes, with our old parent's PID already being recycled. */

        ppid = getppid();
        க்கு (;;) {
                r = pidref_set_pid(&parent, ppid);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (parent.fd < 0) /* If pidfds are not available, then we are done */
                        நிறுத்து;

                pid_t now_ppid = getppid();
                எனில் (now_ppid == ppid) /* If our ppid is still the same, then we are done */
                        நிறுத்து;

                /* Otherwise let's try again with the new ppid */
                ppid = now_ppid;
                pidref_done(&parent);
        }

        *ret = TAKE_PIDREF(parent);
        திரும்பு 0;
}

வெறுமை pidref_done(PidRef *pidref) {
        ப்ரதிஜ்ஞா(pidref);

        *pidref = (PidRef) {
                .fd = safe_close(pidref->fd),
        };
}

PidRef* pidref_free(PidRef *pidref) {
        /* Regularly, this is an embedded structure. But sometimes we want it on the heap too */
        எனில் (!pidref)
                திரும்பு வெற்று;

        pidref_done(pidref);
        திரும்பு mfree(pidref);
}

முழு pidref_copy(மாறிலி PidRef *pidref, PidRef *ret) {
        _cleanup_(pidref_done) PidRef copy = PIDREF_NULL;

        /* If NULL is passed we'll generate a PidRef that refers to no process. This makes it easy to
         * copy pidref fields that might or might not reference a process yet. */

        ப்ரதிஜ்ஞா(ret);

        எனில் (pidref) {
                எனில் (pidref_is_remote(pidref)) /* Propagate remote flag */
                        copy.fd = -EREMOTE;
                இல்லையெனில் எனில் (pidref->fd >= 0) {
                        copy.fd = fcntl(pidref->fd, F_DUPFD_CLOEXEC, 3);
                        எனில் (copy.fd < 0) {
                                எனில் (!ERRNO_IS_RESOURCE(errno))
                                        திரும்பு -errno;

                                copy.fd = -EBADF;
                        }
                }

                copy.pid = pidref->pid;
                copy.fd_id = pidref->fd_id;
        }

        *ret = TAKE_PIDREF(copy);
        திரும்பு 0;
}

முழு pidref_dup(மாறிலி PidRef *pidref, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *dup_pidref = வெற்று;
        முழு r;

        /* Allocates a new PidRef on the heap, making it a copy of the specified pidref. This does not try to
         * acquire a pidfd if we don't have one yet! */

        ப்ரதிஜ்ஞா(ret);

        dup_pidref = newdup(PidRef, &PIDREF_NULL, 1);
        எனில் (!dup_pidref)
                திரும்பு -ENOMEM;

        r = pidref_copy(pidref, dup_pidref);
        எனில் (r < 0)
                திரும்பு r;

        *ret = TAKE_PTR(dup_pidref);
        திரும்பு 0;
}

முழு pidref_new_from_pid(pid_t pid, PidRef **ret) {
        _cleanup_(pidref_freep) PidRef *n = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        எனில் (pid < 0)
                திரும்பு -ESRCH;

        n = new(PidRef, 1);
        எனில் (!n)
                திரும்பு -ENOMEM;

        *n = PIDREF_NULL;

        r = pidref_set_pid(n, pid);
        எனில் (r < 0)
                திரும்பு r;

        *ret = TAKE_PTR(n);
        திரும்பு 0;
}

முழு pidref_kill(மாறிலி PidRef *pidref, முழு sig) {

        எனில் (!pidref)
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        எனில் (pidref->fd >= 0)
                திரும்பு RET_NERRNO(pidfd_send_signal(pidref->fd, sig, வெற்று, 0));

        எனில் (pidref->pid > 0)
                திரும்பு RET_NERRNO(kill(pidref->pid, sig));

        திரும்பு -ESRCH;
}

முழு pidref_kill_and_sigcont(மாறிலி PidRef *pidref, முழு sig) {
        முழு r;

        r = pidref_kill(pidref, sig);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (!IN_SET(sig, SIGCONT, SIGKILL))
                (வெறுமை) pidref_kill(pidref, SIGCONT);

        திரும்பு 0;
}

முழு pidref_sigqueue(மாறிலி PidRef *pidref, முழு sig, முழு மதிப்பு) {

        எனில் (!pidref)
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        எனில் (pidref->fd >= 0) {
                siginfo_t si;

                /* We can't use structured initialization here, since the structure contains various unions
                 * and these fields lie in overlapping (carefully aligned) unions that LLVM is allergic to
                 * allow assignments to */
                zero(si);
                si.si_signo = sig;
                si.si_code = SI_QUEUE;
                si.si_pid = getpid_cached();
                si.si_uid = getuid();
                si.si_value.sival_int = மதிப்பு;

                திரும்பு RET_NERRNO(pidfd_send_signal(pidref->fd, sig, &si, 0));
        }

        எனில் (pidref->pid > 0)
                திரும்பு RET_NERRNO(sigqueue(pidref->pid, sig, (மாறிலி ஒன்றிப்பு sigval) { .sival_int = மதிப்பு }));

        திரும்பு -ESRCH;
}

முழு pidref_verify(மாறிலி PidRef *pidref) {
        முழு r;

        /* This is a helper that is supposed to be called after reading information from procfs via a
         * PidRef. It ensures that the PID we track still matches the PIDFD we pin. If this value differs
         * after a procfs read, we might have read the data from a recycled PID. */

        எனில் (!pidref_is_set(pidref))
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        எனில் (pidref->pid == 1)
                திரும்பு 1; /* PID 1 can never go away, hence never be recycled to a different process → return 1 */

        எனில் (pidref->fd < 0)
                திரும்பு 0; /* If we don't have a pidfd we cannot validate it, hence we assume it's all OK → return 0 */

        r = pidfd_verify_pid(pidref->fd, pidref->pid);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு 1; /* We have a pidfd and it still points to the PID we have, hence all is *really* OK → return 1 */
}

bool pidref_is_self(PidRef *pidref) {
        எனில் (!pidref_is_set(pidref))
                திரும்பு false;

        எனில் (pidref_is_remote(pidref))
                திரும்பு false;

        எனில் (pidref->pid != getpid_cached())
                திரும்பு false;

        /* PID1 cannot exit, hence no point in comparing pidfd IDs, they can never change */
        எனில் (pidref->pid == 1)
                திரும்பு true;

        /* Also compare pidfd ID if we can get it */
        எனில் (pidref_acquire_pidfd_id(pidref) < 0)
                திரும்பு true;

        uint64_t self_id;
        எனில் (pidfd_get_inode_id_self_cached(&self_id) < 0)
                திரும்பு true;

        திரும்பு pidref->fd_id == self_id;
}

முழு pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si) {
        முழு r;

        ப்ரதிஜ்ஞா(timeout > 0);

        எனில் (!pidref_is_set(pidref))
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        எனில் (pidref->pid == 1 || pidref_is_self(pidref))
                திரும்பு -ECHILD;

        எனில் (pidref->fd < 0 && (timeout != USEC_INFINITY || fiber_ops_is_set()))
                திரும்பு -ENOMEDIUM;

        usec_t ts = timeout == USEC_INFINITY ? USEC_INFINITY : usec_add(now(CLOCK_MONOTONIC), timeout);

        /* Poll the pidfd before waitid() if either there's a finite timeout (so we can honor it) or
         * we're on a fiber (so fd_wait_for_event() can suspend us instead of blocking the event loop
         * inside waitid()). Otherwise let waitid() block directly. The precondition above guarantees
         * pidref->fd >= 0 in both cases. */
        bool poll_first = ts != USEC_INFINITY || fiber_ops_is_set();

        க்கு (;;) {
                எனில் (poll_first) {
                        usec_t இடது;

                        எனில் (ts == USEC_INFINITY)
                                இடது = USEC_INFINITY;
                        இல்லையெனில் {
                                இடது = usec_sub_unsigned(ts, now(CLOCK_MONOTONIC));
                                எனில் (இடது == 0)
                                        திரும்பு -ETIMEDOUT;
                        }

                        r = fd_wait_for_event(pidref->fd, POLLIN, இடது);
                        எனில் (r == 0)
                                திரும்பு -ETIMEDOUT;
                        எனில் (r == -EINTR)
                                தொடர்;
                        எனில் (r < 0)
                                திரும்பு r;
                }

                siginfo_t si = {};

                எனில் (pidref->fd >= 0)
                        r = RET_NERRNO(waitid(P_PIDFD, pidref->fd, &si, WEXITED));
                இல்லையெனில்
                        r = RET_NERRNO(waitid(P_PID, pidref->pid, &si, WEXITED));
                எனில் (r >= 0) {
                        எனில் (ret_si)
                                *ret_si = si;
                        திரும்பு 0;
                }
                எனில் (r != -EINTR)
                        திரும்பு r;
        }
}

bool pidref_is_automatic(மாறிலி PidRef *pidref) {
        திரும்பு pidref && pid_is_automatic(pidref->pid);
}

வெறுமை pidref_hash_func(மாறிலி PidRef *pidref, கட்டமைப்பு siphash *state) {
        ப்ரதிஜ்ஞா(pidref);

        siphash24_compress_typesafe(pidref->pid, state);
}

முழு pidref_compare_func(மாறிலி PidRef *a, மாறிலி PidRef *b) {
        முழு r;

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        r = CMP(pidref_is_set(a), pidref_is_set(b));
        எனில் (r != 0)
                திரும்பு r;

        r = CMP(pidref_is_automatic(a), pidref_is_automatic(b));
        எனில் (r != 0)
                திரும்பு r;

        r = CMP(pidref_is_remote(a), pidref_is_remote(b));
        எனில் (r != 0)
                திரும்பு r;

        r = CMP(a->pid, b->pid);
        எனில் (r != 0)
                திரும்பு r;

        எனில் (a->fd_id != 0 && b->fd_id != 0)
                திரும்பு CMP(a->fd_id, b->fd_id);

        திரும்பு 0;
}

DEFINE_HASH_OPS(pidref_hash_ops, PidRef, pidref_hash_func, pidref_compare_func);

DEFINE_HASH_OPS_WITH_சாவி_DESTRUCTOR(pidref_hash_ops_free,
                                    PidRef, pidref_hash_func, pidref_compare_func,
                                    pidref_free);

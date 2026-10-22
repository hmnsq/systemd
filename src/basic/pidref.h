/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* An embeddable structure carrying a reference to a process. Supposed to be used when tracking processes
 * continuously. This combines a PID, a modern Linux pidfd and the 64bit inode number of the pidfd into one
 * structure. Note that depending on kernel support the pidfd might not be initialized, and if it is
 * initialized then fd_id might still not be initialized (because the concept was added to the kernel much
 * later than pidfds themselves).
 *
 * There are three special states a PidRef can be in:
 *
 * 1. It can be *unset*. Use pidref_is_set() to detect this case. Most operations attempted on such a PidRef
 *    will fail with -ESRCH. Use PIDREF_NULL for initializing a PidRef in this state.
 *
 * 2. It can be marked as *automatic*. This is a special state indicating that a process reference is
 *    supposed to be derived automatically from the current context. This is used by the Varlink/JSON
 *    dispatcher as indication that a PidRef shall be derived from the connection peer, but might be
 *    otherwise used too. When marked *automatic* the PidRef will also be considered *unset*, hence most
 *    operations will fail with -ESRCH, as above.
 *
 * 3. It can be marked as *remote*. This is useful when deserializing a PidRef structure from an IPC message
 *    or similar, and it has been determined that the given PID definitely doesn't refer to a local
 *    process. In this case the PidRef logic will refrain from trying to acquire a pidfd for the
 *    process. Moreover, most operations will fail with -EREMOTE. Only PidRef structures that are not marked
 *    *unset* can be marked *remote*.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ PidRef {
        pid_t pid;      /* > 0 if the PidRef is set, otherwise set to INT_MIN (PID_AUTOMATIC) if automatic
                         * mode is desired, or 0 otherwise. */
        ਪੂਰਨ fd;         /* only valid if pidfd are available in the kernel, and we manage to get an fd. If we
                         * know that the PID is not from the local machine we set this to -EREMOTE, otherwise
                         * we use -EBADF as indicator the fd is invalid. */
        uint64_t fd_id; /* the inode number of pidfd. only useful in kernel 6.9+ where pidfds live in
                           their own pidfs and each process comes with a unique inode number */
} PidRef;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PIDREF_NULL (PidRef) { .fd = -EBADF }

/* A special pidref value that we are using when a PID shall be automatically acquired from some surrounding
 * context, for example connection peer. Much like PIDREF_NULL it will be considered unset by
 * pidref_is_set(). */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PIDREF_AUTOMATIC (ਅਚਲ PidRef) { .pid = (pid_t) INT_MIN, .fd = -EBADF }

/* Turns a pid_t into a PidRef structure on-the-fly *without* acquiring a pidfd for it. (As opposed to
 * pidref_set_pid() which does so *with* acquiring one, see below) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PIDREF_MAKE_FROM_PID(x) (PidRef) { .pid = (x), .fd = -EBADF }

ਸਥਿਰ ਸਤਰਅੰਦਰ bool pidref_is_ਸੈੱਟ(ਅਚਲ PidRef *pidref) {
        ਵਾਪਸ pidref && pidref->pid > 0;
}

bool pidref_is_automatic(ਅਚਲ PidRef *pidref);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool pidref_is_ਸੈੱਟ_or_automatic(ਅਚਲ PidRef *pidref) {
        ਵਾਪਸ pidref_is_ਸੈੱਟ(pidref) || pidref_is_automatic(pidref);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool pidref_is_remote(ਅਚਲ PidRef *pidref) {
        /* If the fd is set to -EREMOTE we assume PidRef does not refer to a local PID, but on another
         * machine (and we just got the PidRef initialized due to deserialization of some RPC message) */
        ਵਾਪਸ pidref_is_ਸੈੱਟ(pidref) && pidref->fd == -EREMOTE;
}

ਪੂਰਨ pidref_acquire_pidfd_id(PidRef *pidref);
bool pidref_equal(PidRef *a, PidRef *b);

/* This turns a pid_t into a PidRef structure, and acquires a pidfd for it, if possible. (As opposed to
 * PIDREF_MAKE_FROM_PID() above, which does not acquire a pidfd.) */
ਪੂਰਨ pidref_ਸੈੱਟ_pid_ਭਰਿਆ(PidRef *pidref, pid_t pid, ਬਿਨਾਨਿਸ਼ਾਨੀ flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ pidref_ਸੈੱਟ_pid(PidRef *pidref, pid_t pid) {
        ਵਾਪਸ pidref_ਸੈੱਟ_pid_ਭਰਿਆ(pidref, pid, /* flags= */ 0);
}
ਪੂਰਨ pidref_ਸੈੱਟ_pidstr_ਭਰਿਆ(PidRef *pidref, ਅਚਲ ਅੱਖਰ *pid, ਬਿਨਾਨਿਸ਼ਾਨੀ flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ pidref_ਸੈੱਟ_pidstr(PidRef *pidref, ਅਚਲ ਅੱਖਰ *pid) {
        ਵਾਪਸ pidref_ਸੈੱਟ_pidstr_ਭਰਿਆ(pidref, pid, /* flags= */ 0);
}
ਪੂਰਨ pidref_ਸੈੱਟ_pid_and_pidfd_id(PidRef *pidref, pid_t pid, uint64_t pidfd_id);
ਪੂਰਨ pidref_ਸੈੱਟ_pidfd(PidRef *pidref, ਪੂਰਨ fd);
ਪੂਰਨ pidref_ਸੈੱਟ_pidfd_take(PidRef *pidref, ਪੂਰਨ fd); /* takes ownership of the passed pidfd on success */
ਪੂਰਨ pidref_ਸੈੱਟ_pidfd_consume(PidRef *pidref, ਪੂਰਨ fd); /* takes ownership of the passed pidfd in both success and failure */
ਪੂਰਨ pidref_ਸੈੱਟ_ਮਾਪਾ(PidRef *ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ pidref_ਸੈੱਟ_self(PidRef *pidref) {
        ਵਾਪਸ pidref_ਸੈੱਟ_pid(pidref, 0);
}

bool pidref_is_self(PidRef *pidref);

ਖਾਲੀ pidref_ਪੂਰਾ(PidRef *pidref);
PidRef* pidref_free(PidRef *pidref);
DEFINE_TRIVIAL_CLEANUP_FUNC(PidRef*, pidref_free);

ਪੂਰਨ pidref_copy(ਅਚਲ PidRef *pidref, PidRef *ret);
ਪੂਰਨ pidref_dup(ਅਚਲ PidRef *pidref, PidRef **ret);

ਪੂਰਨ pidref_new_from_pid(pid_t pid, PidRef **ret);

ਪੂਰਨ pidref_kill(ਅਚਲ PidRef *pidref, ਪੂਰਨ sig);
ਪੂਰਨ pidref_kill_and_sigcont(ਅਚਲ PidRef *pidref, ਪੂਰਨ sig);
ਪੂਰਨ pidref_sigqueue(ਅਚਲ PidRef *pidref, ਪੂਰਨ sig, ਪੂਰਨ ਮੁੱਲ);

ਪੂਰਨ pidref_wait_for_terminate_ਭਰਿਆ(PidRef *pidref, usec_t timeout, siginfo_t *ret_si);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ pidref_wait_for_terminate(PidRef *pidref, siginfo_t *ret_si) {
        ਵਾਪਸ pidref_wait_for_terminate_ਭਰਿਆ(pidref, USEC_INFINITY, ret_si);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ pidref_ਪੂਰਾ_sigterm_wait(PidRef *pidref) {
        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ;

        (ਖਾਲੀ) pidref_kill(pidref, SIGTERM);
        (ਖਾਲੀ) pidref_wait_for_terminate(pidref, ਨਿਰਾ);
        pidref_ਪੂਰਾ(pidref);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ pidref_ਪੂਰਾ_sigkill_wait(PidRef *pidref) {
        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ;

        (ਖਾਲੀ) pidref_kill(pidref, SIGKILL);
        (ਖਾਲੀ) pidref_wait_for_terminate(pidref, ਨਿਰਾ);
        pidref_ਪੂਰਾ(pidref);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ pidref_ਪੂਰਾ_sigkill_nowait(PidRef *pidref) {
        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ;

        (ਖਾਲੀ) pidref_kill(pidref, SIGKILL);
        pidref_ਪੂਰਾ(pidref);
}

ਪੂਰਨ pidref_verify(ਅਚਲ PidRef *pidref);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TAKE_PIDREF(p) TAKE_GENERIC((p), PidRef, PIDREF_NULL)

ਬਣਤਰ siphash;
ਖਾਲੀ pidref_hash_func(ਅਚਲ PidRef *pidref, ਬਣਤਰ siphash *state);
ਪੂਰਨ pidref_compare_func(ਅਚਲ PidRef *a, ਅਚਲ PidRef *b);

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops pidref_hash_ops;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops pidref_hash_ops_free; /* Has destructor call for pidref_free(), i.e. expects heap allocated PidRef as keys */

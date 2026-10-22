/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <signal.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ PidRef {
        pid_t pid;      /* > 0 if the PidRef is set, otherwise set to INT_MIN (PID_AUTOMATIC) if automatic
                         * mode is desired, or 0 otherwise. */
        ಪೂರ್ಣಾಂಕ fd;         /* only valid if pidfd are available in the kernel, and we manage to get an fd. If we
                         * know that the PID is not from the local machine we set this to -EREMOTE, otherwise
                         * we use -EBADF as indicator the fd is invalid. */
        uint64_t fd_id; /* the inode number of pidfd. only useful in kernel 6.9+ where pidfds live in
                           their own pidfs and each process comes with a unique inode number */
} PidRef;

#ವ್ಯಾಖ್ಯಾನಿಸು PIDREF_NULL (PidRef) { .fd = -EBADF }

/* A special pidref value that we are using when a PID shall be automatically acquired from some surrounding
 * context, for example connection peer. Much like PIDREF_NULL it will be considered unset by
 * pidref_is_set(). */
#ವ್ಯಾಖ್ಯಾನಿಸು PIDREF_AUTOMATIC (ಸ್ಥಿರಾಂಕ PidRef) { .pid = (pid_t) INT_MIN, .fd = -EBADF }

/* Turns a pid_t into a PidRef structure on-the-fly *without* acquiring a pidfd for it. (As opposed to
 * pidref_set_pid() which does so *with* acquiring one, see below) */
#ವ್ಯಾಖ್ಯಾನಿಸು PIDREF_MAKE_FROM_PID(x) (PidRef) { .pid = (x), .fd = -EBADF }

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool pidref_is_ಗಣ(ಸ್ಥಿರಾಂಕ PidRef *pidref) {
        ಹಿಂತಿರುಗಿ pidref && pidref->pid > 0;
}

bool pidref_is_automatic(ಸ್ಥಿರಾಂಕ PidRef *pidref);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool pidref_is_ಗಣ_or_automatic(ಸ್ಥಿರಾಂಕ PidRef *pidref) {
        ಹಿಂತಿರುಗಿ pidref_is_ಗಣ(pidref) || pidref_is_automatic(pidref);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool pidref_is_remote(ಸ್ಥಿರಾಂಕ PidRef *pidref) {
        /* If the fd is set to -EREMOTE we assume PidRef does not refer to a local PID, but on another
         * machine (and we just got the PidRef initialized due to deserialization of some RPC message) */
        ಹಿಂತಿರುಗಿ pidref_is_ಗಣ(pidref) && pidref->fd == -EREMOTE;
}

ಪೂರ್ಣಾಂಕ pidref_acquire_pidfd_id(PidRef *pidref);
bool pidref_equal(PidRef *a, PidRef *b);

/* This turns a pid_t into a PidRef structure, and acquires a pidfd for it, if possible. (As opposed to
 * PIDREF_MAKE_FROM_PID() above, which does not acquire a pidfd.) */
ಪೂರ್ಣಾಂಕ pidref_ಗಣ_pid_ತುಂಬಿದ(PidRef *pidref, pid_t pid, ಚಿಹ್ನೆರಹಿತ flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ pidref_ಗಣ_pid(PidRef *pidref, pid_t pid) {
        ಹಿಂತಿರುಗಿ pidref_ಗಣ_pid_ತುಂಬಿದ(pidref, pid, /* flags= */ 0);
}
ಪೂರ್ಣಾಂಕ pidref_ಗಣ_pidstr_ತುಂಬಿದ(PidRef *pidref, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pid, ಚಿಹ್ನೆರಹಿತ flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ pidref_ಗಣ_pidstr(PidRef *pidref, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pid) {
        ಹಿಂತಿರುಗಿ pidref_ಗಣ_pidstr_ತುಂಬಿದ(pidref, pid, /* flags= */ 0);
}
ಪೂರ್ಣಾಂಕ pidref_ಗಣ_pid_and_pidfd_id(PidRef *pidref, pid_t pid, uint64_t pidfd_id);
ಪೂರ್ಣಾಂಕ pidref_ಗಣ_pidfd(PidRef *pidref, ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ pidref_ಗಣ_pidfd_take(PidRef *pidref, ಪೂರ್ಣಾಂಕ fd); /* takes ownership of the passed pidfd on success */
ಪೂರ್ಣಾಂಕ pidref_ಗಣ_pidfd_consume(PidRef *pidref, ಪೂರ್ಣಾಂಕ fd); /* takes ownership of the passed pidfd in both success and failure */
ಪೂರ್ಣಾಂಕ pidref_ಗಣ_ಪೋಷಕ(PidRef *ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ pidref_ಗಣ_self(PidRef *pidref) {
        ಹಿಂತಿರುಗಿ pidref_ಗಣ_pid(pidref, 0);
}

bool pidref_is_self(PidRef *pidref);

ಶೂನ್ಯ pidref_ಮುಗಿದ(PidRef *pidref);
PidRef* pidref_free(PidRef *pidref);
DEFINE_TRIVIAL_CLEANUP_FUNC(PidRef*, pidref_free);

ಪೂರ್ಣಾಂಕ pidref_copy(ಸ್ಥಿರಾಂಕ PidRef *pidref, PidRef *ret);
ಪೂರ್ಣಾಂಕ pidref_dup(ಸ್ಥಿರಾಂಕ PidRef *pidref, PidRef **ret);

ಪೂರ್ಣಾಂಕ pidref_new_from_pid(pid_t pid, PidRef **ret);

ಪೂರ್ಣಾಂಕ pidref_kill(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಪೂರ್ಣಾಂಕ sig);
ಪೂರ್ಣಾಂಕ pidref_kill_and_sigcont(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಪೂರ್ಣಾಂಕ sig);
ಪೂರ್ಣಾಂಕ pidref_sigqueue(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಪೂರ್ಣಾಂಕ sig, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ);

ಪೂರ್ಣಾಂಕ pidref_wait_for_terminate_ತುಂಬಿದ(PidRef *pidref, usec_t timeout, siginfo_t *ret_si);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ pidref_wait_for_terminate(PidRef *pidref, siginfo_t *ret_si) {
        ಹಿಂತಿರುಗಿ pidref_wait_for_terminate_ತುಂಬಿದ(pidref, USEC_INFINITY, ret_si);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ pidref_ಮುಗಿದ_sigterm_wait(PidRef *pidref) {
        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ;

        (ಶೂನ್ಯ) pidref_kill(pidref, SIGTERM);
        (ಶೂನ್ಯ) pidref_wait_for_terminate(pidref, ಖಾಲಿ);
        pidref_ಮುಗಿದ(pidref);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ pidref_ಮುಗಿದ_sigkill_wait(PidRef *pidref) {
        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ;

        (ಶೂನ್ಯ) pidref_kill(pidref, SIGKILL);
        (ಶೂನ್ಯ) pidref_wait_for_terminate(pidref, ಖಾಲಿ);
        pidref_ಮುಗಿದ(pidref);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ pidref_ಮುಗಿದ_sigkill_nowait(PidRef *pidref) {
        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ;

        (ಶೂನ್ಯ) pidref_kill(pidref, SIGKILL);
        pidref_ಮುಗಿದ(pidref);
}

ಪೂರ್ಣಾಂಕ pidref_verify(ಸ್ಥಿರಾಂಕ PidRef *pidref);

#ವ್ಯಾಖ್ಯಾನಿಸು TAKE_PIDREF(p) TAKE_GENERIC((p), PidRef, PIDREF_NULL)

ರಚನೆ siphash;
ಶೂನ್ಯ pidref_hash_func(ಸ್ಥಿರಾಂಕ PidRef *pidref, ರಚನೆ siphash *state);
ಪೂರ್ಣಾಂಕ pidref_compare_func(ಸ್ಥಿರಾಂಕ PidRef *a, ಸ್ಥಿರಾಂಕ PidRef *b);

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops pidref_hash_ops;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops pidref_hash_ops_free; /* Has destructor call for pidref_free(), i.e. expects heap allocated PidRef as keys */

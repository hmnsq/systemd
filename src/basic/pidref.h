/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <signal.h>

#அந்தர்பாவ "forward.h"

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
வகைவரை கட்டமைப்பு PidRef {
        pid_t pid;      /* > 0 if the PidRef is set, otherwise set to INT_MIN (PID_AUTOMATIC) if automatic
                         * mode is desired, or 0 otherwise. */
        முழு fd;         /* only valid if pidfd are available in the kernel, and we manage to get an fd. If we
                         * know that the PID is not from the local machine we set this to -EREMOTE, otherwise
                         * we use -EBADF as indicator the fd is invalid. */
        uint64_t fd_id; /* the inode number of pidfd. only useful in kernel 6.9+ where pidfds live in
                           their own pidfs and each process comes with a unique inode number */
} PidRef;

#define PIDREF_NULL (PidRef) { .fd = -EBADF }

/* A special pidref value that we are using when a PID shall be automatically acquired from some surrounding
 * context, for example connection peer. Much like PIDREF_NULL it will be considered unset by
 * pidref_is_set(). */
#define PIDREF_AUTOMATIC (மாறிலி PidRef) { .pid = (pid_t) INT_MIN, .fd = -EBADF }

/* Turns a pid_t into a PidRef structure on-the-fly *without* acquiring a pidfd for it. (As opposed to
 * pidref_set_pid() which does so *with* acquiring one, see below) */
#define PIDREF_MAKE_FROM_PID(x) (PidRef) { .pid = (x), .fd = -EBADF }

நிலையான உள்வரி bool pidref_is_set(மாறிலி PidRef *pidref) {
        திரும்பு pidref && pidref->pid > 0;
}

bool pidref_is_automatic(மாறிலி PidRef *pidref);

நிலையான உள்வரி bool pidref_is_set_or_automatic(மாறிலி PidRef *pidref) {
        திரும்பு pidref_is_set(pidref) || pidref_is_automatic(pidref);
}

நிலையான உள்வரி bool pidref_is_remote(மாறிலி PidRef *pidref) {
        /* If the fd is set to -EREMOTE we assume PidRef does not refer to a local PID, but on another
         * machine (and we just got the PidRef initialized due to deserialization of some RPC message) */
        திரும்பு pidref_is_set(pidref) && pidref->fd == -EREMOTE;
}

முழு pidref_acquire_pidfd_id(PidRef *pidref);
bool pidref_equal(PidRef *a, PidRef *b);

/* This turns a pid_t into a PidRef structure, and acquires a pidfd for it, if possible. (As opposed to
 * PIDREF_MAKE_FROM_PID() above, which does not acquire a pidfd.) */
முழு pidref_set_pid_full(PidRef *pidref, pid_t pid, unsigned flags);
நிலையான உள்வரி முழு pidref_set_pid(PidRef *pidref, pid_t pid) {
        திரும்பு pidref_set_pid_full(pidref, pid, /* flags= */ 0);
}
முழு pidref_set_pidstr_full(PidRef *pidref, மாறிலி எழுத்து *pid, unsigned flags);
நிலையான உள்வரி முழு pidref_set_pidstr(PidRef *pidref, மாறிலி எழுத்து *pid) {
        திரும்பு pidref_set_pidstr_full(pidref, pid, /* flags= */ 0);
}
முழு pidref_set_pid_and_pidfd_id(PidRef *pidref, pid_t pid, uint64_t pidfd_id);
முழு pidref_set_pidfd(PidRef *pidref, முழு fd);
முழு pidref_set_pidfd_take(PidRef *pidref, முழு fd); /* takes ownership of the passed pidfd on success */
முழு pidref_set_pidfd_consume(PidRef *pidref, முழு fd); /* takes ownership of the passed pidfd in both success and failure */
முழு pidref_set_parent(PidRef *ret);
நிலையான உள்வரி முழு pidref_set_self(PidRef *pidref) {
        திரும்பு pidref_set_pid(pidref, 0);
}

bool pidref_is_self(PidRef *pidref);

வெறுமை pidref_done(PidRef *pidref);
PidRef* pidref_free(PidRef *pidref);
DEFINE_TRIVIAL_CLEANUP_FUNC(PidRef*, pidref_free);

முழு pidref_copy(மாறிலி PidRef *pidref, PidRef *ret);
முழு pidref_dup(மாறிலி PidRef *pidref, PidRef **ret);

முழு pidref_new_from_pid(pid_t pid, PidRef **ret);

முழு pidref_kill(மாறிலி PidRef *pidref, முழு sig);
முழு pidref_kill_and_sigcont(மாறிலி PidRef *pidref, முழு sig);
முழு pidref_sigqueue(மாறிலி PidRef *pidref, முழு sig, முழு மதிப்பு);

முழு pidref_wait_for_terminate_full(PidRef *pidref, usec_t timeout, siginfo_t *ret_si);
நிலையான உள்வரி முழு pidref_wait_for_terminate(PidRef *pidref, siginfo_t *ret_si) {
        திரும்பு pidref_wait_for_terminate_full(pidref, USEC_INFINITY, ret_si);
}

நிலையான உள்வரி வெறுமை pidref_done_sigterm_wait(PidRef *pidref) {
        எனில் (!pidref_is_set(pidref))
                திரும்பு;

        (வெறுமை) pidref_kill(pidref, SIGTERM);
        (வெறுமை) pidref_wait_for_terminate(pidref, வெற்று);
        pidref_done(pidref);
}

நிலையான உள்வரி வெறுமை pidref_done_sigkill_wait(PidRef *pidref) {
        எனில் (!pidref_is_set(pidref))
                திரும்பு;

        (வெறுமை) pidref_kill(pidref, SIGKILL);
        (வெறுமை) pidref_wait_for_terminate(pidref, வெற்று);
        pidref_done(pidref);
}

நிலையான உள்வரி வெறுமை pidref_done_sigkill_nowait(PidRef *pidref) {
        எனில் (!pidref_is_set(pidref))
                திரும்பு;

        (வெறுமை) pidref_kill(pidref, SIGKILL);
        pidref_done(pidref);
}

முழு pidref_verify(மாறிலி PidRef *pidref);

#define TAKE_PIDREF(p) TAKE_GENERIC((p), PidRef, PIDREF_NULL)

கட்டமைப்பு siphash;
வெறுமை pidref_hash_func(மாறிலி PidRef *pidref, கட்டமைப்பு siphash *state);
முழு pidref_compare_func(மாறிலி PidRef *a, மாறிலி PidRef *b);

வெளி மாறிலி கட்டமைப்பு hash_ops pidref_hash_ops;
வெளி மாறிலி கட்டமைப்பு hash_ops pidref_hash_ops_free; /* Has destructor call for pidref_free(), i.e. expects heap allocated PidRef as keys */

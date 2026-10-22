/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <signal.h>

#अन्तर्भाव "forward.h"

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
प्रकारनिर्देश संरचना PidRef {
        pid_t pid;      /* > 0 if the PidRef is set, otherwise set to INT_MIN (PID_AUTOMATIC) if automatic
                         * mode is desired, or 0 otherwise. */
        पूर्णाङ्क fd;         /* only valid if pidfd are available in the kernel, and we manage to get an fd. If we
                         * know that the PID is not from the local machine we set this to -EREMOTE, otherwise
                         * we use -EBADF as indicator the fd is invalid. */
        uint64_t fd_id; /* the inode number of pidfd. only useful in kernel 6.9+ where pidfds live in
                           their own pidfs and each process comes with a unique inode number */
} PidRef;

#परिभाषय PIDREF_NULL (PidRef) { .fd = -EBADF }

/* A special pidref value that we are using when a PID shall be automatically acquired from some surrounding
 * context, for example connection peer. Much like PIDREF_NULL it will be considered unset by
 * pidref_is_set(). */
#परिभाषय PIDREF_AUTOMATIC (ध्रुव PidRef) { .pid = (pid_t) INT_MIN, .fd = -EBADF }

/* Turns a pid_t into a PidRef structure on-the-fly *without* acquiring a pidfd for it. (As opposed to
 * pidref_set_pid() which does so *with* acquiring one, see below) */
#परिभाषय PIDREF_MAKE_FROM_PID(x) (PidRef) { .pid = (x), .fd = -EBADF }

स्थिर अन्तःस्थ bool pidref_is_समुच्चयः(ध्रुव PidRef *pidref) {
        प्रत्यागम pidref && pidref->pid > 0;
}

bool pidref_is_automatic(ध्रुव PidRef *pidref);

स्थिर अन्तःस्थ bool pidref_is_समुच्चयः_or_automatic(ध्रुव PidRef *pidref) {
        प्रत्यागम pidref_is_समुच्चयः(pidref) || pidref_is_automatic(pidref);
}

स्थिर अन्तःस्थ bool pidref_is_remote(ध्रुव PidRef *pidref) {
        /* If the fd is set to -EREMOTE we assume PidRef does not refer to a local PID, but on another
         * machine (and we just got the PidRef initialized due to deserialization of some RPC message) */
        प्रत्यागम pidref_is_समुच्चयः(pidref) && pidref->fd == -EREMOTE;
}

पूर्णाङ्क pidref_acquire_pidfd_id(PidRef *pidref);
bool pidref_equal(PidRef *a, PidRef *b);

/* This turns a pid_t into a PidRef structure, and acquires a pidfd for it, if possible. (As opposed to
 * PIDREF_MAKE_FROM_PID() above, which does not acquire a pidfd.) */
पूर्णाङ्क pidref_समुच्चयः_pid_पूर्णम्(PidRef *pidref, pid_t pid, अचिह्नित flags);
स्थिर अन्तःस्थ पूर्णाङ्क pidref_समुच्चयः_pid(PidRef *pidref, pid_t pid) {
        प्रत्यागम pidref_समुच्चयः_pid_पूर्णम्(pidref, pid, /* flags= */ 0);
}
पूर्णाङ्क pidref_समुच्चयः_pidstr_पूर्णम्(PidRef *pidref, ध्रुव वर्ण *pid, अचिह्नित flags);
स्थिर अन्तःस्थ पूर्णाङ्क pidref_समुच्चयः_pidstr(PidRef *pidref, ध्रुव वर्ण *pid) {
        प्रत्यागम pidref_समुच्चयः_pidstr_पूर्णम्(pidref, pid, /* flags= */ 0);
}
पूर्णाङ्क pidref_समुच्चयः_pid_and_pidfd_id(PidRef *pidref, pid_t pid, uint64_t pidfd_id);
पूर्णाङ्क pidref_समुच्चयः_pidfd(PidRef *pidref, पूर्णाङ्क fd);
पूर्णाङ्क pidref_समुच्चयः_pidfd_take(PidRef *pidref, पूर्णाङ्क fd); /* takes ownership of the passed pidfd on success */
पूर्णाङ्क pidref_समुच्चयः_pidfd_consume(PidRef *pidref, पूर्णाङ्क fd); /* takes ownership of the passed pidfd in both success and failure */
पूर्णाङ्क pidref_समुच्चयः_parent(PidRef *ret);
स्थिर अन्तःस्थ पूर्णाङ्क pidref_समुच्चयः_self(PidRef *pidref) {
        प्रत्यागम pidref_समुच्चयः_pid(pidref, 0);
}

bool pidref_is_self(PidRef *pidref);

शून्य pidref_समाप्तम्(PidRef *pidref);
PidRef* pidref_free(PidRef *pidref);
DEFINE_TRIVIAL_CLEANUP_FUNC(PidRef*, pidref_free);

पूर्णाङ्क pidref_copy(ध्रुव PidRef *pidref, PidRef *ret);
पूर्णाङ्क pidref_dup(ध्रुव PidRef *pidref, PidRef **ret);

पूर्णाङ्क pidref_new_from_pid(pid_t pid, PidRef **ret);

पूर्णाङ्क pidref_kill(ध्रुव PidRef *pidref, पूर्णाङ्क sig);
पूर्णाङ्क pidref_kill_and_sigcont(ध्रुव PidRef *pidref, पूर्णाङ्क sig);
पूर्णाङ्क pidref_sigqueue(ध्रुव PidRef *pidref, पूर्णाङ्क sig, पूर्णाङ्क मानम्);

पूर्णाङ्क pidref_wait_for_terminate_पूर्णम्(PidRef *pidref, usec_t timeout, siginfo_t *ret_si);
स्थिर अन्तःस्थ पूर्णाङ्क pidref_wait_for_terminate(PidRef *pidref, siginfo_t *ret_si) {
        प्रत्यागम pidref_wait_for_terminate_पूर्णम्(pidref, USEC_INFINITY, ret_si);
}

स्थिर अन्तःस्थ शून्य pidref_समाप्तम्_sigterm_wait(PidRef *pidref) {
        यदि (!pidref_is_समुच्चयः(pidref))
                प्रत्यागम;

        (शून्य) pidref_kill(pidref, SIGTERM);
        (शून्य) pidref_wait_for_terminate(pidref, रिक्त);
        pidref_समाप्तम्(pidref);
}

स्थिर अन्तःस्थ शून्य pidref_समाप्तम्_sigkill_wait(PidRef *pidref) {
        यदि (!pidref_is_समुच्चयः(pidref))
                प्रत्यागम;

        (शून्य) pidref_kill(pidref, SIGKILL);
        (शून्य) pidref_wait_for_terminate(pidref, रिक्त);
        pidref_समाप्तम्(pidref);
}

स्थिर अन्तःस्थ शून्य pidref_समाप्तम्_sigkill_nowait(PidRef *pidref) {
        यदि (!pidref_is_समुच्चयः(pidref))
                प्रत्यागम;

        (शून्य) pidref_kill(pidref, SIGKILL);
        pidref_समाप्तम्(pidref);
}

पूर्णाङ्क pidref_verify(ध्रुव PidRef *pidref);

#परिभाषय TAKE_PIDREF(p) TAKE_GENERIC((p), PidRef, PIDREF_NULL)

संरचना siphash;
शून्य pidref_hash_func(ध्रुव PidRef *pidref, संरचना siphash *state);
पूर्णाङ्क pidref_compare_func(ध्रुव PidRef *a, ध्रुव PidRef *b);

बाह्य ध्रुव संरचना hash_ops pidref_hash_ops;
बाह्य ध्रुव संरचना hash_ops pidref_hash_ops_free; /* Has destructor call for pidref_free(), i.e. expects heap allocated PidRef as keys */

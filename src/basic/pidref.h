/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <signal.h>

#అన్తర్భావ "forward.h"

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
రకనిర్వచనం నిర్మాణం PidRef {
        pid_t pid;      /* > 0 if the PidRef is set, otherwise set to INT_MIN (PID_AUTOMATIC) if automatic
                         * mode is desired, or 0 otherwise. */
        పూర్ణాంకం fd;         /* only valid if pidfd are available in the kernel, and we manage to get an fd. If we
                         * know that the PID is not from the local machine we set this to -EREMOTE, otherwise
                         * we use -EBADF as indicator the fd is invalid. */
        uint64_t fd_id; /* the inode number of pidfd. only useful in kernel 6.9+ where pidfds live in
                           their own pidfs and each process comes with a unique inode number */
} PidRef;

#నిర్వచించు PIDREF_NULL (PidRef) { .fd = -EBADF }

/* A special pidref value that we are using when a PID shall be automatically acquired from some surrounding
 * context, for example connection peer. Much like PIDREF_NULL it will be considered unset by
 * pidref_is_set(). */
#నిర్వచించు PIDREF_AUTOMATIC (స్థిరరాశి PidRef) { .pid = (pid_t) INT_MIN, .fd = -EBADF }

/* Turns a pid_t into a PidRef structure on-the-fly *without* acquiring a pidfd for it. (As opposed to
 * pidref_set_pid() which does so *with* acquiring one, see below) */
#నిర్వచించు PIDREF_MAKE_FROM_PID(x) (PidRef) { .pid = (x), .fd = -EBADF }

స్థిర అంతఃవరుస bool pidref_is_సమితి(స్థిరరాశి PidRef *pidref) {
        తిరిగి pidref && pidref->pid > 0;
}

bool pidref_is_automatic(స్థిరరాశి PidRef *pidref);

స్థిర అంతఃవరుస bool pidref_is_సమితి_or_automatic(స్థిరరాశి PidRef *pidref) {
        తిరిగి pidref_is_సమితి(pidref) || pidref_is_automatic(pidref);
}

స్థిర అంతఃవరుస bool pidref_is_remote(స్థిరరాశి PidRef *pidref) {
        /* If the fd is set to -EREMOTE we assume PidRef does not refer to a local PID, but on another
         * machine (and we just got the PidRef initialized due to deserialization of some RPC message) */
        తిరిగి pidref_is_సమితి(pidref) && pidref->fd == -EREMOTE;
}

పూర్ణాంకం pidref_acquire_pidfd_id(PidRef *pidref);
bool pidref_equal(PidRef *a, PidRef *b);

/* This turns a pid_t into a PidRef structure, and acquires a pidfd for it, if possible. (As opposed to
 * PIDREF_MAKE_FROM_PID() above, which does not acquire a pidfd.) */
పూర్ణాంకం pidref_సమితి_pid_నిండిన(PidRef *pidref, pid_t pid, గుర్తులేని flags);
స్థిర అంతఃవరుస పూర్ణాంకం pidref_సమితి_pid(PidRef *pidref, pid_t pid) {
        తిరిగి pidref_సమితి_pid_నిండిన(pidref, pid, /* flags= */ 0);
}
పూర్ణాంకం pidref_సమితి_pidstr_నిండిన(PidRef *pidref, స్థిరరాశి అక్షరం *pid, గుర్తులేని flags);
స్థిర అంతఃవరుస పూర్ణాంకం pidref_సమితి_pidstr(PidRef *pidref, స్థిరరాశి అక్షరం *pid) {
        తిరిగి pidref_సమితి_pidstr_నిండిన(pidref, pid, /* flags= */ 0);
}
పూర్ణాంకం pidref_సమితి_pid_and_pidfd_id(PidRef *pidref, pid_t pid, uint64_t pidfd_id);
పూర్ణాంకం pidref_సమితి_pidfd(PidRef *pidref, పూర్ణాంకం fd);
పూర్ణాంకం pidref_సమితి_pidfd_take(PidRef *pidref, పూర్ణాంకం fd); /* takes ownership of the passed pidfd on success */
పూర్ణాంకం pidref_సమితి_pidfd_consume(PidRef *pidref, పూర్ణాంకం fd); /* takes ownership of the passed pidfd in both success and failure */
పూర్ణాంకం pidref_సమితి_మాతృ(PidRef *ret);
స్థిర అంతఃవరుస పూర్ణాంకం pidref_సమితి_self(PidRef *pidref) {
        తిరిగి pidref_సమితి_pid(pidref, 0);
}

bool pidref_is_self(PidRef *pidref);

శూన్య pidref_పూర్తి(PidRef *pidref);
PidRef* pidref_free(PidRef *pidref);
DEFINE_TRIVIAL_CLEANUP_FUNC(PidRef*, pidref_free);

పూర్ణాంకం pidref_copy(స్థిరరాశి PidRef *pidref, PidRef *ret);
పూర్ణాంకం pidref_dup(స్థిరరాశి PidRef *pidref, PidRef **ret);

పూర్ణాంకం pidref_new_from_pid(pid_t pid, PidRef **ret);

పూర్ణాంకం pidref_kill(స్థిరరాశి PidRef *pidref, పూర్ణాంకం sig);
పూర్ణాంకం pidref_kill_and_sigcont(స్థిరరాశి PidRef *pidref, పూర్ణాంకం sig);
పూర్ణాంకం pidref_sigqueue(స్థిరరాశి PidRef *pidref, పూర్ణాంకం sig, పూర్ణాంకం విలువ);

పూర్ణాంకం pidref_wait_for_terminate_నిండిన(PidRef *pidref, usec_t timeout, siginfo_t *ret_si);
స్థిర అంతఃవరుస పూర్ణాంకం pidref_wait_for_terminate(PidRef *pidref, siginfo_t *ret_si) {
        తిరిగి pidref_wait_for_terminate_నిండిన(pidref, USEC_INFINITY, ret_si);
}

స్థిర అంతఃవరుస శూన్య pidref_పూర్తి_sigterm_wait(PidRef *pidref) {
        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి;

        (శూన్య) pidref_kill(pidref, SIGTERM);
        (శూన్య) pidref_wait_for_terminate(pidref, ఖాళీ);
        pidref_పూర్తి(pidref);
}

స్థిర అంతఃవరుస శూన్య pidref_పూర్తి_sigkill_wait(PidRef *pidref) {
        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి;

        (శూన్య) pidref_kill(pidref, SIGKILL);
        (శూన్య) pidref_wait_for_terminate(pidref, ఖాళీ);
        pidref_పూర్తి(pidref);
}

స్థిర అంతఃవరుస శూన్య pidref_పూర్తి_sigkill_nowait(PidRef *pidref) {
        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి;

        (శూన్య) pidref_kill(pidref, SIGKILL);
        pidref_పూర్తి(pidref);
}

పూర్ణాంకం pidref_verify(స్థిరరాశి PidRef *pidref);

#నిర్వచించు TAKE_PIDREF(p) TAKE_GENERIC((p), PidRef, PIDREF_NULL)

నిర్మాణం siphash;
శూన్య pidref_hash_func(స్థిరరాశి PidRef *pidref, నిర్మాణం siphash *state);
పూర్ణాంకం pidref_compare_func(స్థిరరాశి PidRef *a, స్థిరరాశి PidRef *b);

బాహ్య స్థిరరాశి నిర్మాణం hash_ops pidref_hash_ops;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops pidref_hash_ops_free; /* Has destructor call for pidref_free(), i.e. expects heap allocated PidRef as keys */

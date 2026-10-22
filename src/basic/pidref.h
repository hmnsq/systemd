/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <signal.h>

#شامل "forward.h"

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
قسمتعریف ساخت PidRef {
        pid_t pid;      /* > 0 if the PidRef is set, otherwise set to INT_MIN (PID_AUTOMATIC) if automatic
                         * mode is desired, or 0 otherwise. */
        صحیح fd;         /* only valid if pidfd are available in the kernel, and we manage to get an fd. If we
                         * know that the PID is not from the local machine we set this to -EREMOTE, otherwise
                         * we use -EBADF as indicator the fd is invalid. */
        uint64_t fd_id; /* the inode number of pidfd. only useful in kernel 6.9+ where pidfds live in
                           their own pidfs and each process comes with a unique inode number */
} PidRef;

#تعریف_کرو PIDREF_NULL (PidRef) { .fd = -EBADF }

/* A special pidref value that we are using when a PID shall be automatically acquired from some surrounding
 * context, for example connection peer. Much like PIDREF_NULL it will be considered unset by
 * pidref_is_set(). */
#تعریف_کرو PIDREF_AUTOMATIC (ثابت PidRef) { .pid = (pid_t) INT_MIN, .fd = -EBADF }

/* Turns a pid_t into a PidRef structure on-the-fly *without* acquiring a pidfd for it. (As opposed to
 * pidref_set_pid() which does so *with* acquiring one, see below) */
#تعریف_کرو PIDREF_MAKE_FROM_PID(x) (PidRef) { .pid = (x), .fd = -EBADF }

ساکن اندرونی bool pidref_is_سیٹ(ثابت PidRef *pidref) {
        واپس pidref && pidref->pid > 0;
}

bool pidref_is_automatic(ثابت PidRef *pidref);

ساکن اندرونی bool pidref_is_سیٹ_or_automatic(ثابت PidRef *pidref) {
        واپس pidref_is_سیٹ(pidref) || pidref_is_automatic(pidref);
}

ساکن اندرونی bool pidref_is_remote(ثابت PidRef *pidref) {
        /* If the fd is set to -EREMOTE we assume PidRef does not refer to a local PID, but on another
         * machine (and we just got the PidRef initialized due to deserialization of some RPC message) */
        واپس pidref_is_سیٹ(pidref) && pidref->fd == -EREMOTE;
}

صحیح pidref_acquire_pidfd_id(PidRef *pidref);
bool pidref_equal(PidRef *a, PidRef *b);

/* This turns a pid_t into a PidRef structure, and acquires a pidfd for it, if possible. (As opposed to
 * PIDREF_MAKE_FROM_PID() above, which does not acquire a pidfd.) */
صحیح pidref_سیٹ_pid_بھرا(PidRef *pidref, pid_t pid, غیرعلامتی flags);
ساکن اندرونی صحیح pidref_سیٹ_pid(PidRef *pidref, pid_t pid) {
        واپس pidref_سیٹ_pid_بھرا(pidref, pid, /* flags= */ 0);
}
صحیح pidref_سیٹ_pidstr_بھرا(PidRef *pidref, ثابت حرف *pid, غیرعلامتی flags);
ساکن اندرونی صحیح pidref_سیٹ_pidstr(PidRef *pidref, ثابت حرف *pid) {
        واپس pidref_سیٹ_pidstr_بھرا(pidref, pid, /* flags= */ 0);
}
صحیح pidref_سیٹ_pid_and_pidfd_id(PidRef *pidref, pid_t pid, uint64_t pidfd_id);
صحیح pidref_سیٹ_pidfd(PidRef *pidref, صحیح fd);
صحیح pidref_سیٹ_pidfd_take(PidRef *pidref, صحیح fd); /* takes ownership of the passed pidfd on success */
صحیح pidref_سیٹ_pidfd_consume(PidRef *pidref, صحیح fd); /* takes ownership of the passed pidfd in both success and failure */
صحیح pidref_سیٹ_والد(PidRef *ret);
ساکن اندرونی صحیح pidref_سیٹ_self(PidRef *pidref) {
        واپس pidref_سیٹ_pid(pidref, 0);
}

bool pidref_is_self(PidRef *pidref);

خالی pidref_مکمل(PidRef *pidref);
PidRef* pidref_free(PidRef *pidref);
DEFINE_TRIVIAL_CLEANUP_FUNC(PidRef*, pidref_free);

صحیح pidref_copy(ثابت PidRef *pidref, PidRef *ret);
صحیح pidref_dup(ثابت PidRef *pidref, PidRef **ret);

صحیح pidref_new_from_pid(pid_t pid, PidRef **ret);

صحیح pidref_kill(ثابت PidRef *pidref, صحیح sig);
صحیح pidref_kill_and_sigcont(ثابت PidRef *pidref, صحیح sig);
صحیح pidref_sigqueue(ثابت PidRef *pidref, صحیح sig, صحیح قدر);

صحیح pidref_wait_for_terminate_بھرا(PidRef *pidref, usec_t timeout, siginfo_t *ret_si);
ساکن اندرونی صحیح pidref_wait_for_terminate(PidRef *pidref, siginfo_t *ret_si) {
        واپس pidref_wait_for_terminate_بھرا(pidref, USEC_INFINITY, ret_si);
}

ساکن اندرونی خالی pidref_مکمل_sigterm_wait(PidRef *pidref) {
        اگر (!pidref_is_سیٹ(pidref))
                واپس;

        (خالی) pidref_kill(pidref, SIGTERM);
        (خالی) pidref_wait_for_terminate(pidref, تہی);
        pidref_مکمل(pidref);
}

ساکن اندرونی خالی pidref_مکمل_sigkill_wait(PidRef *pidref) {
        اگر (!pidref_is_سیٹ(pidref))
                واپس;

        (خالی) pidref_kill(pidref, SIGKILL);
        (خالی) pidref_wait_for_terminate(pidref, تہی);
        pidref_مکمل(pidref);
}

ساکن اندرونی خالی pidref_مکمل_sigkill_nowait(PidRef *pidref) {
        اگر (!pidref_is_سیٹ(pidref))
                واپس;

        (خالی) pidref_kill(pidref, SIGKILL);
        pidref_مکمل(pidref);
}

صحیح pidref_verify(ثابت PidRef *pidref);

#تعریف_کرو TAKE_PIDREF(p) TAKE_GENERIC((p), PidRef, PIDREF_NULL)

ساخت siphash;
خالی pidref_hash_func(ثابت PidRef *pidref, ساخت siphash *state);
صحیح pidref_compare_func(ثابت PidRef *a, ثابت PidRef *b);

خارجی ثابت ساخت hash_ops pidref_hash_ops;
خارجی ثابت ساخت hash_ops pidref_hash_ops_free; /* Has destructor call for pidref_free(), i.e. expects heap allocated PidRef as keys */

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_RESTART_USEC (100*USEC_ਪ੍ਰਤੀ_MSEC)

/* Many different things, but also system unit start/stop */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_TIMEOUT_USEC (DEFAULT_TIMEOUT_SEC*USEC_ਪ੍ਰਤੀ_SEC)
/* User unit start/stop */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_USER_TIMEOUT_USEC (DEFAULT_USER_TIMEOUT_SEC*USEC_ਪ੍ਰਤੀ_SEC)
/* Timeout for user confirmation on the console */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_CONFIRM_USEC (30*USEC_ਪ੍ਰਤੀ_SEC)

/* We use an extra-long timeout for the reload. This is because a reload or reexec means generators are rerun
 * which are timed out after DEFAULT_TIMEOUT_USEC. Let's use twice that time here, so that the generators can
 * have their timeout, and for everything else there's the same time budget in place. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DAEMON_RELOAD_TIMEOUT_SEC (DEFAULT_TIMEOUT_USEC * 2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_ਸ਼ੁਰੂ_ਹੱਦਬੰਦੀ_INTERVAL (10*USEC_ਪ੍ਰਤੀ_SEC)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_ਸ਼ੁਰੂ_ਹੱਦਬੰਦੀ_ਮਿਆਦ 5

/* The default time after which exit-on-idle services exit. This
 * should be kept lower than the watchdog timeout, because otherwise
 * the watchdog pings will keep the loop busy. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_EXIT_USEC (30*USEC_ਪ੍ਰਤੀ_SEC)

/* The default value for the net.unix.max_dgram_qlen sysctl */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_UNIX_MAX_DGRAM_QLEN 512

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਕੇਤ_CRASH_HANDLER SIGSEGV,SIGILL,SIGFPE,SIGBUS,SIGQUIT,SIGABRT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਕੇਤ_IGNORE SIGPIPE

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NOTIFY_FD_MAX 768
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NOTIFY_ਬਫਰ_MAX PIPE_BUF

/* Return a nulstr for a standard cascade of configuration paths, suitable to pass to
 * conf_files_list_nulstr() to implement drop-in directories for extending configuration files. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONF_PATHS_NULSTR(n)                    \
        "/etc/" n "\0"                          \
        "/run/" n "\0"                          \
        "/usr/local/lib/" n "\0"                \
        "/usr/lib/" n "\0"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONF_PATHS(n)                           \
        "/etc/" n,                              \
        "/run/" n,                              \
        "/usr/local/lib/" n,                    \
        "/usr/lib/" n

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONF_PATHS_STRV(n)                      \
        STRV_MAKE(CONF_PATHS(n))

/* The limit for PID 1 itself (which is not inherited to children) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਉੱਚਾ_RLIMIT_MEMLOCK (1024ULL*1024ULL*64ULL)

/* Since kernel 5.16 the kernel default limit was raised to 8M. Let's adjust things on old kernels too, and
 * in containers so that our children inherit that. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_RLIMIT_MEMLOCK (1024ULL*1024ULL*8ULL)

/* Path where PID1 listens for varlink subscriptions from systemd-oomd to notify of changes in ManagedOOM settings. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VARLINK_ਰਸਤਾ_MANAGED_OOM_SYSTEM "/run/systemd/io.systemd.ManagedOOM"
/* Path where systemd-oomd listens for varlink connections from user managers to report changes in ManagedOOM settings. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VARLINK_ਰਸਤਾ_MANAGED_OOM_USER "/run/systemd/oom/io.systemd.ManagedOOM"
/* Path where systemd-machined listens to userdb varlink queries */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VARLINK_ਰਸਤਾ_MACHINED_USERDB "/run/systemd/userdb/io.systemd.Machine"
/* Path where systemd-machined listens to resolve.hook varlink queries */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VARLINK_ਰਸਤਾ_MACHINED_RESOLVE_HOOK "/run/systemd/resolve.hook/io.systemd.Machine"
/* Path where to connect to send varlink prekill events */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VARLINK_DIR_OOMD_PREKILL_HOOK "/run/systemd/oomd.prekill.hook/"
/* Directory whose sockets receive io.systemd.SysUpdate.Notify() after a successful sysupdate run */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VARLINK_DIR_SYSUPDATE_NOTIFY_HOOK "/run/systemd/sysupdate/notify/"
/* Directory whose sockets systemd-sysupdate queries via io.systemd.SysUpdate.Provider for component definitions */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VARLINK_DIR_SYSUPDATE_PROVIDER "/run/systemd/sysupdate/provider/"

/* Recommended baseline - see README for details */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KERNEL_BASELINE_ਸੰਸਕਰਣ "5.14"

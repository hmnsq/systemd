/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "time-util.h"

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_RESTART_USEC (100*USEC_ಪ್ರತಿ_MSEC)

/* Many different things, but also system unit start/stop */
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_TIMEOUT_USEC (DEFAULT_TIMEOUT_SEC*USEC_ಪ್ರತಿ_SEC)
/* User unit start/stop */
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_USER_TIMEOUT_USEC (DEFAULT_USER_TIMEOUT_SEC*USEC_ಪ್ರತಿ_SEC)
/* Timeout for user confirmation on the console */
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_CONFIRM_USEC (30*USEC_ಪ್ರತಿ_SEC)

/* We use an extra-long timeout for the reload. This is because a reload or reexec means generators are rerun
 * which are timed out after DEFAULT_TIMEOUT_USEC. Let's use twice that time here, so that the generators can
 * have their timeout, and for everything else there's the same time budget in place. */
#ವ್ಯಾಖ್ಯಾನಿಸು DAEMON_RELOAD_TIMEOUT_SEC (DEFAULT_TIMEOUT_USEC * 2)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_ಆರಂಭ_ಎಲ್ಲೆ_INTERVAL (10*USEC_ಪ್ರತಿ_SEC)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_ಆರಂಭ_ಎಲ್ಲೆ_ಅವಧಿ 5

/* The default time after which exit-on-idle services exit. This
 * should be kept lower than the watchdog timeout, because otherwise
 * the watchdog pings will keep the loop busy. */
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_EXIT_USEC (30*USEC_ಪ್ರತಿ_SEC)

/* The default value for the net.unix.max_dgram_qlen sysctl */
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_UNIX_MAX_DGRAM_QLEN 512

#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂಕೇತಗಳು_CRASH_HANDLER SIGSEGV,SIGILL,SIGFPE,SIGBUS,SIGQUIT,SIGABRT
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂಕೇತಗಳು_IGNORE SIGPIPE

#ವ್ಯಾಖ್ಯಾನಿಸು NOTIFY_FD_MAX 768
#ವ್ಯಾಖ್ಯಾನಿಸು NOTIFY_ಬಫರ್_MAX PIPE_BUF

/* Return a nulstr for a standard cascade of configuration paths, suitable to pass to
 * conf_files_list_nulstr() to implement drop-in directories for extending configuration files. */
#ವ್ಯಾಖ್ಯಾನಿಸು CONF_PATHS_NULSTR(n)                    \
        "/etc/" n "\0"                          \
        "/run/" n "\0"                          \
        "/usr/local/lib/" n "\0"                \
        "/usr/lib/" n "\0"

#ವ್ಯಾಖ್ಯಾನಿಸು CONF_PATHS(n)                           \
        "/etc/" n,                              \
        "/run/" n,                              \
        "/usr/local/lib/" n,                    \
        "/usr/lib/" n

#ವ್ಯಾಖ್ಯಾನಿಸು CONF_PATHS_STRV(n)                      \
        STRV_MAKE(CONF_PATHS(n))

/* The limit for PID 1 itself (which is not inherited to children) */
#ವ್ಯಾಖ್ಯಾನಿಸು ಮೇಲಿನ_RLIMIT_MEMLOCK (1024ULL*1024ULL*64ULL)

/* Since kernel 5.16 the kernel default limit was raised to 8M. Let's adjust things on old kernels too, and
 * in containers so that our children inherit that. */
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_RLIMIT_MEMLOCK (1024ULL*1024ULL*8ULL)

/* Path where PID1 listens for varlink subscriptions from systemd-oomd to notify of changes in ManagedOOM settings. */
#ವ್ಯಾಖ್ಯಾನಿಸು VARLINK_ಮಾರ್ಗ_MANAGED_OOM_SYSTEM "/run/systemd/io.systemd.ManagedOOM"
/* Path where systemd-oomd listens for varlink connections from user managers to report changes in ManagedOOM settings. */
#ವ್ಯಾಖ್ಯಾನಿಸು VARLINK_ಮಾರ್ಗ_MANAGED_OOM_USER "/run/systemd/oom/io.systemd.ManagedOOM"
/* Path where systemd-machined listens to userdb varlink queries */
#ವ್ಯಾಖ್ಯಾನಿಸು VARLINK_ಮಾರ್ಗ_MACHINED_USERDB "/run/systemd/userdb/io.systemd.Machine"
/* Path where systemd-machined listens to resolve.hook varlink queries */
#ವ್ಯಾಖ್ಯಾನಿಸು VARLINK_ಮಾರ್ಗ_MACHINED_RESOLVE_HOOK "/run/systemd/resolve.hook/io.systemd.Machine"
/* Path where to connect to send varlink prekill events */
#ವ್ಯಾಖ್ಯಾನಿಸು VARLINK_DIR_OOMD_PREKILL_HOOK "/run/systemd/oomd.prekill.hook/"
/* Directory whose sockets receive io.systemd.SysUpdate.Notify() after a successful sysupdate run */
#ವ್ಯಾಖ್ಯಾನಿಸು VARLINK_DIR_SYSUPDATE_NOTIFY_HOOK "/run/systemd/sysupdate/notify/"
/* Directory whose sockets systemd-sysupdate queries via io.systemd.SysUpdate.Provider for component definitions */
#ವ್ಯಾಖ್ಯಾನಿಸು VARLINK_DIR_SYSUPDATE_PROVIDER "/run/systemd/sysupdate/provider/"

/* Recommended baseline - see README for details */
#ವ್ಯಾಖ್ಯಾನಿಸು KERNEL_BASELINE_ಆವೃತ್ತಿ "5.14"

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_DEFAULT_ಗುರಿ "default.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_INITRD_ಗುರಿ "initrd.target"

/* Shutdown targets */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_UMOUNT_ಗುರಿ "umount.target"
/* This is not really intended to be started by directly. This is
 * mostly so that other targets (reboot/halt/poweroff) can depend on
 * it to bring all services down that want to be brought down on
 * system shutdown. */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_SHUTDOWN_ಗುರಿ "shutdown.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_HALT_ಗುರಿ "halt.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_POWEROFF_ಗುರಿ "poweroff.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_REBOOT_ಗುರಿ "reboot.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_SOFT_REBOOT_ಗುರಿ "soft-reboot.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_KEXEC_ಗುರಿ "kexec.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_EXIT_ಗುರಿ "exit.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_SUSPEND_ಗುರಿ "suspend.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_HIBERNATE_ಗುರಿ "hibernate.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_HYBRID_SLEEP_ಗುರಿ "hybrid-sleep.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_SUSPEND_THEN_HIBERNATE_ಗುರಿ "suspend-then-hibernate.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_FACTORY_RESET_ಗುರಿ "factory-reset.target"

/* Special boot targets */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_RESCUE_ಗುರಿ "rescue.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_EMERGENCY_ಗುರಿ "emergency.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_MULTI_USER_ಗುರಿ "multi-user.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_GRAPHICAL_ಗುರಿ "graphical.target"

/* Early boot targets */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_SYSINIT_ಗುರಿ "sysinit.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_SOCKETS_ಗುರಿ "sockets.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_TIMERS_ಗುರಿ "timers.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_PATHS_ಗುರಿ "paths.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_LOCAL_FS_ಗುರಿ "local-fs.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_LOCAL_FS_PRE_ಗುರಿ "local-fs-pre.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_INITRD_FS_ಗುರಿ "initrd-fs.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_INITRD_ಬೇರು_DEVICE_ಗುರಿ "initrd-root-device.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_INITRD_ಬೇರು_FS_ಗುರಿ "initrd-root-fs.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_INITRD_USR_FS_ಗುರಿ "initrd-usr-fs.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_REMOTE_FS_ಗುರಿ "remote-fs.target"       /* LSB's $remote_fs */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_REMOTE_FS_PRE_ಗುರಿ "remote-fs-pre.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_ಅದಲುಬದಲು_ಗುರಿ "swap.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_ಜಾಲ_ONLINE_ಗುರಿ "network-online.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_ಸಮಯ_SYNC_ಗುರಿ "time-sync.target"       /* LSB's $time */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_ಸಮಯ_ಗಣ_ಗುರಿ "time-set.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_BASIC_ಗುರಿ "basic.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_TPM2_ಗುರಿ "tpm2.target"

/* LSB compatibility */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_ಜಾಲ_ಗುರಿ "network.target"           /* LSB's $network */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_NSS_LOOKUP_ಗುರಿ "nss-lookup.target"     /* LSB's $named */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_RPCBIND_ಗುರಿ "rpcbind.target"           /* LSB's $portmap */

/*
 * Rules regarding adding further high level targets like the above:
 *
 * - Be conservative, only add more of these when we really need
 *   them. We need strong use cases for further additions.
 *
 * - When there can be multiple implementations running side-by-side,
 *   it needs to be a .target unit which can pull in all
 *   implementations.
 *
 * - If something can be implemented with socket activation, and
 *   without, it needs to be a .target unit, so that it can pull in
 *   the appropriate unit.
 *
 * - Otherwise, it should be a .service unit.
 *
 * - In some cases it is OK to have both a .service and a .target
 *   unit, i.e. if there can be multiple parallel implementations, but
 *   only one is the "system" one. Example: syslog.
 *
 * Or to put this in other words: .service symlinks can be used to
 * arbitrate between multiple implementations if there can be only one
 * of a kind. .target units can be used to support multiple
 * implementations that can run side-by-side.
 */

/* Magic early boot services */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_FSCK_SERVICE "systemd-fsck@.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_FSCK_ಬೇರು_SERVICE "systemd-fsck-root.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_FSCK_USR_SERVICE "systemd-fsck-usr.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_QUOTACHECK_SERVICE "systemd-quotacheck@.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_QUOTACHECK_ಬೇರು_SERVICE "systemd-quotacheck-root.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_QUOTAON_SERVICE "quotaon@.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_QUOTAON_ಬೇರು_SERVICE "quotaon-root.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_REMOUNT_FS_SERVICE "systemd-remount-fs.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_VOLATILE_ಬೇರು_SERVICE "systemd-volatile-root.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_UDEVD_SERVICE "systemd-udevd.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_GROWFS_SERVICE "systemd-growfs@.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_GROWFS_ಬೇರು_SERVICE "systemd-growfs-root.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_PCRFS_SERVICE "systemd-pcrfs@.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_PCRFS_ಬೇರು_SERVICE "systemd-pcrfs-root.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_VALIDATEFS_SERVICE "systemd-validatefs@.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_HIBERNATE_RESUME_SERVICE "systemd-hibernate-resume.service"

/* Services systemd relies on */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_DBUS_SERVICE "dbus.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_DBUS_SOCKET "dbus.socket"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_JOURNALD_SOCKET "systemd-journald.socket"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_JOURNALD_SERVICE "systemd-journald.service"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_PTYBROKERD_SOCKET "systemd-ptybrokerd.socket"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_TMPFILES_ಸಿದ್ಧತೆ_SERVICE "systemd-tmpfiles-setup.service"

/* Magic init signals */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_KBREQUEST_ಗುರಿ "kbrequest.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_SIGPWR_ಗುರಿ "sigpwr.target"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_CTRL_ALT_DEL_ಗುರಿ "ctrl-alt-del.target"

/* The unit to use as fallback if ctrl-alt-del.target (which is usually an alias) cannot be loaded. */
#ವ್ಯಾಖ್ಯಾನಿಸು FALLBACK_CTRL_ALT_DEL_ಗುರಿ "reboot.target"

/* Where we add all our system units, users and machines by default */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_SYSTEM_ತುಂಡು "system.slice"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_USER_ತುಂಡು "user.slice"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_MACHINE_ತುಂಡು "machine.slice"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_ಬೇರು_ತುಂಡು "-.slice"

/* The scope unit systemd itself lives in. */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_INIT_SCOPE "init.scope"

/* The root directory. */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_ಬೇರು_MOUNT "-.mount"

/* Special slices valid for the user instance */
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_SESSION_ತುಂಡು "session.slice"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_APP_ತುಂಡು "app.slice"
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_BACKGROUND_ತುಂಡು "background.slice"

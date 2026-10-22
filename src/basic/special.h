/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#تعریف_کرو SPECIAL_DEFAULT_ہدف "default.target"
#تعریف_کرو SPECIAL_INITRD_ہدف "initrd.target"

/* Shutdown targets */
#تعریف_کرو SPECIAL_UMOUNT_ہدف "umount.target"
/* This is not really intended to be started by directly. This is
 * mostly so that other targets (reboot/halt/poweroff) can depend on
 * it to bring all services down that want to be brought down on
 * system shutdown. */
#تعریف_کرو SPECIAL_SHUTDOWN_ہدف "shutdown.target"
#تعریف_کرو SPECIAL_HALT_ہدف "halt.target"
#تعریف_کرو SPECIAL_POWEROFF_ہدف "poweroff.target"
#تعریف_کرو SPECIAL_REBOOT_ہدف "reboot.target"
#تعریف_کرو SPECIAL_SOFT_REBOOT_ہدف "soft-reboot.target"
#تعریف_کرو SPECIAL_KEXEC_ہدف "kexec.target"
#تعریف_کرو SPECIAL_EXIT_ہدف "exit.target"
#تعریف_کرو SPECIAL_SUSPEND_ہدف "suspend.target"
#تعریف_کرو SPECIAL_HIBERNATE_ہدف "hibernate.target"
#تعریف_کرو SPECIAL_HYBRID_SLEEP_ہدف "hybrid-sleep.target"
#تعریف_کرو SPECIAL_SUSPEND_THEN_HIBERNATE_ہدف "suspend-then-hibernate.target"
#تعریف_کرو SPECIAL_FACTORY_RESET_ہدف "factory-reset.target"

/* Special boot targets */
#تعریف_کرو SPECIAL_RESCUE_ہدف "rescue.target"
#تعریف_کرو SPECIAL_EMERGENCY_ہدف "emergency.target"
#تعریف_کرو SPECIAL_MULTI_USER_ہدف "multi-user.target"
#تعریف_کرو SPECIAL_GRAPHICAL_ہدف "graphical.target"

/* Early boot targets */
#تعریف_کرو SPECIAL_SYSINIT_ہدف "sysinit.target"
#تعریف_کرو SPECIAL_SOCKETS_ہدف "sockets.target"
#تعریف_کرو SPECIAL_TIMERS_ہدف "timers.target"
#تعریف_کرو SPECIAL_PATHS_ہدف "paths.target"
#تعریف_کرو SPECIAL_LOCAL_FS_ہدف "local-fs.target"
#تعریف_کرو SPECIAL_LOCAL_FS_PRE_ہدف "local-fs-pre.target"
#تعریف_کرو SPECIAL_INITRD_FS_ہدف "initrd-fs.target"
#تعریف_کرو SPECIAL_INITRD_جڑ_DEVICE_ہدف "initrd-root-device.target"
#تعریف_کرو SPECIAL_INITRD_جڑ_FS_ہدف "initrd-root-fs.target"
#تعریف_کرو SPECIAL_INITRD_USR_FS_ہدف "initrd-usr-fs.target"
#تعریف_کرو SPECIAL_REMOTE_FS_ہدف "remote-fs.target"       /* LSB's $remote_fs */
#تعریف_کرو SPECIAL_REMOTE_FS_PRE_ہدف "remote-fs-pre.target"
#تعریف_کرو SPECIAL_تبادلہ_ہدف "swap.target"
#تعریف_کرو SPECIAL_جال_ONLINE_ہدف "network-online.target"
#تعریف_کرو SPECIAL_وقت_SYNC_ہدف "time-sync.target"       /* LSB's $time */
#تعریف_کرو SPECIAL_وقت_سیٹ_ہدف "time-set.target"
#تعریف_کرو SPECIAL_BASIC_ہدف "basic.target"
#تعریف_کرو SPECIAL_TPM2_ہدف "tpm2.target"

/* LSB compatibility */
#تعریف_کرو SPECIAL_جال_ہدف "network.target"           /* LSB's $network */
#تعریف_کرو SPECIAL_NSS_LOOKUP_ہدف "nss-lookup.target"     /* LSB's $named */
#تعریف_کرو SPECIAL_RPCBIND_ہدف "rpcbind.target"           /* LSB's $portmap */

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
#تعریف_کرو SPECIAL_FSCK_SERVICE "systemd-fsck@.service"
#تعریف_کرو SPECIAL_FSCK_جڑ_SERVICE "systemd-fsck-root.service"
#تعریف_کرو SPECIAL_FSCK_USR_SERVICE "systemd-fsck-usr.service"
#تعریف_کرو SPECIAL_QUOTACHECK_SERVICE "systemd-quotacheck@.service"
#تعریف_کرو SPECIAL_QUOTACHECK_جڑ_SERVICE "systemd-quotacheck-root.service"
#تعریف_کرو SPECIAL_QUOTAON_SERVICE "quotaon@.service"
#تعریف_کرو SPECIAL_QUOTAON_جڑ_SERVICE "quotaon-root.service"
#تعریف_کرو SPECIAL_REMOUNT_FS_SERVICE "systemd-remount-fs.service"
#تعریف_کرو SPECIAL_VOLATILE_جڑ_SERVICE "systemd-volatile-root.service"
#تعریف_کرو SPECIAL_UDEVD_SERVICE "systemd-udevd.service"
#تعریف_کرو SPECIAL_GROWFS_SERVICE "systemd-growfs@.service"
#تعریف_کرو SPECIAL_GROWFS_جڑ_SERVICE "systemd-growfs-root.service"
#تعریف_کرو SPECIAL_PCRFS_SERVICE "systemd-pcrfs@.service"
#تعریف_کرو SPECIAL_PCRFS_جڑ_SERVICE "systemd-pcrfs-root.service"
#تعریف_کرو SPECIAL_VALIDATEFS_SERVICE "systemd-validatefs@.service"
#تعریف_کرو SPECIAL_HIBERNATE_RESUME_SERVICE "systemd-hibernate-resume.service"

/* Services systemd relies on */
#تعریف_کرو SPECIAL_DBUS_SERVICE "dbus.service"
#تعریف_کرو SPECIAL_DBUS_SOCKET "dbus.socket"
#تعریف_کرو SPECIAL_JOURNALD_SOCKET "systemd-journald.socket"
#تعریف_کرو SPECIAL_JOURNALD_SERVICE "systemd-journald.service"
#تعریف_کرو SPECIAL_PTYBROKERD_SOCKET "systemd-ptybrokerd.socket"
#تعریف_کرو SPECIAL_TMPFILES_تیاری_SERVICE "systemd-tmpfiles-setup.service"

/* Magic init signals */
#تعریف_کرو SPECIAL_KBREQUEST_ہدف "kbrequest.target"
#تعریف_کرو SPECIAL_SIGPWR_ہدف "sigpwr.target"
#تعریف_کرو SPECIAL_CTRL_ALT_DEL_ہدف "ctrl-alt-del.target"

/* The unit to use as fallback if ctrl-alt-del.target (which is usually an alias) cannot be loaded. */
#تعریف_کرو FALLBACK_CTRL_ALT_DEL_ہدف "reboot.target"

/* Where we add all our system units, users and machines by default */
#تعریف_کرو SPECIAL_SYSTEM_قاش "system.slice"
#تعریف_کرو SPECIAL_USER_قاش "user.slice"
#تعریف_کرو SPECIAL_MACHINE_قاش "machine.slice"
#تعریف_کرو SPECIAL_جڑ_قاش "-.slice"

/* The scope unit systemd itself lives in. */
#تعریف_کرو SPECIAL_INIT_SCOPE "init.scope"

/* The root directory. */
#تعریف_کرو SPECIAL_جڑ_MOUNT "-.mount"

/* Special slices valid for the user instance */
#تعریف_کرو SPECIAL_SESSION_قاش "session.slice"
#تعریف_کرو SPECIAL_APP_قاش "app.slice"
#تعریف_کرو SPECIAL_BACKGROUND_قاش "background.slice"

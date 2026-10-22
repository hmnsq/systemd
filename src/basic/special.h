/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#وصف_ڪريو SPECIAL_DEFAULT_هدف "default.target"
#وصف_ڪريو SPECIAL_INITRD_هدف "initrd.target"

/* Shutdown targets */
#وصف_ڪريو SPECIAL_UMOUNT_هدف "umount.target"
/* This is not really intended to be started by directly. This is
 * mostly so that other targets (reboot/halt/poweroff) can depend on
 * it to bring all services down that want to be brought down on
 * system shutdown. */
#وصف_ڪريو SPECIAL_SHUTDOWN_هدف "shutdown.target"
#وصف_ڪريو SPECIAL_HALT_هدف "halt.target"
#وصف_ڪريو SPECIAL_POWEROFF_هدف "poweroff.target"
#وصف_ڪريو SPECIAL_REBOOT_هدف "reboot.target"
#وصف_ڪريو SPECIAL_SOFT_REBOOT_هدف "soft-reboot.target"
#وصف_ڪريو SPECIAL_KEXEC_هدف "kexec.target"
#وصف_ڪريو SPECIAL_EXIT_هدف "exit.target"
#وصف_ڪريو SPECIAL_SUSPEND_هدف "suspend.target"
#وصف_ڪريو SPECIAL_HIBERNATE_هدف "hibernate.target"
#وصف_ڪريو SPECIAL_HYBRID_SLEEP_هدف "hybrid-sleep.target"
#وصف_ڪريو SPECIAL_SUSPEND_THEN_HIBERNATE_هدف "suspend-then-hibernate.target"
#وصف_ڪريو SPECIAL_FACTORY_RESET_هدف "factory-reset.target"

/* Special boot targets */
#وصف_ڪريو SPECIAL_RESCUE_هدف "rescue.target"
#وصف_ڪريو SPECIAL_EMERGENCY_هدف "emergency.target"
#وصف_ڪريو SPECIAL_MULTI_USER_هدف "multi-user.target"
#وصف_ڪريو SPECIAL_GRAPHICAL_هدف "graphical.target"

/* Early boot targets */
#وصف_ڪريو SPECIAL_SYSINIT_هدف "sysinit.target"
#وصف_ڪريو SPECIAL_SOCKETS_هدف "sockets.target"
#وصف_ڪريو SPECIAL_TIMERS_هدف "timers.target"
#وصف_ڪريو SPECIAL_PATHS_هدف "paths.target"
#وصف_ڪريو SPECIAL_LOCAL_FS_هدف "local-fs.target"
#وصف_ڪريو SPECIAL_LOCAL_FS_PRE_هدف "local-fs-pre.target"
#وصف_ڪريو SPECIAL_INITRD_FS_هدف "initrd-fs.target"
#وصف_ڪريو SPECIAL_INITRD_پاڙ_DEVICE_هدف "initrd-root-device.target"
#وصف_ڪريو SPECIAL_INITRD_پاڙ_FS_هدف "initrd-root-fs.target"
#وصف_ڪريو SPECIAL_INITRD_USR_FS_هدف "initrd-usr-fs.target"
#وصف_ڪريو SPECIAL_REMOTE_FS_هدف "remote-fs.target"       /* LSB's $remote_fs */
#وصف_ڪريو SPECIAL_REMOTE_FS_PRE_هدف "remote-fs-pre.target"
#وصف_ڪريو SPECIAL_مٽا_سٽا_هدف "swap.target"
#وصف_ڪريو SPECIAL_ڄار_ONLINE_هدف "network-online.target"
#وصف_ڪريو SPECIAL_وقت_SYNC_هدف "time-sync.target"       /* LSB's $time */
#وصف_ڪريو SPECIAL_وقت_سيٽ_هدف "time-set.target"
#وصف_ڪريو SPECIAL_BASIC_هدف "basic.target"
#وصف_ڪريو SPECIAL_TPM2_هدف "tpm2.target"

/* LSB compatibility */
#وصف_ڪريو SPECIAL_ڄار_هدف "network.target"           /* LSB's $network */
#وصف_ڪريو SPECIAL_NSS_LOOKUP_هدف "nss-lookup.target"     /* LSB's $named */
#وصف_ڪريو SPECIAL_RPCBIND_هدف "rpcbind.target"           /* LSB's $portmap */

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
#وصف_ڪريو SPECIAL_FSCK_SERVICE "systemd-fsck@.service"
#وصف_ڪريو SPECIAL_FSCK_پاڙ_SERVICE "systemd-fsck-root.service"
#وصف_ڪريو SPECIAL_FSCK_USR_SERVICE "systemd-fsck-usr.service"
#وصف_ڪريو SPECIAL_QUOTACHECK_SERVICE "systemd-quotacheck@.service"
#وصف_ڪريو SPECIAL_QUOTACHECK_پاڙ_SERVICE "systemd-quotacheck-root.service"
#وصف_ڪريو SPECIAL_QUOTAON_SERVICE "quotaon@.service"
#وصف_ڪريو SPECIAL_QUOTAON_پاڙ_SERVICE "quotaon-root.service"
#وصف_ڪريو SPECIAL_REMOUNT_FS_SERVICE "systemd-remount-fs.service"
#وصف_ڪريو SPECIAL_VOLATILE_پاڙ_SERVICE "systemd-volatile-root.service"
#وصف_ڪريو SPECIAL_UDEVD_SERVICE "systemd-udevd.service"
#وصف_ڪريو SPECIAL_GROWFS_SERVICE "systemd-growfs@.service"
#وصف_ڪريو SPECIAL_GROWFS_پاڙ_SERVICE "systemd-growfs-root.service"
#وصف_ڪريو SPECIAL_PCRFS_SERVICE "systemd-pcrfs@.service"
#وصف_ڪريو SPECIAL_PCRFS_پاڙ_SERVICE "systemd-pcrfs-root.service"
#وصف_ڪريو SPECIAL_VALIDATEFS_SERVICE "systemd-validatefs@.service"
#وصف_ڪريو SPECIAL_HIBERNATE_RESUME_SERVICE "systemd-hibernate-resume.service"

/* Services systemd relies on */
#وصف_ڪريو SPECIAL_DBUS_SERVICE "dbus.service"
#وصف_ڪريو SPECIAL_DBUS_SOCKET "dbus.socket"
#وصف_ڪريو SPECIAL_JOURNALD_SOCKET "systemd-journald.socket"
#وصف_ڪريو SPECIAL_JOURNALD_SERVICE "systemd-journald.service"
#وصف_ڪريو SPECIAL_PTYBROKERD_SOCKET "systemd-ptybrokerd.socket"
#وصف_ڪريو SPECIAL_TMPFILES_تياري_SERVICE "systemd-tmpfiles-setup.service"

/* Magic init signals */
#وصف_ڪريو SPECIAL_KBREQUEST_هدف "kbrequest.target"
#وصف_ڪريو SPECIAL_SIGPWR_هدف "sigpwr.target"
#وصف_ڪريو SPECIAL_CTRL_ALT_DEL_هدف "ctrl-alt-del.target"

/* The unit to use as fallback if ctrl-alt-del.target (which is usually an alias) cannot be loaded. */
#وصف_ڪريو FALLBACK_CTRL_ALT_DEL_هدف "reboot.target"

/* Where we add all our system units, users and machines by default */
#وصف_ڪريو SPECIAL_SYSTEM_ٽڪرو "system.slice"
#وصف_ڪريو SPECIAL_USER_ٽڪرو "user.slice"
#وصف_ڪريو SPECIAL_MACHINE_ٽڪرو "machine.slice"
#وصف_ڪريو SPECIAL_پاڙ_ٽڪرو "-.slice"

/* The scope unit systemd itself lives in. */
#وصف_ڪريو SPECIAL_INIT_SCOPE "init.scope"

/* The root directory. */
#وصف_ڪريو SPECIAL_پاڙ_MOUNT "-.mount"

/* Special slices valid for the user instance */
#وصف_ڪريو SPECIAL_SESSION_ٽڪرو "session.slice"
#وصف_ڪريو SPECIAL_APP_ٽڪرو "app.slice"
#وصف_ڪريو SPECIAL_BACKGROUND_ٽڪرو "background.slice"

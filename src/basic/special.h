/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#تعریف_کرو SPECIAL_DEFAULT_نشانہ "default.target"
#تعریف_کرو SPECIAL_INITRD_نشانہ "initrd.target"

/* Shutdown targets */
#تعریف_کرو SPECIAL_UMOUNT_نشانہ "umount.target"
/* This is not really intended to be started by directly. This is
 * mostly so that other targets (reboot/halt/poweroff) can depend on
 * it to bring all services down that want to be brought down on
 * system shutdown. */
#تعریف_کرو SPECIAL_SHUTDOWN_نشانہ "shutdown.target"
#تعریف_کرو SPECIAL_HALT_نشانہ "halt.target"
#تعریف_کرو SPECIAL_POWEROFF_نشانہ "poweroff.target"
#تعریف_کرو SPECIAL_REBOOT_نشانہ "reboot.target"
#تعریف_کرو SPECIAL_SOFT_REBOOT_نشانہ "soft-reboot.target"
#تعریف_کرو SPECIAL_KEXEC_نشانہ "kexec.target"
#تعریف_کرو SPECIAL_EXIT_نشانہ "exit.target"
#تعریف_کرو SPECIAL_SUSPEND_نشانہ "suspend.target"
#تعریف_کرو SPECIAL_HIBERNATE_نشانہ "hibernate.target"
#تعریف_کرو SPECIAL_HYBRID_SLEEP_نشانہ "hybrid-sleep.target"
#تعریف_کرو SPECIAL_SUSPEND_THEN_HIBERNATE_نشانہ "suspend-then-hibernate.target"
#تعریف_کرو SPECIAL_FACTORY_RESET_نشانہ "factory-reset.target"

/* Special boot targets */
#تعریف_کرو SPECIAL_RESCUE_نشانہ "rescue.target"
#تعریف_کرو SPECIAL_EMERGENCY_نشانہ "emergency.target"
#تعریف_کرو SPECIAL_MULTI_USER_نشانہ "multi-user.target"
#تعریف_کرو SPECIAL_GRAPHICAL_نشانہ "graphical.target"

/* Early boot targets */
#تعریف_کرو SPECIAL_SYSINIT_نشانہ "sysinit.target"
#تعریف_کرو SPECIAL_SOCKETS_نشانہ "sockets.target"
#تعریف_کرو SPECIAL_TIMERS_نشانہ "timers.target"
#تعریف_کرو SPECIAL_PATHS_نشانہ "paths.target"
#تعریف_کرو SPECIAL_LOCAL_FS_نشانہ "local-fs.target"
#تعریف_کرو SPECIAL_LOCAL_FS_PRE_نشانہ "local-fs-pre.target"
#تعریف_کرو SPECIAL_INITRD_FS_نشانہ "initrd-fs.target"
#تعریف_کرو SPECIAL_INITRD_جڑ_DEVICE_نشانہ "initrd-root-device.target"
#تعریف_کرو SPECIAL_INITRD_جڑ_FS_نشانہ "initrd-root-fs.target"
#تعریف_کرو SPECIAL_INITRD_USR_FS_نشانہ "initrd-usr-fs.target"
#تعریف_کرو SPECIAL_REMOTE_FS_نشانہ "remote-fs.target"       /* LSB's $remote_fs */
#تعریف_کرو SPECIAL_REMOTE_FS_PRE_نشانہ "remote-fs-pre.target"
#تعریف_کرو SPECIAL_وٹاندرا_نشانہ "swap.target"
#تعریف_کرو SPECIAL_جال_ONLINE_نشانہ "network-online.target"
#تعریف_کرو SPECIAL_ویلا_SYNC_نشانہ "time-sync.target"       /* LSB's $time */
#تعریف_کرو SPECIAL_ویلا_سیٹ_نشانہ "time-set.target"
#تعریف_کرو SPECIAL_BASIC_نشانہ "basic.target"
#تعریف_کرو SPECIAL_TPM2_نشانہ "tpm2.target"

/* LSB compatibility */
#تعریف_کرو SPECIAL_جال_نشانہ "network.target"           /* LSB's $network */
#تعریف_کرو SPECIAL_NSS_LOOKUP_نشانہ "nss-lookup.target"     /* LSB's $named */
#تعریف_کرو SPECIAL_RPCBIND_نشانہ "rpcbind.target"           /* LSB's $portmap */

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
#تعریف_کرو SPECIAL_KBREQUEST_نشانہ "kbrequest.target"
#تعریف_کرو SPECIAL_SIGPWR_نشانہ "sigpwr.target"
#تعریف_کرو SPECIAL_CTRL_ALT_DEL_نشانہ "ctrl-alt-del.target"

/* The unit to use as fallback if ctrl-alt-del.target (which is usually an alias) cannot be loaded. */
#تعریف_کرو FALLBACK_CTRL_ALT_DEL_نشانہ "reboot.target"

/* Where we add all our system units, users and machines by default */
#تعریف_کرو SPECIAL_SYSTEM_ٹکڑا "system.slice"
#تعریف_کرو SPECIAL_USER_ٹکڑا "user.slice"
#تعریف_کرو SPECIAL_MACHINE_ٹکڑا "machine.slice"
#تعریف_کرو SPECIAL_جڑ_ٹکڑا "-.slice"

/* The scope unit systemd itself lives in. */
#تعریف_کرو SPECIAL_INIT_SCOPE "init.scope"

/* The root directory. */
#تعریف_کرو SPECIAL_جڑ_MOUNT "-.mount"

/* Special slices valid for the user instance */
#تعریف_کرو SPECIAL_SESSION_ٹکڑا "session.slice"
#تعریف_کرو SPECIAL_APP_ٹکڑا "app.slice"
#تعریف_کرو SPECIAL_BACKGROUND_ٹکڑا "background.slice"

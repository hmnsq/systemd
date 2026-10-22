/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#నిర్వచించు SPECIAL_DEFAULT_లక్ష్యం "default.target"
#నిర్వచించు SPECIAL_INITRD_లక్ష్యం "initrd.target"

/* Shutdown targets */
#నిర్వచించు SPECIAL_UMOUNT_లక్ష్యం "umount.target"
/* This is not really intended to be started by directly. This is
 * mostly so that other targets (reboot/halt/poweroff) can depend on
 * it to bring all services down that want to be brought down on
 * system shutdown. */
#నిర్వచించు SPECIAL_SHUTDOWN_లక్ష్యం "shutdown.target"
#నిర్వచించు SPECIAL_HALT_లక్ష్యం "halt.target"
#నిర్వచించు SPECIAL_POWEROFF_లక్ష్యం "poweroff.target"
#నిర్వచించు SPECIAL_REBOOT_లక్ష్యం "reboot.target"
#నిర్వచించు SPECIAL_SOFT_REBOOT_లక్ష్యం "soft-reboot.target"
#నిర్వచించు SPECIAL_KEXEC_లక్ష్యం "kexec.target"
#నిర్వచించు SPECIAL_EXIT_లక్ష్యం "exit.target"
#నిర్వచించు SPECIAL_SUSPEND_లక్ష్యం "suspend.target"
#నిర్వచించు SPECIAL_HIBERNATE_లక్ష్యం "hibernate.target"
#నిర్వచించు SPECIAL_HYBRID_SLEEP_లక్ష్యం "hybrid-sleep.target"
#నిర్వచించు SPECIAL_SUSPEND_THEN_HIBERNATE_లక్ష్యం "suspend-then-hibernate.target"
#నిర్వచించు SPECIAL_FACTORY_RESET_లక్ష్యం "factory-reset.target"

/* Special boot targets */
#నిర్వచించు SPECIAL_RESCUE_లక్ష్యం "rescue.target"
#నిర్వచించు SPECIAL_EMERGENCY_లక్ష్యం "emergency.target"
#నిర్వచించు SPECIAL_MULTI_USER_లక్ష్యం "multi-user.target"
#నిర్వచించు SPECIAL_GRAPHICAL_లక్ష్యం "graphical.target"

/* Early boot targets */
#నిర్వచించు SPECIAL_SYSINIT_లక్ష్యం "sysinit.target"
#నిర్వచించు SPECIAL_SOCKETS_లక్ష్యం "sockets.target"
#నిర్వచించు SPECIAL_TIMERS_లక్ష్యం "timers.target"
#నిర్వచించు SPECIAL_PATHS_లక్ష్యం "paths.target"
#నిర్వచించు SPECIAL_LOCAL_FS_లక్ష్యం "local-fs.target"
#నిర్వచించు SPECIAL_LOCAL_FS_PRE_లక్ష్యం "local-fs-pre.target"
#నిర్వచించు SPECIAL_INITRD_FS_లక్ష్యం "initrd-fs.target"
#నిర్వచించు SPECIAL_INITRD_మూలం_DEVICE_లక్ష్యం "initrd-root-device.target"
#నిర్వచించు SPECIAL_INITRD_మూలం_FS_లక్ష్యం "initrd-root-fs.target"
#నిర్వచించు SPECIAL_INITRD_USR_FS_లక్ష్యం "initrd-usr-fs.target"
#నిర్వచించు SPECIAL_REMOTE_FS_లక్ష్యం "remote-fs.target"       /* LSB's $remote_fs */
#నిర్వచించు SPECIAL_REMOTE_FS_PRE_లక్ష్యం "remote-fs-pre.target"
#నిర్వచించు SPECIAL_మార్పిడి_లక్ష్యం "swap.target"
#నిర్వచించు SPECIAL_జాలం_ONLINE_లక్ష్యం "network-online.target"
#నిర్వచించు SPECIAL_సమయం_SYNC_లక్ష్యం "time-sync.target"       /* LSB's $time */
#నిర్వచించు SPECIAL_సమయం_సమితి_లక్ష్యం "time-set.target"
#నిర్వచించు SPECIAL_BASIC_లక్ష్యం "basic.target"
#నిర్వచించు SPECIAL_TPM2_లక్ష్యం "tpm2.target"

/* LSB compatibility */
#నిర్వచించు SPECIAL_జాలం_లక్ష్యం "network.target"           /* LSB's $network */
#నిర్వచించు SPECIAL_NSS_LOOKUP_లక్ష్యం "nss-lookup.target"     /* LSB's $named */
#నిర్వచించు SPECIAL_RPCBIND_లక్ష్యం "rpcbind.target"           /* LSB's $portmap */

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
#నిర్వచించు SPECIAL_FSCK_SERVICE "systemd-fsck@.service"
#నిర్వచించు SPECIAL_FSCK_మూలం_SERVICE "systemd-fsck-root.service"
#నిర్వచించు SPECIAL_FSCK_USR_SERVICE "systemd-fsck-usr.service"
#నిర్వచించు SPECIAL_QUOTACHECK_SERVICE "systemd-quotacheck@.service"
#నిర్వచించు SPECIAL_QUOTACHECK_మూలం_SERVICE "systemd-quotacheck-root.service"
#నిర్వచించు SPECIAL_QUOTAON_SERVICE "quotaon@.service"
#నిర్వచించు SPECIAL_QUOTAON_మూలం_SERVICE "quotaon-root.service"
#నిర్వచించు SPECIAL_REMOUNT_FS_SERVICE "systemd-remount-fs.service"
#నిర్వచించు SPECIAL_VOLATILE_మూలం_SERVICE "systemd-volatile-root.service"
#నిర్వచించు SPECIAL_UDEVD_SERVICE "systemd-udevd.service"
#నిర్వచించు SPECIAL_GROWFS_SERVICE "systemd-growfs@.service"
#నిర్వచించు SPECIAL_GROWFS_మూలం_SERVICE "systemd-growfs-root.service"
#నిర్వచించు SPECIAL_PCRFS_SERVICE "systemd-pcrfs@.service"
#నిర్వచించు SPECIAL_PCRFS_మూలం_SERVICE "systemd-pcrfs-root.service"
#నిర్వచించు SPECIAL_VALIDATEFS_SERVICE "systemd-validatefs@.service"
#నిర్వచించు SPECIAL_HIBERNATE_RESUME_SERVICE "systemd-hibernate-resume.service"

/* Services systemd relies on */
#నిర్వచించు SPECIAL_DBUS_SERVICE "dbus.service"
#నిర్వచించు SPECIAL_DBUS_SOCKET "dbus.socket"
#నిర్వచించు SPECIAL_JOURNALD_SOCKET "systemd-journald.socket"
#నిర్వచించు SPECIAL_JOURNALD_SERVICE "systemd-journald.service"
#నిర్వచించు SPECIAL_PTYBROKERD_SOCKET "systemd-ptybrokerd.socket"
#నిర్వచించు SPECIAL_TMPFILES_అమరిక_SERVICE "systemd-tmpfiles-setup.service"

/* Magic init signals */
#నిర్వచించు SPECIAL_KBREQUEST_లక్ష్యం "kbrequest.target"
#నిర్వచించు SPECIAL_SIGPWR_లక్ష్యం "sigpwr.target"
#నిర్వచించు SPECIAL_CTRL_ALT_DEL_లక్ష్యం "ctrl-alt-del.target"

/* The unit to use as fallback if ctrl-alt-del.target (which is usually an alias) cannot be loaded. */
#నిర్వచించు FALLBACK_CTRL_ALT_DEL_లక్ష్యం "reboot.target"

/* Where we add all our system units, users and machines by default */
#నిర్వచించు SPECIAL_SYSTEM_ముక్క "system.slice"
#నిర్వచించు SPECIAL_USER_ముక్క "user.slice"
#నిర్వచించు SPECIAL_MACHINE_ముక్క "machine.slice"
#నిర్వచించు SPECIAL_మూలం_ముక్క "-.slice"

/* The scope unit systemd itself lives in. */
#నిర్వచించు SPECIAL_INIT_SCOPE "init.scope"

/* The root directory. */
#నిర్వచించు SPECIAL_మూలం_MOUNT "-.mount"

/* Special slices valid for the user instance */
#నిర్వచించు SPECIAL_SESSION_ముక్క "session.slice"
#నిర్వచించు SPECIAL_APP_ముక్క "app.slice"
#నిర్వచించు SPECIAL_BACKGROUND_ముక్క "background.slice"

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#परिभाषय SPECIAL_DEFAULT_TARGET "default.target"
#परिभाषय SPECIAL_INITRD_TARGET "initrd.target"

/* Shutdown targets */
#परिभाषय SPECIAL_UMOUNT_TARGET "umount.target"
/* This is not really intended to be started by directly. This is
 * mostly so that other targets (reboot/halt/poweroff) can depend on
 * it to bring all services down that want to be brought down on
 * system shutdown. */
#परिभाषय SPECIAL_SHUTDOWN_TARGET "shutdown.target"
#परिभाषय SPECIAL_HALT_TARGET "halt.target"
#परिभाषय SPECIAL_POWEROFF_TARGET "poweroff.target"
#परिभाषय SPECIAL_REBOOT_TARGET "reboot.target"
#परिभाषय SPECIAL_SOFT_REBOOT_TARGET "soft-reboot.target"
#परिभाषय SPECIAL_KEXEC_TARGET "kexec.target"
#परिभाषय SPECIAL_EXIT_TARGET "exit.target"
#परिभाषय SPECIAL_SUSPEND_TARGET "suspend.target"
#परिभाषय SPECIAL_HIBERNATE_TARGET "hibernate.target"
#परिभाषय SPECIAL_HYBRID_SLEEP_TARGET "hybrid-sleep.target"
#परिभाषय SPECIAL_SUSPEND_THEN_HIBERNATE_TARGET "suspend-then-hibernate.target"
#परिभाषय SPECIAL_FACTORY_RESET_TARGET "factory-reset.target"

/* Special boot targets */
#परिभाषय SPECIAL_RESCUE_TARGET "rescue.target"
#परिभाषय SPECIAL_EMERGENCY_TARGET "emergency.target"
#परिभाषय SPECIAL_MULTI_USER_TARGET "multi-user.target"
#परिभाषय SPECIAL_GRAPHICAL_TARGET "graphical.target"

/* Early boot targets */
#परिभाषय SPECIAL_SYSINIT_TARGET "sysinit.target"
#परिभाषय SPECIAL_SOCKETS_TARGET "sockets.target"
#परिभाषय SPECIAL_TIMERS_TARGET "timers.target"
#परिभाषय SPECIAL_PATHS_TARGET "paths.target"
#परिभाषय SPECIAL_LOCAL_FS_TARGET "local-fs.target"
#परिभाषय SPECIAL_LOCAL_FS_PRE_TARGET "local-fs-pre.target"
#परिभाषय SPECIAL_INITRD_FS_TARGET "initrd-fs.target"
#परिभाषय SPECIAL_INITRD_मूलम्_DEVICE_TARGET "initrd-root-device.target"
#परिभाषय SPECIAL_INITRD_मूलम्_FS_TARGET "initrd-root-fs.target"
#परिभाषय SPECIAL_INITRD_USR_FS_TARGET "initrd-usr-fs.target"
#परिभाषय SPECIAL_REMOTE_FS_TARGET "remote-fs.target"       /* LSB's $remote_fs */
#परिभाषय SPECIAL_REMOTE_FS_PRE_TARGET "remote-fs-pre.target"
#परिभाषय SPECIAL_SWAP_TARGET "swap.target"
#परिभाषय SPECIAL_जालम्_ONLINE_TARGET "network-online.target"
#परिभाषय SPECIAL_TIME_SYNC_TARGET "time-sync.target"       /* LSB's $time */
#परिभाषय SPECIAL_TIME_समुच्चयः_TARGET "time-set.target"
#परिभाषय SPECIAL_BASIC_TARGET "basic.target"
#परिभाषय SPECIAL_TPM2_TARGET "tpm2.target"

/* LSB compatibility */
#परिभाषय SPECIAL_जालम्_TARGET "network.target"           /* LSB's $network */
#परिभाषय SPECIAL_NSS_LOOKUP_TARGET "nss-lookup.target"     /* LSB's $named */
#परिभाषय SPECIAL_RPCBIND_TARGET "rpcbind.target"           /* LSB's $portmap */

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
#परिभाषय SPECIAL_FSCK_SERVICE "systemd-fsck@.service"
#परिभाषय SPECIAL_FSCK_मूलम्_SERVICE "systemd-fsck-root.service"
#परिभाषय SPECIAL_FSCK_USR_SERVICE "systemd-fsck-usr.service"
#परिभाषय SPECIAL_QUOTACHECK_SERVICE "systemd-quotacheck@.service"
#परिभाषय SPECIAL_QUOTACHECK_मूलम्_SERVICE "systemd-quotacheck-root.service"
#परिभाषय SPECIAL_QUOTAON_SERVICE "quotaon@.service"
#परिभाषय SPECIAL_QUOTAON_मूलम्_SERVICE "quotaon-root.service"
#परिभाषय SPECIAL_REMOUNT_FS_SERVICE "systemd-remount-fs.service"
#परिभाषय SPECIAL_VOLATILE_मूलम्_SERVICE "systemd-volatile-root.service"
#परिभाषय SPECIAL_UDEVD_SERVICE "systemd-udevd.service"
#परिभाषय SPECIAL_GROWFS_SERVICE "systemd-growfs@.service"
#परिभाषय SPECIAL_GROWFS_मूलम्_SERVICE "systemd-growfs-root.service"
#परिभाषय SPECIAL_PCRFS_SERVICE "systemd-pcrfs@.service"
#परिभाषय SPECIAL_PCRFS_मूलम्_SERVICE "systemd-pcrfs-root.service"
#परिभाषय SPECIAL_VALIDATEFS_SERVICE "systemd-validatefs@.service"
#परिभाषय SPECIAL_HIBERNATE_RESUME_SERVICE "systemd-hibernate-resume.service"

/* Services systemd relies on */
#परिभाषय SPECIAL_DBUS_SERVICE "dbus.service"
#परिभाषय SPECIAL_DBUS_SOCKET "dbus.socket"
#परिभाषय SPECIAL_JOURNALD_SOCKET "systemd-journald.socket"
#परिभाषय SPECIAL_JOURNALD_SERVICE "systemd-journald.service"
#परिभाषय SPECIAL_PTYBROKERD_SOCKET "systemd-ptybrokerd.socket"
#परिभाषय SPECIAL_TMPFILES_सज्जता_SERVICE "systemd-tmpfiles-setup.service"

/* Magic init signals */
#परिभाषय SPECIAL_KBREQUEST_TARGET "kbrequest.target"
#परिभाषय SPECIAL_SIGPWR_TARGET "sigpwr.target"
#परिभाषय SPECIAL_CTRL_ALT_DEL_TARGET "ctrl-alt-del.target"

/* The unit to use as fallback if ctrl-alt-del.target (which is usually an alias) cannot be loaded. */
#परिभाषय FALLBACK_CTRL_ALT_DEL_TARGET "reboot.target"

/* Where we add all our system units, users and machines by default */
#परिभाषय SPECIAL_SYSTEM_खण्डिका "system.slice"
#परिभाषय SPECIAL_USER_खण्डिका "user.slice"
#परिभाषय SPECIAL_MACHINE_खण्डिका "machine.slice"
#परिभाषय SPECIAL_मूलम्_खण्डिका "-.slice"

/* The scope unit systemd itself lives in. */
#परिभाषय SPECIAL_INIT_SCOPE "init.scope"

/* The root directory. */
#परिभाषय SPECIAL_मूलम्_MOUNT "-.mount"

/* Special slices valid for the user instance */
#परिभाषय SPECIAL_SESSION_खण्डिका "session.slice"
#परिभाषय SPECIAL_APP_खण्डिका "app.slice"
#परिभाषय SPECIAL_BACKGROUND_खण्डिका "background.slice"

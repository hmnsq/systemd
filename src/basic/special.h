/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#घोषणा SPECIAL_DEFAULT_TARGET "default.target"
#घोषणा SPECIAL_INITRD_TARGET "initrd.target"

/* Shutdown targets */
#घोषणा SPECIAL_UMOUNT_TARGET "umount.target"
/* This is not really intended to be started by directly. This is
 * mostly so that other targets (reboot/halt/poweroff) can depend on
 * it to bring all services down that want to be brought down on
 * system shutdown. */
#घोषणा SPECIAL_SHUTDOWN_TARGET "shutdown.target"
#घोषणा SPECIAL_HALT_TARGET "halt.target"
#घोषणा SPECIAL_POWEROFF_TARGET "poweroff.target"
#घोषणा SPECIAL_REBOOT_TARGET "reboot.target"
#घोषणा SPECIAL_SOFT_REBOOT_TARGET "soft-reboot.target"
#घोषणा SPECIAL_KEXEC_TARGET "kexec.target"
#घोषणा SPECIAL_EXIT_TARGET "exit.target"
#घोषणा SPECIAL_SUSPEND_TARGET "suspend.target"
#घोषणा SPECIAL_HIBERNATE_TARGET "hibernate.target"
#घोषणा SPECIAL_HYBRID_SLEEP_TARGET "hybrid-sleep.target"
#घोषणा SPECIAL_SUSPEND_THEN_HIBERNATE_TARGET "suspend-then-hibernate.target"
#घोषणा SPECIAL_FACTORY_RESET_TARGET "factory-reset.target"

/* Special boot targets */
#घोषणा SPECIAL_RESCUE_TARGET "rescue.target"
#घोषणा SPECIAL_EMERGENCY_TARGET "emergency.target"
#घोषणा SPECIAL_MULTI_USER_TARGET "multi-user.target"
#घोषणा SPECIAL_GRAPHICAL_TARGET "graphical.target"

/* Early boot targets */
#घोषणा SPECIAL_SYSINIT_TARGET "sysinit.target"
#घोषणा SPECIAL_SOCKETS_TARGET "sockets.target"
#घोषणा SPECIAL_TIMERS_TARGET "timers.target"
#घोषणा SPECIAL_PATHS_TARGET "paths.target"
#घोषणा SPECIAL_LOCAL_FS_TARGET "local-fs.target"
#घोषणा SPECIAL_LOCAL_FS_PRE_TARGET "local-fs-pre.target"
#घोषणा SPECIAL_INITRD_FS_TARGET "initrd-fs.target"
#घोषणा SPECIAL_INITRD_मूल_DEVICE_TARGET "initrd-root-device.target"
#घोषणा SPECIAL_INITRD_मूल_FS_TARGET "initrd-root-fs.target"
#घोषणा SPECIAL_INITRD_USR_FS_TARGET "initrd-usr-fs.target"
#घोषणा SPECIAL_REMOTE_FS_TARGET "remote-fs.target"       /* LSB's $remote_fs */
#घोषणा SPECIAL_REMOTE_FS_PRE_TARGET "remote-fs-pre.target"
#घोषणा SPECIAL_SWAP_TARGET "swap.target"
#घोषणा SPECIAL_NETWORK_ONLINE_TARGET "network-online.target"
#घोषणा SPECIAL_TIME_SYNC_TARGET "time-sync.target"       /* LSB's $time */
#घोषणा SPECIAL_TIME_SET_TARGET "time-set.target"
#घोषणा SPECIAL_BASIC_TARGET "basic.target"
#घोषणा SPECIAL_TPM2_TARGET "tpm2.target"

/* LSB compatibility */
#घोषणा SPECIAL_NETWORK_TARGET "network.target"           /* LSB's $network */
#घोषणा SPECIAL_NSS_LOOKUP_TARGET "nss-lookup.target"     /* LSB's $named */
#घोषणा SPECIAL_RPCBIND_TARGET "rpcbind.target"           /* LSB's $portmap */

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
#घोषणा SPECIAL_FSCK_SERVICE "systemd-fsck@.service"
#घोषणा SPECIAL_FSCK_मूल_SERVICE "systemd-fsck-root.service"
#घोषणा SPECIAL_FSCK_USR_SERVICE "systemd-fsck-usr.service"
#घोषणा SPECIAL_QUOTACHECK_SERVICE "systemd-quotacheck@.service"
#घोषणा SPECIAL_QUOTACHECK_मूल_SERVICE "systemd-quotacheck-root.service"
#घोषणा SPECIAL_QUOTAON_SERVICE "quotaon@.service"
#घोषणा SPECIAL_QUOTAON_मूल_SERVICE "quotaon-root.service"
#घोषणा SPECIAL_REMOUNT_FS_SERVICE "systemd-remount-fs.service"
#घोषणा SPECIAL_VOLATILE_मूल_SERVICE "systemd-volatile-root.service"
#घोषणा SPECIAL_UDEVD_SERVICE "systemd-udevd.service"
#घोषणा SPECIAL_GROWFS_SERVICE "systemd-growfs@.service"
#घोषणा SPECIAL_GROWFS_मूल_SERVICE "systemd-growfs-root.service"
#घोषणा SPECIAL_PCRFS_SERVICE "systemd-pcrfs@.service"
#घोषणा SPECIAL_PCRFS_मूल_SERVICE "systemd-pcrfs-root.service"
#घोषणा SPECIAL_VALIDATEFS_SERVICE "systemd-validatefs@.service"
#घोषणा SPECIAL_HIBERNATE_RESUME_SERVICE "systemd-hibernate-resume.service"

/* Services systemd relies on */
#घोषणा SPECIAL_DBUS_SERVICE "dbus.service"
#घोषणा SPECIAL_DBUS_SOCKET "dbus.socket"
#घोषणा SPECIAL_JOURNALD_SOCKET "systemd-journald.socket"
#घोषणा SPECIAL_JOURNALD_SERVICE "systemd-journald.service"
#घोषणा SPECIAL_PTYBROKERD_SOCKET "systemd-ptybrokerd.socket"
#घोषणा SPECIAL_TMPFILES_SETUP_SERVICE "systemd-tmpfiles-setup.service"

/* Magic init signals */
#घोषणा SPECIAL_KBREQUEST_TARGET "kbrequest.target"
#घोषणा SPECIAL_SIGPWR_TARGET "sigpwr.target"
#घोषणा SPECIAL_CTRL_ALT_DEL_TARGET "ctrl-alt-del.target"

/* The unit to use as fallback if ctrl-alt-del.target (which is usually an alias) cannot be loaded. */
#घोषणा FALLBACK_CTRL_ALT_DEL_TARGET "reboot.target"

/* Where we add all our system units, users and machines by default */
#घोषणा SPECIAL_SYSTEM_SLICE "system.slice"
#घोषणा SPECIAL_USER_SLICE "user.slice"
#घोषणा SPECIAL_MACHINE_SLICE "machine.slice"
#घोषणा SPECIAL_मूल_SLICE "-.slice"

/* The scope unit systemd itself lives in. */
#घोषणा SPECIAL_INIT_SCOPE "init.scope"

/* The root directory. */
#घोषणा SPECIAL_मूल_MOUNT "-.mount"

/* Special slices valid for the user instance */
#घोषणा SPECIAL_SESSION_SLICE "session.slice"
#घोषणा SPECIAL_APP_SLICE "app.slice"
#घोषणा SPECIAL_BACKGROUND_SLICE "background.slice"

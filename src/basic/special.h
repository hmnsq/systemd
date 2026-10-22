/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_DEFAULT_ਨਿਸ਼ਾਨਾ "default.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_INITRD_ਨਿਸ਼ਾਨਾ "initrd.target"

/* Shutdown targets */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_UMOUNT_ਨਿਸ਼ਾਨਾ "umount.target"
/* This is not really intended to be started by directly. This is
 * mostly so that other targets (reboot/halt/poweroff) can depend on
 * it to bring all services down that want to be brought down on
 * system shutdown. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_SHUTDOWN_ਨਿਸ਼ਾਨਾ "shutdown.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_HALT_ਨਿਸ਼ਾਨਾ "halt.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_POWEROFF_ਨਿਸ਼ਾਨਾ "poweroff.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_REBOOT_ਨਿਸ਼ਾਨਾ "reboot.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_SOFT_REBOOT_ਨਿਸ਼ਾਨਾ "soft-reboot.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_KEXEC_ਨਿਸ਼ਾਨਾ "kexec.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_EXIT_ਨਿਸ਼ਾਨਾ "exit.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_SUSPEND_ਨਿਸ਼ਾਨਾ "suspend.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_HIBERNATE_ਨਿਸ਼ਾਨਾ "hibernate.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_HYBRID_SLEEP_ਨਿਸ਼ਾਨਾ "hybrid-sleep.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_SUSPEND_THEN_HIBERNATE_ਨਿਸ਼ਾਨਾ "suspend-then-hibernate.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_FACTORY_RESET_ਨਿਸ਼ਾਨਾ "factory-reset.target"

/* Special boot targets */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_RESCUE_ਨਿਸ਼ਾਨਾ "rescue.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_EMERGENCY_ਨਿਸ਼ਾਨਾ "emergency.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_MULTI_USER_ਨਿਸ਼ਾਨਾ "multi-user.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_GRAPHICAL_ਨਿਸ਼ਾਨਾ "graphical.target"

/* Early boot targets */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_SYSINIT_ਨਿਸ਼ਾਨਾ "sysinit.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_SOCKETS_ਨਿਸ਼ਾਨਾ "sockets.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_TIMERS_ਨਿਸ਼ਾਨਾ "timers.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_PATHS_ਨਿਸ਼ਾਨਾ "paths.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_LOCAL_FS_ਨਿਸ਼ਾਨਾ "local-fs.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_LOCAL_FS_PRE_ਨਿਸ਼ਾਨਾ "local-fs-pre.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_INITRD_FS_ਨਿਸ਼ਾਨਾ "initrd-fs.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_INITRD_ਜੜ੍ਹ_DEVICE_ਨਿਸ਼ਾਨਾ "initrd-root-device.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_INITRD_ਜੜ੍ਹ_FS_ਨਿਸ਼ਾਨਾ "initrd-root-fs.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_INITRD_USR_FS_ਨਿਸ਼ਾਨਾ "initrd-usr-fs.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_REMOTE_FS_ਨਿਸ਼ਾਨਾ "remote-fs.target"       /* LSB's $remote_fs */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_REMOTE_FS_PRE_ਨਿਸ਼ਾਨਾ "remote-fs-pre.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_ਅਦਲਾ_ਬਦਲੀ_ਨਿਸ਼ਾਨਾ "swap.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_ਜਾਲ_ONLINE_ਨਿਸ਼ਾਨਾ "network-online.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_ਸਮਾਂ_SYNC_ਨਿਸ਼ਾਨਾ "time-sync.target"       /* LSB's $time */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_ਸਮਾਂ_ਸੈੱਟ_ਨਿਸ਼ਾਨਾ "time-set.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_BASIC_ਨਿਸ਼ਾਨਾ "basic.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_TPM2_ਨਿਸ਼ਾਨਾ "tpm2.target"

/* LSB compatibility */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_ਜਾਲ_ਨਿਸ਼ਾਨਾ "network.target"           /* LSB's $network */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_NSS_LOOKUP_ਨਿਸ਼ਾਨਾ "nss-lookup.target"     /* LSB's $named */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_RPCBIND_ਨਿਸ਼ਾਨਾ "rpcbind.target"           /* LSB's $portmap */

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_FSCK_SERVICE "systemd-fsck@.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_FSCK_ਜੜ੍ਹ_SERVICE "systemd-fsck-root.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_FSCK_USR_SERVICE "systemd-fsck-usr.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_QUOTACHECK_SERVICE "systemd-quotacheck@.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_QUOTACHECK_ਜੜ੍ਹ_SERVICE "systemd-quotacheck-root.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_QUOTAON_SERVICE "quotaon@.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_QUOTAON_ਜੜ੍ਹ_SERVICE "quotaon-root.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_REMOUNT_FS_SERVICE "systemd-remount-fs.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_VOLATILE_ਜੜ੍ਹ_SERVICE "systemd-volatile-root.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_UDEVD_SERVICE "systemd-udevd.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_GROWFS_SERVICE "systemd-growfs@.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_GROWFS_ਜੜ੍ਹ_SERVICE "systemd-growfs-root.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_PCRFS_SERVICE "systemd-pcrfs@.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_PCRFS_ਜੜ੍ਹ_SERVICE "systemd-pcrfs-root.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_VALIDATEFS_SERVICE "systemd-validatefs@.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_HIBERNATE_RESUME_SERVICE "systemd-hibernate-resume.service"

/* Services systemd relies on */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_DBUS_SERVICE "dbus.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_DBUS_SOCKET "dbus.socket"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_JOURNALD_SOCKET "systemd-journald.socket"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_JOURNALD_SERVICE "systemd-journald.service"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_PTYBROKERD_SOCKET "systemd-ptybrokerd.socket"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_TMPFILES_ਤਿਆਰੀ_SERVICE "systemd-tmpfiles-setup.service"

/* Magic init signals */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_KBREQUEST_ਨਿਸ਼ਾਨਾ "kbrequest.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_SIGPWR_ਨਿਸ਼ਾਨਾ "sigpwr.target"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_CTRL_ALT_DEL_ਨਿਸ਼ਾਨਾ "ctrl-alt-del.target"

/* The unit to use as fallback if ctrl-alt-del.target (which is usually an alias) cannot be loaded. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FALLBACK_CTRL_ALT_DEL_ਨਿਸ਼ਾਨਾ "reboot.target"

/* Where we add all our system units, users and machines by default */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_SYSTEM_ਟੁਕੜਾ "system.slice"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_USER_ਟੁਕੜਾ "user.slice"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_MACHINE_ਟੁਕੜਾ "machine.slice"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_ਜੜ੍ਹ_ਟੁਕੜਾ "-.slice"

/* The scope unit systemd itself lives in. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_INIT_SCOPE "init.scope"

/* The root directory. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_ਜੜ੍ਹ_MOUNT "-.mount"

/* Special slices valid for the user instance */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_SESSION_ਟੁਕੜਾ "session.slice"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_APP_ਟੁਕੜਾ "app.slice"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_BACKGROUND_ਟੁਕੜਾ "background.slice"

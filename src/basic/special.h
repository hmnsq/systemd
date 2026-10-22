/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#تعریف_کړه SPECIAL_DEFAULT_موخه "default.target"
#تعریف_کړه SPECIAL_INITRD_موخه "initrd.target"

/* Shutdown targets */
#تعریف_کړه SPECIAL_UMOUNT_موخه "umount.target"
/* This is not really intended to be started by directly. This is
 * mostly so that other targets (reboot/halt/poweroff) can depend on
 * it to bring all services down that want to be brought down on
 * system shutdown. */
#تعریف_کړه SPECIAL_SHUTDOWN_موخه "shutdown.target"
#تعریف_کړه SPECIAL_HALT_موخه "halt.target"
#تعریف_کړه SPECIAL_POWEROFF_موخه "poweroff.target"
#تعریف_کړه SPECIAL_REBOOT_موخه "reboot.target"
#تعریف_کړه SPECIAL_SOFT_REBOOT_موخه "soft-reboot.target"
#تعریف_کړه SPECIAL_KEXEC_موخه "kexec.target"
#تعریف_کړه SPECIAL_EXIT_موخه "exit.target"
#تعریف_کړه SPECIAL_SUSPEND_موخه "suspend.target"
#تعریف_کړه SPECIAL_HIBERNATE_موخه "hibernate.target"
#تعریف_کړه SPECIAL_HYBRID_SLEEP_موخه "hybrid-sleep.target"
#تعریف_کړه SPECIAL_SUSPEND_THEN_HIBERNATE_موخه "suspend-then-hibernate.target"
#تعریف_کړه SPECIAL_FACTORY_RESET_موخه "factory-reset.target"

/* Special boot targets */
#تعریف_کړه SPECIAL_RESCUE_موخه "rescue.target"
#تعریف_کړه SPECIAL_EMERGENCY_موخه "emergency.target"
#تعریف_کړه SPECIAL_MULTI_USER_موخه "multi-user.target"
#تعریف_کړه SPECIAL_GRAPHICAL_موخه "graphical.target"

/* Early boot targets */
#تعریف_کړه SPECIAL_SYSINIT_موخه "sysinit.target"
#تعریف_کړه SPECIAL_SOCKETS_موخه "sockets.target"
#تعریف_کړه SPECIAL_TIMERS_موخه "timers.target"
#تعریف_کړه SPECIAL_PATHS_موخه "paths.target"
#تعریف_کړه SPECIAL_LOCAL_FS_موخه "local-fs.target"
#تعریف_کړه SPECIAL_LOCAL_FS_PRE_موخه "local-fs-pre.target"
#تعریف_کړه SPECIAL_INITRD_FS_موخه "initrd-fs.target"
#تعریف_کړه SPECIAL_INITRD_ریښه_DEVICE_موخه "initrd-root-device.target"
#تعریف_کړه SPECIAL_INITRD_ریښه_FS_موخه "initrd-root-fs.target"
#تعریف_کړه SPECIAL_INITRD_USR_FS_موخه "initrd-usr-fs.target"
#تعریف_کړه SPECIAL_REMOTE_FS_موخه "remote-fs.target"       /* LSB's $remote_fs */
#تعریف_کړه SPECIAL_REMOTE_FS_PRE_موخه "remote-fs-pre.target"
#تعریف_کړه SPECIAL_بدلون_موخه "swap.target"
#تعریف_کړه SPECIAL_شبکه_ONLINE_موخه "network-online.target"
#تعریف_کړه SPECIAL_وخت_SYNC_موخه "time-sync.target"       /* LSB's $time */
#تعریف_کړه SPECIAL_وخت_سېټ_موخه "time-set.target"
#تعریف_کړه SPECIAL_BASIC_موخه "basic.target"
#تعریف_کړه SPECIAL_TPM2_موخه "tpm2.target"

/* LSB compatibility */
#تعریف_کړه SPECIAL_شبکه_موخه "network.target"           /* LSB's $network */
#تعریف_کړه SPECIAL_NSS_LOOKUP_موخه "nss-lookup.target"     /* LSB's $named */
#تعریف_کړه SPECIAL_RPCBIND_موخه "rpcbind.target"           /* LSB's $portmap */

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
#تعریف_کړه SPECIAL_FSCK_SERVICE "systemd-fsck@.service"
#تعریف_کړه SPECIAL_FSCK_ریښه_SERVICE "systemd-fsck-root.service"
#تعریف_کړه SPECIAL_FSCK_USR_SERVICE "systemd-fsck-usr.service"
#تعریف_کړه SPECIAL_QUOTACHECK_SERVICE "systemd-quotacheck@.service"
#تعریف_کړه SPECIAL_QUOTACHECK_ریښه_SERVICE "systemd-quotacheck-root.service"
#تعریف_کړه SPECIAL_QUOTAON_SERVICE "quotaon@.service"
#تعریف_کړه SPECIAL_QUOTAON_ریښه_SERVICE "quotaon-root.service"
#تعریف_کړه SPECIAL_REMOUNT_FS_SERVICE "systemd-remount-fs.service"
#تعریف_کړه SPECIAL_VOLATILE_ریښه_SERVICE "systemd-volatile-root.service"
#تعریف_کړه SPECIAL_UDEVD_SERVICE "systemd-udevd.service"
#تعریف_کړه SPECIAL_GROWFS_SERVICE "systemd-growfs@.service"
#تعریف_کړه SPECIAL_GROWFS_ریښه_SERVICE "systemd-growfs-root.service"
#تعریف_کړه SPECIAL_PCRFS_SERVICE "systemd-pcrfs@.service"
#تعریف_کړه SPECIAL_PCRFS_ریښه_SERVICE "systemd-pcrfs-root.service"
#تعریف_کړه SPECIAL_VALIDATEFS_SERVICE "systemd-validatefs@.service"
#تعریف_کړه SPECIAL_HIBERNATE_RESUME_SERVICE "systemd-hibernate-resume.service"

/* Services systemd relies on */
#تعریف_کړه SPECIAL_DBUS_SERVICE "dbus.service"
#تعریف_کړه SPECIAL_DBUS_SOCKET "dbus.socket"
#تعریف_کړه SPECIAL_JOURNALD_SOCKET "systemd-journald.socket"
#تعریف_کړه SPECIAL_JOURNALD_SERVICE "systemd-journald.service"
#تعریف_کړه SPECIAL_PTYBROKERD_SOCKET "systemd-ptybrokerd.socket"
#تعریف_کړه SPECIAL_TMPFILES_تیاری_SERVICE "systemd-tmpfiles-setup.service"

/* Magic init signals */
#تعریف_کړه SPECIAL_KBREQUEST_موخه "kbrequest.target"
#تعریف_کړه SPECIAL_SIGPWR_موخه "sigpwr.target"
#تعریف_کړه SPECIAL_CTRL_ALT_DEL_موخه "ctrl-alt-del.target"

/* The unit to use as fallback if ctrl-alt-del.target (which is usually an alias) cannot be loaded. */
#تعریف_کړه FALLBACK_CTRL_ALT_DEL_موخه "reboot.target"

/* Where we add all our system units, users and machines by default */
#تعریف_کړه SPECIAL_SYSTEM_ټوټه "system.slice"
#تعریف_کړه SPECIAL_USER_ټوټه "user.slice"
#تعریف_کړه SPECIAL_MACHINE_ټوټه "machine.slice"
#تعریف_کړه SPECIAL_ریښه_ټوټه "-.slice"

/* The scope unit systemd itself lives in. */
#تعریف_کړه SPECIAL_INIT_SCOPE "init.scope"

/* The root directory. */
#تعریف_کړه SPECIAL_ریښه_MOUNT "-.mount"

/* Special slices valid for the user instance */
#تعریف_کړه SPECIAL_SESSION_ټوټه "session.slice"
#تعریف_کړه SPECIAL_APP_ټوټه "app.slice"
#تعریف_کړه SPECIAL_BACKGROUND_ټوټه "background.slice"

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

#define SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
تعريفنوع تعداد CGroupController {
        /* Original cgroup controllers */
        CGROUP_CONTROLLER_CPU,
        CGROUP_CONTROLLER_CPUACCT,    /* v1 only */
        CGROUP_CONTROLLER_CPUSET,     /* v2 only */
        CGROUP_CONTROLLER_IO,         /* v2 only */
        CGROUP_CONTROLLER_BLKIO,      /* v1 only */
        CGROUP_CONTROLLER_MEMORY,
        CGROUP_CONTROLLER_DEVICES,    /* v1 only */
        CGROUP_CONTROLLER_PIDS,

        /* BPF-based pseudo-controllers, v2 only */
        CGROUP_CONTROLLER_BPF_FIREWALL,
        CGROUP_CONTROLLER_BPF_DEVICES,
        CGROUP_CONTROLLER_BPF_FOREIGN,
        CGROUP_CONTROLLER_BPF_SOCKET_BIND,
        CGROUP_CONTROLLER_BPF_RESTRICT_NETWORK_INTERFACES,
        CGROUP_CONTROLLER_BPF_BIND_NETWORK_INTERFACE,
        /* The BPF hook implementing RestrictFileSystems= is not defined here.
         * It's applied as late as possible in exec_invoke() so we don't block
         * our own unit setup code. */

        _CGROUP_CONTROLLER_MAX,
        _CGROUP_CONTROLLER_INVALID = -EINVAL,
} CGroupController;

#define CGROUP_CONTROLLER_TO_MASK(c) (1U << (c))

/* A bit mask of well known cgroup controllers */
تعريفنوع تعداد CGroupMask {
        CGROUP_MASK_CPU = CGROUP_CONTROLLER_TO_MASK(CGROUP_CONTROLLER_CPU),
        CGROUP_MASK_CPUACCT = CGROUP_CONTROLLER_TO_MASK(CGROUP_CONTROLLER_CPUACCT),
        CGROUP_MASK_CPUSET = CGROUP_CONTROLLER_TO_MASK(CGROUP_CONTROLLER_CPUSET),
        CGROUP_MASK_IO = CGROUP_CONTROLLER_TO_MASK(CGROUP_CONTROLLER_IO),
        CGROUP_MASK_BLKIO = CGROUP_CONTROLLER_TO_MASK(CGROUP_CONTROLLER_BLKIO),
        CGROUP_MASK_MEMORY = CGROUP_CONTROLLER_TO_MASK(CGROUP_CONTROLLER_MEMORY),
        CGROUP_MASK_DEVICES = CGROUP_CONTROLLER_TO_MASK(CGROUP_CONTROLLER_DEVICES),
        CGROUP_MASK_PIDS = CGROUP_CONTROLLER_TO_MASK(CGROUP_CONTROLLER_PIDS),
        CGROUP_MASK_BPF_FIREWALL = CGROUP_CONTROLLER_TO_MASK(CGROUP_CONTROLLER_BPF_FIREWALL),
        CGROUP_MASK_BPF_DEVICES = CGROUP_CONTROLLER_TO_MASK(CGROUP_CONTROLLER_BPF_DEVICES),
        CGROUP_MASK_BPF_FOREIGN = CGROUP_CONTROLLER_TO_MASK(CGROUP_CONTROLLER_BPF_FOREIGN),
        CGROUP_MASK_BPF_SOCKET_BIND = CGROUP_CONTROLLER_TO_MASK(CGROUP_CONTROLLER_BPF_SOCKET_BIND),
        CGROUP_MASK_BPF_RESTRICT_NETWORK_INTERFACES = CGROUP_CONTROLLER_TO_MASK(CGROUP_CONTROLLER_BPF_RESTRICT_NETWORK_INTERFACES),
        CGROUP_MASK_BPF_BIND_NETWORK_INTERFACE = CGROUP_CONTROLLER_TO_MASK(CGROUP_CONTROLLER_BPF_BIND_NETWORK_INTERFACE),

        /* All real cgroup v1 controllers */
        CGROUP_MASK_V1 = CGROUP_MASK_CPU|CGROUP_MASK_CPUACCT|CGROUP_MASK_BLKIO|CGROUP_MASK_MEMORY|CGROUP_MASK_DEVICES|CGROUP_MASK_PIDS,

        /* All real cgroup v2 controllers */
        CGROUP_MASK_V2 = CGROUP_MASK_CPU|CGROUP_MASK_CPUSET|CGROUP_MASK_IO|CGROUP_MASK_MEMORY|CGROUP_MASK_PIDS,

        /* All controllers we want to delegate in case of Delegate=yes. Which are pretty much the v2 controllers only, as delegation on v1 is not safe, and bpf stuff isn't a real controller */
        CGROUP_MASK_DELEGATE = CGROUP_MASK_V2,

        /* All cgroup v2 BPF pseudo-controllers */
        CGROUP_MASK_BPF = CGROUP_MASK_BPF_FIREWALL|CGROUP_MASK_BPF_DEVICES|CGROUP_MASK_BPF_FOREIGN|CGROUP_MASK_BPF_SOCKET_BIND|CGROUP_MASK_BPF_RESTRICT_NETWORK_INTERFACES|CGROUP_MASK_BPF_BIND_NETWORK_INTERFACE,

        _CGROUP_MASK_ALL = CGROUP_CONTROLLER_TO_MASK(_CGROUP_CONTROLLER_MAX) - 1,
} CGroupMask;

/* Special values for all weight knobs on unified hierarchy */
#define CGROUP_WEIGHT_INVALID UINT64_MAX
#define CGROUP_WEIGHT_IDLE UINT64_C(0)
#define CGROUP_WEIGHT_MIN UINT64_C(1)
#define CGROUP_WEIGHT_MAX UINT64_C(10000)
#define CGROUP_WEIGHT_DEFAULT UINT64_C(100)

#define CGROUP_LIMIT_MIN UINT64_C(0)
#define CGROUP_LIMIT_MAX UINT64_MAX

ساكن مضمن bool CGROUP_WEIGHT_IS_OK(uint64_t x) {
        أرجع
            x == CGROUP_WEIGHT_INVALID ||
            (x >= CGROUP_WEIGHT_MIN && x <= CGROUP_WEIGHT_MAX);
}

/* IO limits on unified hierarchy */
تعريفنوع تعداد CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        _CGROUP_IO_LIMIT_TYPE_MAX,
        _CGROUP_IO_LIMIT_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

خارجي ثابت uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX];

DECLARE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);
فراغ cgroup_io_limits_list(فراغ);

/* Special values for the io.bfq.weight attribute */
#define CGROUP_BFQ_WEIGHT_INVALID UINT64_MAX
#define CGROUP_BFQ_WEIGHT_MIN UINT64_C(1)
#define CGROUP_BFQ_WEIGHT_MAX UINT64_C(1000)
#define CGROUP_BFQ_WEIGHT_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
ساكن مضمن uint64_t BFQ_WEIGHT(uint64_t io_weight) {
        أرجع
            io_weight <= CGROUP_WEIGHT_DEFAULT ?
            CGROUP_BFQ_WEIGHT_DEFAULT - (CGROUP_WEIGHT_DEFAULT - io_weight) * (CGROUP_BFQ_WEIGHT_DEFAULT - CGROUP_BFQ_WEIGHT_MIN) / (CGROUP_WEIGHT_DEFAULT - CGROUP_WEIGHT_MIN) :
            CGROUP_BFQ_WEIGHT_DEFAULT + (io_weight - CGROUP_WEIGHT_DEFAULT) * (CGROUP_BFQ_WEIGHT_MAX - CGROUP_BFQ_WEIGHT_DEFAULT) / (CGROUP_WEIGHT_MAX - CGROUP_WEIGHT_DEFAULT);
}

/*
 * General rules:
 *
 * We require absolute cgroup paths. When returning, we will always
 * generate paths with multiple adjacent / removed.
 */

صحيح cg_is_available(فراغ);

صحيح cg_path_open(ثابت حرف *path);
صحيح cg_cgroupid_open(صحيح cgroupfs_fd, uint64_t id);

صحيح cg_path_from_cgroupid(صحيح cgroupfs_fd, uint64_t id, حرف **ret);

تعريفنوع تعداد CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

صحيح cg_enumerate_processes(ثابت حرف *path, FILE **ret);
صحيح cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
صحيح cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

صحيح cg_enumerate_subgroups(ثابت حرف *path, DIR **ret);
صحيح cg_read_subgroup(DIR *d, حرف **ret);

تعريفنوع صحيح (*cg_kill_log_func_t)(ثابت PidRef *pid, صحيح sig, فراغ *userdata);

صحيح cg_kill(ثابت حرف *path, صحيح sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, فراغ *userdata);
صحيح cg_kill_kernel_sigkill(ثابت حرف *path, uint64_t *ret_n_pids_killed);
صحيح cg_kill_recursive(ثابت حرف *path, صحيح sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, فراغ *userdata);

صحيح cg_split_spec(ثابت حرف *spec, حرف **ret_controller, حرف **ret_path);

صحيح cg_get_path(ثابت حرف *path, ثابت حرف *suffix, حرف **ret);

صحيح cg_pid_get_path(pid_t pid, حرف **ret);
صحيح cg_pidref_get_path(ثابت PidRef *pidref, حرف **ret);

صحيح cg_is_threaded(ثابت حرف *path);

صحيح cg_is_delegated(ثابت حرف *path);
صحيح cg_is_delegated_fd(صحيح fd);

صحيح cg_has_coredump_receive(ثابت حرف *path);

صحيح cg_set_attribute(ثابت حرف *path, ثابت حرف *attribute, ثابت حرف *قيمة);
صحيح cg_get_attribute(ثابت حرف *path, ثابت حرف *attribute, حرف **ret);
صحيح cg_get_attribute_as_uint64(ثابت حرف *path, ثابت حرف *attribute, uint64_t *ret);
صحيح cg_get_attribute_as_bool(ثابت حرف *path, ثابت حرف *attribute);

صحيح cg_get_keyed_attribute(ثابت حرف *path, ثابت حرف *attribute, حرف * ثابت *مفاتيح, حرف **values);
صحيح cg_get_keyed_attribute_uint64(ثابت حرف *path, ثابت حرف *attribute, ثابت حرف *مفتاح, uint64_t *ret);

صحيح cg_get_owner(ثابت حرف *path, uid_t *ret_uid);

صحيح cg_set_xattr(ثابت حرف *path, ثابت حرف *name, ثابت فراغ *قيمة, size_t size, صحيح flags);
صحيح cg_get_xattr(ثابت حرف *path, ثابت حرف *name, حرف **ret, size_t *ret_size);
/* Returns negative on error, and 0 or 1 on success for the bool value */
صحيح cg_get_xattr_bool(ثابت حرف *path, ثابت حرف *name);
صحيح cg_remove_xattr(ثابت حرف *path, ثابت حرف *name);

صحيح cg_is_empty(ثابت حرف *path);

صحيح cg_get_جذر_path(حرف **path);

صحيح cg_path_get_session(ثابت حرف *path, حرف **ret_session);
صحيح cg_path_get_owner_uid(ثابت حرف *path, uid_t *ret_uid);
صحيح cg_path_get_unit_full(ثابت حرف *path, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_path_get_unit(ثابت حرف *path, حرف **ret_unit) {
        أرجع cg_path_get_unit_full(path, ret_unit, عدم);
}
صحيح cg_path_get_unit_path(ثابت حرف *path, حرف **ret_unit);
صحيح cg_path_get_user_unit_full(ثابت حرف *path, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_path_get_user_unit(ثابت حرف *path, حرف **ret_unit) {
        أرجع cg_path_get_user_unit_full(path, ret_unit, عدم);
}
صحيح cg_path_get_machine_name(ثابت حرف *path, حرف **ret_machine);
صحيح cg_path_get_slice(ثابت حرف *path, حرف **ret_slice);
صحيح cg_path_get_user_slice(ثابت حرف *path, حرف **ret_slice);

صحيح cg_shift_path(ثابت حرف *cgroup, ثابت حرف *cached_جذر, ثابت حرف **ret_shifted);
صحيح cg_pid_get_path_shifted(pid_t pid, ثابت حرف *cached_جذر, حرف **ret_cgroup);
صحيح cg_pidref_get_path_shifted(ثابت PidRef *pidref, ثابت حرف *cached_جذر, حرف **ret_cgroup);

صحيح cg_pid_get_session(pid_t pid, حرف **ret_session);
صحيح cg_pidref_get_session(ثابت PidRef *pidref, حرف **ret);
صحيح cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
صحيح cg_pidref_get_owner_uid(ثابت PidRef *pidref, uid_t *ret);
صحيح cg_pid_get_unit_full(pid_t pid, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pid_get_unit(pid_t pid, حرف **ret_unit) {
        أرجع cg_pid_get_unit_full(pid, ret_unit, عدم);
}
صحيح cg_pidref_get_unit_full(ثابت PidRef *pidref, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pidref_get_unit(ثابت PidRef *pidref, حرف **ret_unit) {
        أرجع cg_pidref_get_unit_full(pidref, ret_unit, عدم);
}
صحيح cg_pid_get_user_unit_full(pid_t pid, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pid_get_user_unit(pid_t pid, حرف **ret_unit) {
        أرجع cg_pid_get_user_unit_full(pid, ret_unit, عدم);
}
صحيح cg_pidref_get_user_unit_full(ثابت PidRef *pidref, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pidref_get_user_unit(ثابت PidRef *pidref, حرف **ret_unit) {
        أرجع cg_pidref_get_user_unit_full(pidref, ret_unit, عدم);
}
صحيح cg_pid_get_machine_name(pid_t pid, حرف **ret_machine);
صحيح cg_pid_get_slice(pid_t pid, حرف **ret_slice);
صحيح cg_pid_get_user_slice(pid_t pid, حرف **ret_slice);

صحيح cg_path_decode_unit(ثابت حرف *cgroup, حرف **ret_unit);

bool cg_needs_escape(ثابت حرف *p) _pure_;
صحيح cg_escape(ثابت حرف *p, حرف **ret);
حرف* cg_unescape(ثابت حرف *p) _pure_;

صحيح cg_slice_to_path(ثابت حرف *unit, حرف **ret);

صحيح cg_mask_supported(CGroupMask *ret);
صحيح cg_mask_supported_subtree(ثابت حرف *جذر, CGroupMask *ret);
صحيح cg_mask_from_string(ثابت حرف *s, CGroupMask *ret);
صحيح cg_mask_to_string(CGroupMask mask, حرف **ret);

bool cg_kill_supported(فراغ);

DECLARE_STRING_TABLE_LOOKUP(cgroup_controller, CGroupController);

تعريفنوع تعداد ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        _MANAGED_OOM_MODE_MAX,
        _MANAGED_OOM_MODE_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

تعريفنوع تعداد ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);

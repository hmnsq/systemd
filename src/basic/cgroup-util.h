/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

#define SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
تعریفنوع شمارش CGroupController {
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
تعریفنوع شمارش CGroupMask {
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

ایستا درونخطی bool CGROUP_WEIGHT_IS_OK(uint64_t x) {
        بازگشت
            x == CGROUP_WEIGHT_INVALID ||
            (x >= CGROUP_WEIGHT_MIN && x <= CGROUP_WEIGHT_MAX);
}

/* IO limits on unified hierarchy */
تعریفنوع شمارش CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        _CGROUP_IO_LIMIT_TYPE_MAX,
        _CGROUP_IO_LIMIT_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

بیرونی ثابت uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX];

DECLARE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);
تهی cgroup_io_limits_list(تهی);

/* Special values for the io.bfq.weight attribute */
#define CGROUP_BFQ_WEIGHT_INVALID UINT64_MAX
#define CGROUP_BFQ_WEIGHT_MIN UINT64_C(1)
#define CGROUP_BFQ_WEIGHT_MAX UINT64_C(1000)
#define CGROUP_BFQ_WEIGHT_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
ایستا درونخطی uint64_t BFQ_WEIGHT(uint64_t io_weight) {
        بازگشت
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

صحیح cg_is_available(تهی);

صحیح cg_path_open(ثابت نویسه *path);
صحیح cg_cgroupid_open(صحیح cgroupfs_fd, uint64_t id);

صحیح cg_path_from_cgroupid(صحیح cgroupfs_fd, uint64_t id, نویسه **ret);

تعریفنوع شمارش CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

صحیح cg_enumerate_processes(ثابت نویسه *path, FILE **ret);
صحیح cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
صحیح cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

صحیح cg_enumerate_subgroups(ثابت نویسه *path, DIR **ret);
صحیح cg_read_subgroup(DIR *d, نویسه **ret);

تعریفنوع صحیح (*cg_kill_log_func_t)(ثابت PidRef *pid, صحیح sig, تهی *userdata);

صحیح cg_kill(ثابت نویسه *path, صحیح sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, تهی *userdata);
صحیح cg_kill_kernel_sigkill(ثابت نویسه *path, uint64_t *ret_n_pids_killed);
صحیح cg_kill_recursive(ثابت نویسه *path, صحیح sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, تهی *userdata);

صحیح cg_split_spec(ثابت نویسه *spec, نویسه **ret_controller, نویسه **ret_path);

صحیح cg_get_path(ثابت نویسه *path, ثابت نویسه *suffix, نویسه **ret);

صحیح cg_pid_get_path(pid_t pid, نویسه **ret);
صحیح cg_pidref_get_path(ثابت PidRef *pidref, نویسه **ret);

صحیح cg_is_threaded(ثابت نویسه *path);

صحیح cg_is_delegated(ثابت نویسه *path);
صحیح cg_is_delegated_fd(صحیح fd);

صحیح cg_has_coredump_receive(ثابت نویسه *path);

صحیح cg_set_attribute(ثابت نویسه *path, ثابت نویسه *attribute, ثابت نویسه *مقدار);
صحیح cg_get_attribute(ثابت نویسه *path, ثابت نویسه *attribute, نویسه **ret);
صحیح cg_get_attribute_as_uint64(ثابت نویسه *path, ثابت نویسه *attribute, uint64_t *ret);
صحیح cg_get_attribute_as_bool(ثابت نویسه *path, ثابت نویسه *attribute);

صحیح cg_get_keyed_attribute(ثابت نویسه *path, ثابت نویسه *attribute, نویسه * ثابت *کلیدها, نویسه **values);
صحیح cg_get_keyed_attribute_uint64(ثابت نویسه *path, ثابت نویسه *attribute, ثابت نویسه *کلید, uint64_t *ret);

صحیح cg_get_owner(ثابت نویسه *path, uid_t *ret_uid);

صحیح cg_set_xattr(ثابت نویسه *path, ثابت نویسه *name, ثابت تهی *مقدار, size_t size, صحیح flags);
صحیح cg_get_xattr(ثابت نویسه *path, ثابت نویسه *name, نویسه **ret, size_t *ret_size);
/* Returns negative on error, and 0 or 1 on success for the bool value */
صحیح cg_get_xattr_bool(ثابت نویسه *path, ثابت نویسه *name);
صحیح cg_remove_xattr(ثابت نویسه *path, ثابت نویسه *name);

صحیح cg_is_empty(ثابت نویسه *path);

صحیح cg_get_ریشه_path(نویسه **path);

صحیح cg_path_get_session(ثابت نویسه *path, نویسه **ret_session);
صحیح cg_path_get_owner_uid(ثابت نویسه *path, uid_t *ret_uid);
صحیح cg_path_get_unit_full(ثابت نویسه *path, نویسه **ret_unit, نویسه **ret_subgroup);
ایستا درونخطی صحیح cg_path_get_unit(ثابت نویسه *path, نویسه **ret_unit) {
        بازگشت cg_path_get_unit_full(path, ret_unit, پوچ);
}
صحیح cg_path_get_unit_path(ثابت نویسه *path, نویسه **ret_unit);
صحیح cg_path_get_user_unit_full(ثابت نویسه *path, نویسه **ret_unit, نویسه **ret_subgroup);
ایستا درونخطی صحیح cg_path_get_user_unit(ثابت نویسه *path, نویسه **ret_unit) {
        بازگشت cg_path_get_user_unit_full(path, ret_unit, پوچ);
}
صحیح cg_path_get_machine_name(ثابت نویسه *path, نویسه **ret_machine);
صحیح cg_path_get_slice(ثابت نویسه *path, نویسه **ret_slice);
صحیح cg_path_get_user_slice(ثابت نویسه *path, نویسه **ret_slice);

صحیح cg_shift_path(ثابت نویسه *cgroup, ثابت نویسه *cached_ریشه, ثابت نویسه **ret_shifted);
صحیح cg_pid_get_path_shifted(pid_t pid, ثابت نویسه *cached_ریشه, نویسه **ret_cgroup);
صحیح cg_pidref_get_path_shifted(ثابت PidRef *pidref, ثابت نویسه *cached_ریشه, نویسه **ret_cgroup);

صحیح cg_pid_get_session(pid_t pid, نویسه **ret_session);
صحیح cg_pidref_get_session(ثابت PidRef *pidref, نویسه **ret);
صحیح cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
صحیح cg_pidref_get_owner_uid(ثابت PidRef *pidref, uid_t *ret);
صحیح cg_pid_get_unit_full(pid_t pid, نویسه **ret_unit, نویسه **ret_subgroup);
ایستا درونخطی صحیح cg_pid_get_unit(pid_t pid, نویسه **ret_unit) {
        بازگشت cg_pid_get_unit_full(pid, ret_unit, پوچ);
}
صحیح cg_pidref_get_unit_full(ثابت PidRef *pidref, نویسه **ret_unit, نویسه **ret_subgroup);
ایستا درونخطی صحیح cg_pidref_get_unit(ثابت PidRef *pidref, نویسه **ret_unit) {
        بازگشت cg_pidref_get_unit_full(pidref, ret_unit, پوچ);
}
صحیح cg_pid_get_user_unit_full(pid_t pid, نویسه **ret_unit, نویسه **ret_subgroup);
ایستا درونخطی صحیح cg_pid_get_user_unit(pid_t pid, نویسه **ret_unit) {
        بازگشت cg_pid_get_user_unit_full(pid, ret_unit, پوچ);
}
صحیح cg_pidref_get_user_unit_full(ثابت PidRef *pidref, نویسه **ret_unit, نویسه **ret_subgroup);
ایستا درونخطی صحیح cg_pidref_get_user_unit(ثابت PidRef *pidref, نویسه **ret_unit) {
        بازگشت cg_pidref_get_user_unit_full(pidref, ret_unit, پوچ);
}
صحیح cg_pid_get_machine_name(pid_t pid, نویسه **ret_machine);
صحیح cg_pid_get_slice(pid_t pid, نویسه **ret_slice);
صحیح cg_pid_get_user_slice(pid_t pid, نویسه **ret_slice);

صحیح cg_path_decode_unit(ثابت نویسه *cgroup, نویسه **ret_unit);

bool cg_needs_escape(ثابت نویسه *p) _pure_;
صحیح cg_escape(ثابت نویسه *p, نویسه **ret);
نویسه* cg_unescape(ثابت نویسه *p) _pure_;

صحیح cg_slice_to_path(ثابت نویسه *unit, نویسه **ret);

صحیح cg_mask_supported(CGroupMask *ret);
صحیح cg_mask_supported_subtree(ثابت نویسه *ریشه, CGroupMask *ret);
صحیح cg_mask_from_string(ثابت نویسه *s, CGroupMask *ret);
صحیح cg_mask_to_string(CGroupMask mask, نویسه **ret);

bool cg_kill_supported(تهی);

DECLARE_STRING_TABLE_LOOKUP(cgroup_controller, CGroupController);

تعریفنوع شمارش ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        _MANAGED_OOM_MODE_MAX,
        _MANAGED_OOM_MODE_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

تعریفنوع شمارش ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);

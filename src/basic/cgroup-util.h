/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

#define SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
வகைவரை எண்ணிக்கை CGroupController {
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
வகைவரை எண்ணிக்கை CGroupMask {
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

நிலையான உள்வரி bool CGROUP_WEIGHT_IS_OK(uint64_t x) {
        திரும்பு
            x == CGROUP_WEIGHT_INVALID ||
            (x >= CGROUP_WEIGHT_MIN && x <= CGROUP_WEIGHT_MAX);
}

/* IO limits on unified hierarchy */
வகைவரை எண்ணிக்கை CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        _CGROUP_IO_LIMIT_TYPE_MAX,
        _CGROUP_IO_LIMIT_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

வெளி மாறிலி uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX];

DECLARE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);
வெறுமை cgroup_io_limits_list(வெறுமை);

/* Special values for the io.bfq.weight attribute */
#define CGROUP_BFQ_WEIGHT_INVALID UINT64_MAX
#define CGROUP_BFQ_WEIGHT_MIN UINT64_C(1)
#define CGROUP_BFQ_WEIGHT_MAX UINT64_C(1000)
#define CGROUP_BFQ_WEIGHT_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
நிலையான உள்வரி uint64_t BFQ_WEIGHT(uint64_t io_weight) {
        திரும்பு
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

முழு cg_is_available(வெறுமை);

முழு cg_path_open(மாறிலி எழுத்து *path);
முழு cg_cgroupid_open(முழு cgroupfs_fd, uint64_t id);

முழு cg_path_from_cgroupid(முழு cgroupfs_fd, uint64_t id, எழுத்து **ret);

வகைவரை எண்ணிக்கை CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

முழு cg_enumerate_processes(மாறிலி எழுத்து *path, FILE **ret);
முழு cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
முழு cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

முழு cg_enumerate_subgroups(மாறிலி எழுத்து *path, DIR **ret);
முழு cg_read_subgroup(DIR *d, எழுத்து **ret);

வகைவரை முழு (*cg_kill_log_func_t)(மாறிலி PidRef *pid, முழு sig, வெறுமை *userdata);

முழு cg_kill(மாறிலி எழுத்து *path, முழு sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, வெறுமை *userdata);
முழு cg_kill_kernel_sigkill(மாறிலி எழுத்து *path, uint64_t *ret_n_pids_killed);
முழு cg_kill_recursive(மாறிலி எழுத்து *path, முழு sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, வெறுமை *userdata);

முழு cg_split_spec(மாறிலி எழுத்து *spec, எழுத்து **ret_controller, எழுத்து **ret_path);

முழு cg_get_path(மாறிலி எழுத்து *path, மாறிலி எழுத்து *suffix, எழுத்து **ret);

முழு cg_pid_get_path(pid_t pid, எழுத்து **ret);
முழு cg_pidref_get_path(மாறிலி PidRef *pidref, எழுத்து **ret);

முழு cg_is_threaded(மாறிலி எழுத்து *path);

முழு cg_is_delegated(மாறிலி எழுத்து *path);
முழு cg_is_delegated_fd(முழு fd);

முழு cg_has_coredump_receive(மாறிலி எழுத்து *path);

முழு cg_set_attribute(மாறிலி எழுத்து *path, மாறிலி எழுத்து *attribute, மாறிலி எழுத்து *மதிப்பு);
முழு cg_get_attribute(மாறிலி எழுத்து *path, மாறிலி எழுத்து *attribute, எழுத்து **ret);
முழு cg_get_attribute_as_uint64(மாறிலி எழுத்து *path, மாறிலி எழுத்து *attribute, uint64_t *ret);
முழு cg_get_attribute_as_bool(மாறிலி எழுத்து *path, மாறிலி எழுத்து *attribute);

முழு cg_get_keyed_attribute(மாறிலி எழுத்து *path, மாறிலி எழுத்து *attribute, எழுத்து * மாறிலி *சாவிகள், எழுத்து **values);
முழு cg_get_keyed_attribute_uint64(மாறிலி எழுத்து *path, மாறிலி எழுத்து *attribute, மாறிலி எழுத்து *சாவி, uint64_t *ret);

முழு cg_get_owner(மாறிலி எழுத்து *path, uid_t *ret_uid);

முழு cg_set_xattr(மாறிலி எழுத்து *path, மாறிலி எழுத்து *name, மாறிலி வெறுமை *மதிப்பு, size_t size, முழு flags);
முழு cg_get_xattr(மாறிலி எழுத்து *path, மாறிலி எழுத்து *name, எழுத்து **ret, size_t *ret_size);
/* Returns negative on error, and 0 or 1 on success for the bool value */
முழு cg_get_xattr_bool(மாறிலி எழுத்து *path, மாறிலி எழுத்து *name);
முழு cg_remove_xattr(மாறிலி எழுத்து *path, மாறிலி எழுத்து *name);

முழு cg_is_empty(மாறிலி எழுத்து *path);

முழு cg_get_வேர்_path(எழுத்து **path);

முழு cg_path_get_session(மாறிலி எழுத்து *path, எழுத்து **ret_session);
முழு cg_path_get_owner_uid(மாறிலி எழுத்து *path, uid_t *ret_uid);
முழு cg_path_get_unit_full(மாறிலி எழுத்து *path, எழுத்து **ret_unit, எழுத்து **ret_subgroup);
நிலையான உள்வரி முழு cg_path_get_unit(மாறிலி எழுத்து *path, எழுத்து **ret_unit) {
        திரும்பு cg_path_get_unit_full(path, ret_unit, வெற்று);
}
முழு cg_path_get_unit_path(மாறிலி எழுத்து *path, எழுத்து **ret_unit);
முழு cg_path_get_user_unit_full(மாறிலி எழுத்து *path, எழுத்து **ret_unit, எழுத்து **ret_subgroup);
நிலையான உள்வரி முழு cg_path_get_user_unit(மாறிலி எழுத்து *path, எழுத்து **ret_unit) {
        திரும்பு cg_path_get_user_unit_full(path, ret_unit, வெற்று);
}
முழு cg_path_get_machine_name(மாறிலி எழுத்து *path, எழுத்து **ret_machine);
முழு cg_path_get_slice(மாறிலி எழுத்து *path, எழுத்து **ret_slice);
முழு cg_path_get_user_slice(மாறிலி எழுத்து *path, எழுத்து **ret_slice);

முழு cg_shift_path(மாறிலி எழுத்து *cgroup, மாறிலி எழுத்து *cached_வேர், மாறிலி எழுத்து **ret_shifted);
முழு cg_pid_get_path_shifted(pid_t pid, மாறிலி எழுத்து *cached_வேர், எழுத்து **ret_cgroup);
முழு cg_pidref_get_path_shifted(மாறிலி PidRef *pidref, மாறிலி எழுத்து *cached_வேர், எழுத்து **ret_cgroup);

முழு cg_pid_get_session(pid_t pid, எழுத்து **ret_session);
முழு cg_pidref_get_session(மாறிலி PidRef *pidref, எழுத்து **ret);
முழு cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
முழு cg_pidref_get_owner_uid(மாறிலி PidRef *pidref, uid_t *ret);
முழு cg_pid_get_unit_full(pid_t pid, எழுத்து **ret_unit, எழுத்து **ret_subgroup);
நிலையான உள்வரி முழு cg_pid_get_unit(pid_t pid, எழுத்து **ret_unit) {
        திரும்பு cg_pid_get_unit_full(pid, ret_unit, வெற்று);
}
முழு cg_pidref_get_unit_full(மாறிலி PidRef *pidref, எழுத்து **ret_unit, எழுத்து **ret_subgroup);
நிலையான உள்வரி முழு cg_pidref_get_unit(மாறிலி PidRef *pidref, எழுத்து **ret_unit) {
        திரும்பு cg_pidref_get_unit_full(pidref, ret_unit, வெற்று);
}
முழு cg_pid_get_user_unit_full(pid_t pid, எழுத்து **ret_unit, எழுத்து **ret_subgroup);
நிலையான உள்வரி முழு cg_pid_get_user_unit(pid_t pid, எழுத்து **ret_unit) {
        திரும்பு cg_pid_get_user_unit_full(pid, ret_unit, வெற்று);
}
முழு cg_pidref_get_user_unit_full(மாறிலி PidRef *pidref, எழுத்து **ret_unit, எழுத்து **ret_subgroup);
நிலையான உள்வரி முழு cg_pidref_get_user_unit(மாறிலி PidRef *pidref, எழுத்து **ret_unit) {
        திரும்பு cg_pidref_get_user_unit_full(pidref, ret_unit, வெற்று);
}
முழு cg_pid_get_machine_name(pid_t pid, எழுத்து **ret_machine);
முழு cg_pid_get_slice(pid_t pid, எழுத்து **ret_slice);
முழு cg_pid_get_user_slice(pid_t pid, எழுத்து **ret_slice);

முழு cg_path_decode_unit(மாறிலி எழுத்து *cgroup, எழுத்து **ret_unit);

bool cg_needs_escape(மாறிலி எழுத்து *p) _pure_;
முழு cg_escape(மாறிலி எழுத்து *p, எழுத்து **ret);
எழுத்து* cg_unescape(மாறிலி எழுத்து *p) _pure_;

முழு cg_slice_to_path(மாறிலி எழுத்து *unit, எழுத்து **ret);

முழு cg_mask_supported(CGroupMask *ret);
முழு cg_mask_supported_subtree(மாறிலி எழுத்து *வேர், CGroupMask *ret);
முழு cg_mask_from_string(மாறிலி எழுத்து *s, CGroupMask *ret);
முழு cg_mask_to_string(CGroupMask mask, எழுத்து **ret);

bool cg_kill_supported(வெறுமை);

DECLARE_STRING_TABLE_LOOKUP(cgroup_controller, CGroupController);

வகைவரை எண்ணிக்கை ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        _MANAGED_OOM_MODE_MAX,
        _MANAGED_OOM_MODE_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

வகைவரை எண்ணிக்கை ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);

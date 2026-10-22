/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

#define SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ CGroupController {
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ CGroupMask {
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

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool CGROUP_WEIGHT_IS_OK(uint64_t x) {
        ಪ್ರತ್ಯಾಗಮ
            x == CGROUP_WEIGHT_INVALID ||
            (x >= CGROUP_WEIGHT_MIN && x <= CGROUP_WEIGHT_MAX);
}

/* IO limits on unified hierarchy */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        _CGROUP_IO_LIMIT_TYPE_MAX,
        _CGROUP_IO_LIMIT_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

ಬಾಹ್ಯ ಧ್ರುವ uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX];

DECLARE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);
ಶೂನ್ಯ cgroup_io_limits_list(ಶೂನ್ಯ);

/* Special values for the io.bfq.weight attribute */
#define CGROUP_BFQ_WEIGHT_INVALID UINT64_MAX
#define CGROUP_BFQ_WEIGHT_MIN UINT64_C(1)
#define CGROUP_BFQ_WEIGHT_MAX UINT64_C(1000)
#define CGROUP_BFQ_WEIGHT_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ uint64_t BFQ_WEIGHT(uint64_t io_weight) {
        ಪ್ರತ್ಯಾಗಮ
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

ಪೂರ್ಣಾಙ್ಕ cg_is_available(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ cg_path_open(ಧ್ರುವ ವರ್ಣ *path);
ಪೂರ್ಣಾಙ್ಕ cg_cgroupid_open(ಪೂರ್ಣಾಙ್ಕ cgroupfs_fd, uint64_t id);

ಪೂರ್ಣಾಙ್ಕ cg_path_from_cgroupid(ಪೂರ್ಣಾಙ್ಕ cgroupfs_fd, uint64_t id, ವರ್ಣ **ret);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

ಪೂರ್ಣಾಙ್ಕ cg_enumerate_processes(ಧ್ರುವ ವರ್ಣ *path, FILE **ret);
ಪೂರ್ಣಾಙ್ಕ cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
ಪೂರ್ಣಾಙ್ಕ cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

ಪೂರ್ಣಾಙ್ಕ cg_enumerate_subgroups(ಧ್ರುವ ವರ್ಣ *path, DIR **ret);
ಪೂರ್ಣಾಙ್ಕ cg_read_subgroup(DIR *d, ವರ್ಣ **ret);

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*cg_kill_log_func_t)(ಧ್ರುವ PidRef *pid, ಪೂರ್ಣಾಙ್ಕ sig, ಶೂನ್ಯ *userdata);

ಪೂರ್ಣಾಙ್ಕ cg_kill(ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, ಶೂನ್ಯ *userdata);
ಪೂರ್ಣಾಙ್ಕ cg_kill_kernel_sigkill(ಧ್ರುವ ವರ್ಣ *path, uint64_t *ret_n_pids_killed);
ಪೂರ್ಣಾಙ್ಕ cg_kill_recursive(ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, ಶೂನ್ಯ *userdata);

ಪೂರ್ಣಾಙ್ಕ cg_split_spec(ಧ್ರುವ ವರ್ಣ *spec, ವರ್ಣ **ret_controller, ವರ್ಣ **ret_path);

ಪೂರ್ಣಾಙ್ಕ cg_get_path(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *suffix, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ cg_pid_get_path(pid_t pid, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ cg_pidref_get_path(ಧ್ರುವ PidRef *pidref, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ cg_is_threaded(ಧ್ರುವ ವರ್ಣ *path);

ಪೂರ್ಣಾಙ್ಕ cg_is_delegated(ಧ್ರುವ ವರ್ಣ *path);
ಪೂರ್ಣಾಙ್ಕ cg_is_delegated_fd(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ cg_has_coredump_receive(ಧ್ರುವ ವರ್ಣ *path);

ಪೂರ್ಣಾಙ್ಕ cg_set_attribute(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *attribute, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ);
ಪೂರ್ಣಾಙ್ಕ cg_get_attribute(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *attribute, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ cg_get_attribute_as_uint64(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *attribute, uint64_t *ret);
ಪೂರ್ಣಾಙ್ಕ cg_get_attribute_as_bool(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *attribute);

ಪೂರ್ಣಾಙ್ಕ cg_get_keyed_attribute(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *attribute, ವರ್ಣ * ಧ್ರುವ *ಕೀಲಿಗಳು, ವರ್ಣ **values);
ಪೂರ್ಣಾಙ್ಕ cg_get_keyed_attribute_uint64(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *attribute, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, uint64_t *ret);

ಪೂರ್ಣಾಙ್ಕ cg_get_owner(ಧ್ರುವ ವರ್ಣ *path, uid_t *ret_uid);

ಪೂರ್ಣಾಙ್ಕ cg_set_xattr(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ಶೂನ್ಯ *ಮೌಲ್ಯ, size_t size, ಪೂರ್ಣಾಙ್ಕ flags);
ಪೂರ್ಣಾಙ್ಕ cg_get_xattr(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ret, size_t *ret_size);
/* Returns negative on error, and 0 or 1 on success for the bool value */
ಪೂರ್ಣಾಙ್ಕ cg_get_xattr_bool(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name);
ಪೂರ್ಣಾಙ್ಕ cg_remove_xattr(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name);

ಪೂರ್ಣಾಙ್ಕ cg_is_empty(ಧ್ರುವ ವರ್ಣ *path);

ಪೂರ್ಣಾಙ್ಕ cg_get_ಬೇರು_path(ವರ್ಣ **path);

ಪೂರ್ಣಾಙ್ಕ cg_path_get_session(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_session);
ಪೂರ್ಣಾಙ್ಕ cg_path_get_owner_uid(ಧ್ರುವ ವರ್ಣ *path, uid_t *ret_uid);
ಪೂರ್ಣಾಙ್ಕ cg_path_get_unit_full(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_unit, ವರ್ಣ **ret_subgroup);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ cg_path_get_unit(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_unit) {
        ಪ್ರತ್ಯಾಗಮ cg_path_get_unit_full(path, ret_unit, ರಿಕ್ತ);
}
ಪೂರ್ಣಾಙ್ಕ cg_path_get_unit_path(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_unit);
ಪೂರ್ಣಾಙ್ಕ cg_path_get_user_unit_full(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_unit, ವರ್ಣ **ret_subgroup);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ cg_path_get_user_unit(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_unit) {
        ಪ್ರತ್ಯಾಗಮ cg_path_get_user_unit_full(path, ret_unit, ರಿಕ್ತ);
}
ಪೂರ್ಣಾಙ್ಕ cg_path_get_machine_name(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_machine);
ಪೂರ್ಣಾಙ್ಕ cg_path_get_slice(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_slice);
ಪೂರ್ಣಾಙ್ಕ cg_path_get_user_slice(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_slice);

ಪೂರ್ಣಾಙ್ಕ cg_shift_path(ಧ್ರುವ ವರ್ಣ *cgroup, ಧ್ರುವ ವರ್ಣ *cached_ಬೇರು, ಧ್ರುವ ವರ್ಣ **ret_shifted);
ಪೂರ್ಣಾಙ್ಕ cg_pid_get_path_shifted(pid_t pid, ಧ್ರುವ ವರ್ಣ *cached_ಬೇರು, ವರ್ಣ **ret_cgroup);
ಪೂರ್ಣಾಙ್ಕ cg_pidref_get_path_shifted(ಧ್ರುವ PidRef *pidref, ಧ್ರುವ ವರ್ಣ *cached_ಬೇರು, ವರ್ಣ **ret_cgroup);

ಪೂರ್ಣಾಙ್ಕ cg_pid_get_session(pid_t pid, ವರ್ಣ **ret_session);
ಪೂರ್ಣಾಙ್ಕ cg_pidref_get_session(ಧ್ರುವ PidRef *pidref, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
ಪೂರ್ಣಾಙ್ಕ cg_pidref_get_owner_uid(ಧ್ರುವ PidRef *pidref, uid_t *ret);
ಪೂರ್ಣಾಙ್ಕ cg_pid_get_unit_full(pid_t pid, ವರ್ಣ **ret_unit, ವರ್ಣ **ret_subgroup);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ cg_pid_get_unit(pid_t pid, ವರ್ಣ **ret_unit) {
        ಪ್ರತ್ಯಾಗಮ cg_pid_get_unit_full(pid, ret_unit, ರಿಕ್ತ);
}
ಪೂರ್ಣಾಙ್ಕ cg_pidref_get_unit_full(ಧ್ರುವ PidRef *pidref, ವರ್ಣ **ret_unit, ವರ್ಣ **ret_subgroup);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ cg_pidref_get_unit(ಧ್ರುವ PidRef *pidref, ವರ್ಣ **ret_unit) {
        ಪ್ರತ್ಯಾಗಮ cg_pidref_get_unit_full(pidref, ret_unit, ರಿಕ್ತ);
}
ಪೂರ್ಣಾಙ್ಕ cg_pid_get_user_unit_full(pid_t pid, ವರ್ಣ **ret_unit, ವರ್ಣ **ret_subgroup);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ cg_pid_get_user_unit(pid_t pid, ವರ್ಣ **ret_unit) {
        ಪ್ರತ್ಯಾಗಮ cg_pid_get_user_unit_full(pid, ret_unit, ರಿಕ್ತ);
}
ಪೂರ್ಣಾಙ್ಕ cg_pidref_get_user_unit_full(ಧ್ರುವ PidRef *pidref, ವರ್ಣ **ret_unit, ವರ್ಣ **ret_subgroup);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ cg_pidref_get_user_unit(ಧ್ರುವ PidRef *pidref, ವರ್ಣ **ret_unit) {
        ಪ್ರತ್ಯಾಗಮ cg_pidref_get_user_unit_full(pidref, ret_unit, ರಿಕ್ತ);
}
ಪೂರ್ಣಾಙ್ಕ cg_pid_get_machine_name(pid_t pid, ವರ್ಣ **ret_machine);
ಪೂರ್ಣಾಙ್ಕ cg_pid_get_slice(pid_t pid, ವರ್ಣ **ret_slice);
ಪೂರ್ಣಾಙ್ಕ cg_pid_get_user_slice(pid_t pid, ವರ್ಣ **ret_slice);

ಪೂರ್ಣಾಙ್ಕ cg_path_decode_unit(ಧ್ರುವ ವರ್ಣ *cgroup, ವರ್ಣ **ret_unit);

bool cg_needs_escape(ಧ್ರುವ ವರ್ಣ *p) _pure_;
ಪೂರ್ಣಾಙ್ಕ cg_escape(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ **ret);
ವರ್ಣ* cg_unescape(ಧ್ರುವ ವರ್ಣ *p) _pure_;

ಪೂರ್ಣಾಙ್ಕ cg_slice_to_path(ಧ್ರುವ ವರ್ಣ *unit, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ cg_mask_supported(CGroupMask *ret);
ಪೂರ್ಣಾಙ್ಕ cg_mask_supported_subtree(ಧ್ರುವ ವರ್ಣ *ಬೇರು, CGroupMask *ret);
ಪೂರ್ಣಾಙ್ಕ cg_mask_from_string(ಧ್ರುವ ವರ್ಣ *s, CGroupMask *ret);
ಪೂರ್ಣಾಙ್ಕ cg_mask_to_string(CGroupMask mask, ವರ್ಣ **ret);

bool cg_kill_supported(ಶೂನ್ಯ);

DECLARE_STRING_TABLE_LOOKUP(cgroup_controller, CGroupController);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        _MANAGED_OOM_MODE_MAX,
        _MANAGED_OOM_MODE_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);

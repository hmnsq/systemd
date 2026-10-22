/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

#define SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
প্রকারলেখ গণনা CGroupController {
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
প্রকারলেখ গণনা CGroupMask {
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

স্থির অন্তঃরেখ bool CGROUP_WEIGHT_IS_OK(uint64_t x) {
        ফেরত
            x == CGROUP_WEIGHT_INVALID ||
            (x >= CGROUP_WEIGHT_MIN && x <= CGROUP_WEIGHT_MAX);
}

/* IO limits on unified hierarchy */
প্রকারলেখ গণনা CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        _CGROUP_IO_LIMIT_TYPE_MAX,
        _CGROUP_IO_LIMIT_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

বাহ্যিক ধ্রুবক uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX];

DECLARE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);
শূন্য cgroup_io_limits_list(শূন্য);

/* Special values for the io.bfq.weight attribute */
#define CGROUP_BFQ_WEIGHT_INVALID UINT64_MAX
#define CGROUP_BFQ_WEIGHT_MIN UINT64_C(1)
#define CGROUP_BFQ_WEIGHT_MAX UINT64_C(1000)
#define CGROUP_BFQ_WEIGHT_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
স্থির অন্তঃরেখ uint64_t BFQ_WEIGHT(uint64_t io_weight) {
        ফেরত
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

পূর্ণসংখ্যা cg_is_available(শূন্য);

পূর্ণসংখ্যা cg_path_open(ধ্রুবক অক্ষর *path);
পূর্ণসংখ্যা cg_cgroupid_open(পূর্ণসংখ্যা cgroupfs_fd, uint64_t id);

পূর্ণসংখ্যা cg_path_from_cgroupid(পূর্ণসংখ্যা cgroupfs_fd, uint64_t id, অক্ষর **ret);

প্রকারলেখ গণনা CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

পূর্ণসংখ্যা cg_enumerate_processes(ধ্রুবক অক্ষর *path, FILE **ret);
পূর্ণসংখ্যা cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
পূর্ণসংখ্যা cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

পূর্ণসংখ্যা cg_enumerate_subgroups(ধ্রুবক অক্ষর *path, DIR **ret);
পূর্ণসংখ্যা cg_read_subgroup(DIR *d, অক্ষর **ret);

প্রকারলেখ পূর্ণসংখ্যা (*cg_kill_log_func_t)(ধ্রুবক PidRef *pid, পূর্ণসংখ্যা sig, শূন্য *userdata);

পূর্ণসংখ্যা cg_kill(ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, শূন্য *userdata);
পূর্ণসংখ্যা cg_kill_kernel_sigkill(ধ্রুবক অক্ষর *path, uint64_t *ret_n_pids_killed);
পূর্ণসংখ্যা cg_kill_recursive(ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, শূন্য *userdata);

পূর্ণসংখ্যা cg_split_spec(ধ্রুবক অক্ষর *spec, অক্ষর **ret_controller, অক্ষর **ret_path);

পূর্ণসংখ্যা cg_get_path(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *suffix, অক্ষর **ret);

পূর্ণসংখ্যা cg_pid_get_path(pid_t pid, অক্ষর **ret);
পূর্ণসংখ্যা cg_pidref_get_path(ধ্রুবক PidRef *pidref, অক্ষর **ret);

পূর্ণসংখ্যা cg_is_threaded(ধ্রুবক অক্ষর *path);

পূর্ণসংখ্যা cg_is_delegated(ধ্রুবক অক্ষর *path);
পূর্ণসংখ্যা cg_is_delegated_fd(পূর্ণসংখ্যা fd);

পূর্ণসংখ্যা cg_has_coredump_receive(ধ্রুবক অক্ষর *path);

পূর্ণসংখ্যা cg_set_attribute(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *attribute, ধ্রুবক অক্ষর *মান);
পূর্ণসংখ্যা cg_get_attribute(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *attribute, অক্ষর **ret);
পূর্ণসংখ্যা cg_get_attribute_as_uint64(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *attribute, uint64_t *ret);
পূর্ণসংখ্যা cg_get_attribute_as_bool(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *attribute);

পূর্ণসংখ্যা cg_get_keyed_attribute(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *attribute, অক্ষর * ধ্রুবক *চাবিগুলি, অক্ষর **values);
পূর্ণসংখ্যা cg_get_keyed_attribute_uint64(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *attribute, ধ্রুবক অক্ষর *চাবি, uint64_t *ret);

পূর্ণসংখ্যা cg_get_owner(ধ্রুবক অক্ষর *path, uid_t *ret_uid);

পূর্ণসংখ্যা cg_set_xattr(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name, ধ্রুবক শূন্য *মান, size_t আকার_2, পূর্ণসংখ্যা flags);
পূর্ণসংখ্যা cg_get_xattr(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name, অক্ষর **ret, size_t *ret_আকার);
/* Returns negative on error, and 0 or 1 on success for the bool value */
পূর্ণসংখ্যা cg_get_xattr_bool(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name);
পূর্ণসংখ্যা cg_remove_xattr(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name);

পূর্ণসংখ্যা cg_is_empty(ধ্রুবক অক্ষর *path);

পূর্ণসংখ্যা cg_get_মূল_path(অক্ষর **path);

পূর্ণসংখ্যা cg_path_get_session(ধ্রুবক অক্ষর *path, অক্ষর **ret_session);
পূর্ণসংখ্যা cg_path_get_owner_uid(ধ্রুবক অক্ষর *path, uid_t *ret_uid);
পূর্ণসংখ্যা cg_path_get_unit_full(ধ্রুবক অক্ষর *path, অক্ষর **ret_unit, অক্ষর **ret_subgroup);
স্থির অন্তঃরেখ পূর্ণসংখ্যা cg_path_get_unit(ধ্রুবক অক্ষর *path, অক্ষর **ret_unit) {
        ফেরত cg_path_get_unit_full(path, ret_unit, ফাঁকা);
}
পূর্ণসংখ্যা cg_path_get_unit_path(ধ্রুবক অক্ষর *path, অক্ষর **ret_unit);
পূর্ণসংখ্যা cg_path_get_user_unit_full(ধ্রুবক অক্ষর *path, অক্ষর **ret_unit, অক্ষর **ret_subgroup);
স্থির অন্তঃরেখ পূর্ণসংখ্যা cg_path_get_user_unit(ধ্রুবক অক্ষর *path, অক্ষর **ret_unit) {
        ফেরত cg_path_get_user_unit_full(path, ret_unit, ফাঁকা);
}
পূর্ণসংখ্যা cg_path_get_machine_name(ধ্রুবক অক্ষর *path, অক্ষর **ret_machine);
পূর্ণসংখ্যা cg_path_get_slice(ধ্রুবক অক্ষর *path, অক্ষর **ret_slice);
পূর্ণসংখ্যা cg_path_get_user_slice(ধ্রুবক অক্ষর *path, অক্ষর **ret_slice);

পূর্ণসংখ্যা cg_shift_path(ধ্রুবক অক্ষর *cgroup, ধ্রুবক অক্ষর *cached_মূল, ধ্রুবক অক্ষর **ret_shifted);
পূর্ণসংখ্যা cg_pid_get_path_shifted(pid_t pid, ধ্রুবক অক্ষর *cached_মূল, অক্ষর **ret_cgroup);
পূর্ণসংখ্যা cg_pidref_get_path_shifted(ধ্রুবক PidRef *pidref, ধ্রুবক অক্ষর *cached_মূল, অক্ষর **ret_cgroup);

পূর্ণসংখ্যা cg_pid_get_session(pid_t pid, অক্ষর **ret_session);
পূর্ণসংখ্যা cg_pidref_get_session(ধ্রুবক PidRef *pidref, অক্ষর **ret);
পূর্ণসংখ্যা cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
পূর্ণসংখ্যা cg_pidref_get_owner_uid(ধ্রুবক PidRef *pidref, uid_t *ret);
পূর্ণসংখ্যা cg_pid_get_unit_full(pid_t pid, অক্ষর **ret_unit, অক্ষর **ret_subgroup);
স্থির অন্তঃরেখ পূর্ণসংখ্যা cg_pid_get_unit(pid_t pid, অক্ষর **ret_unit) {
        ফেরত cg_pid_get_unit_full(pid, ret_unit, ফাঁকা);
}
পূর্ণসংখ্যা cg_pidref_get_unit_full(ধ্রুবক PidRef *pidref, অক্ষর **ret_unit, অক্ষর **ret_subgroup);
স্থির অন্তঃরেখ পূর্ণসংখ্যা cg_pidref_get_unit(ধ্রুবক PidRef *pidref, অক্ষর **ret_unit) {
        ফেরত cg_pidref_get_unit_full(pidref, ret_unit, ফাঁকা);
}
পূর্ণসংখ্যা cg_pid_get_user_unit_full(pid_t pid, অক্ষর **ret_unit, অক্ষর **ret_subgroup);
স্থির অন্তঃরেখ পূর্ণসংখ্যা cg_pid_get_user_unit(pid_t pid, অক্ষর **ret_unit) {
        ফেরত cg_pid_get_user_unit_full(pid, ret_unit, ফাঁকা);
}
পূর্ণসংখ্যা cg_pidref_get_user_unit_full(ধ্রুবক PidRef *pidref, অক্ষর **ret_unit, অক্ষর **ret_subgroup);
স্থির অন্তঃরেখ পূর্ণসংখ্যা cg_pidref_get_user_unit(ধ্রুবক PidRef *pidref, অক্ষর **ret_unit) {
        ফেরত cg_pidref_get_user_unit_full(pidref, ret_unit, ফাঁকা);
}
পূর্ণসংখ্যা cg_pid_get_machine_name(pid_t pid, অক্ষর **ret_machine);
পূর্ণসংখ্যা cg_pid_get_slice(pid_t pid, অক্ষর **ret_slice);
পূর্ণসংখ্যা cg_pid_get_user_slice(pid_t pid, অক্ষর **ret_slice);

পূর্ণসংখ্যা cg_path_decode_unit(ধ্রুবক অক্ষর *cgroup, অক্ষর **ret_unit);

bool cg_needs_escape(ধ্রুবক অক্ষর *p) _pure_;
পূর্ণসংখ্যা cg_escape(ধ্রুবক অক্ষর *p, অক্ষর **ret);
অক্ষর* cg_unescape(ধ্রুবক অক্ষর *p) _pure_;

পূর্ণসংখ্যা cg_slice_to_path(ধ্রুবক অক্ষর *unit, অক্ষর **ret);

পূর্ণসংখ্যা cg_mask_supported(CGroupMask *ret);
পূর্ণসংখ্যা cg_mask_supported_subtree(ধ্রুবক অক্ষর *মূল, CGroupMask *ret);
পূর্ণসংখ্যা cg_mask_from_string(ধ্রুবক অক্ষর *s, CGroupMask *ret);
পূর্ণসংখ্যা cg_mask_to_string(CGroupMask mask, অক্ষর **ret);

bool cg_kill_supported(শূন্য);

DECLARE_STRING_TABLE_LOOKUP(cgroup_controller, CGroupController);

প্রকারলেখ গণনা ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        _MANAGED_OOM_MODE_MAX,
        _MANAGED_OOM_MODE_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

প্রকারলেখ গণনা ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);

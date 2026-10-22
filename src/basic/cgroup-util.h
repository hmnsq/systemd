/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

#define SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
തരനിർവചനം എണ്ണൽ CGroupController {
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
തരനിർവചനം എണ്ണൽ CGroupMask {
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

സ്ഥിതം വരിയിൽ bool CGROUP_WEIGHT_IS_OK(uint64_t x) {
        തിരികെ
            x == CGROUP_WEIGHT_INVALID ||
            (x >= CGROUP_WEIGHT_MIN && x <= CGROUP_WEIGHT_MAX);
}

/* IO limits on unified hierarchy */
തരനിർവചനം എണ്ണൽ CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        _CGROUP_IO_LIMIT_TYPE_MAX,
        _CGROUP_IO_LIMIT_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

ബാഹ്യ സ്ഥിരം uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX];

DECLARE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);
ശൂന്യം cgroup_io_limits_list(ശൂന്യം);

/* Special values for the io.bfq.weight attribute */
#define CGROUP_BFQ_WEIGHT_INVALID UINT64_MAX
#define CGROUP_BFQ_WEIGHT_MIN UINT64_C(1)
#define CGROUP_BFQ_WEIGHT_MAX UINT64_C(1000)
#define CGROUP_BFQ_WEIGHT_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
സ്ഥിതം വരിയിൽ uint64_t BFQ_WEIGHT(uint64_t io_weight) {
        തിരികെ
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

പൂർണ്ണസംഖ്യ cg_is_available(ശൂന്യം);

പൂർണ്ണസംഖ്യ cg_path_open(സ്ഥിരം അക്ഷരം *path);
പൂർണ്ണസംഖ്യ cg_cgroupid_open(പൂർണ്ണസംഖ്യ cgroupfs_fd, uint64_t id);

പൂർണ്ണസംഖ്യ cg_path_from_cgroupid(പൂർണ്ണസംഖ്യ cgroupfs_fd, uint64_t id, അക്ഷരം **ret);

തരനിർവചനം എണ്ണൽ CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

പൂർണ്ണസംഖ്യ cg_enumerate_processes(സ്ഥിരം അക്ഷരം *path, FILE **ret);
പൂർണ്ണസംഖ്യ cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
പൂർണ്ണസംഖ്യ cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

പൂർണ്ണസംഖ്യ cg_enumerate_subgroups(സ്ഥിരം അക്ഷരം *path, DIR **ret);
പൂർണ്ണസംഖ്യ cg_read_subgroup(DIR *d, അക്ഷരം **ret);

തരനിർവചനം പൂർണ്ണസംഖ്യ (*cg_kill_log_func_t)(സ്ഥിരം PidRef *pid, പൂർണ്ണസംഖ്യ sig, ശൂന്യം *userdata);

പൂർണ്ണസംഖ്യ cg_kill(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, ശൂന്യം *userdata);
പൂർണ്ണസംഖ്യ cg_kill_kernel_sigkill(സ്ഥിരം അക്ഷരം *path, uint64_t *ret_n_pids_killed);
പൂർണ്ണസംഖ്യ cg_kill_recursive(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, ശൂന്യം *userdata);

പൂർണ്ണസംഖ്യ cg_split_spec(സ്ഥിരം അക്ഷരം *spec, അക്ഷരം **ret_controller, അക്ഷരം **ret_path);

പൂർണ്ണസംഖ്യ cg_get_path(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *suffix, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ cg_pid_get_path(pid_t pid, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ cg_pidref_get_path(സ്ഥിരം PidRef *pidref, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ cg_is_threaded(സ്ഥിരം അക്ഷരം *path);

പൂർണ്ണസംഖ്യ cg_is_delegated(സ്ഥിരം അക്ഷരം *path);
പൂർണ്ണസംഖ്യ cg_is_delegated_fd(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ cg_has_coredump_receive(സ്ഥിരം അക്ഷരം *path);

പൂർണ്ണസംഖ്യ cg_set_attribute(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *attribute, സ്ഥിരം അക്ഷരം *മൂല്യം);
പൂർണ്ണസംഖ്യ cg_get_attribute(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *attribute, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ cg_get_attribute_as_uint64(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *attribute, uint64_t *ret);
പൂർണ്ണസംഖ്യ cg_get_attribute_as_bool(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *attribute);

പൂർണ്ണസംഖ്യ cg_get_keyed_attribute(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *attribute, അക്ഷരം * സ്ഥിരം *താക്കോലുകൾ, അക്ഷരം **values);
പൂർണ്ണസംഖ്യ cg_get_keyed_attribute_uint64(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *attribute, സ്ഥിരം അക്ഷരം *താക്കോൽ, uint64_t *ret);

പൂർണ്ണസംഖ്യ cg_get_owner(സ്ഥിരം അക്ഷരം *path, uid_t *ret_uid);

പൂർണ്ണസംഖ്യ cg_set_xattr(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name, സ്ഥിരം ശൂന്യം *മൂല്യം, size_t size, പൂർണ്ണസംഖ്യ flags);
പൂർണ്ണസംഖ്യ cg_get_xattr(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name, അക്ഷരം **ret, size_t *ret_size);
/* Returns negative on error, and 0 or 1 on success for the bool value */
പൂർണ്ണസംഖ്യ cg_get_xattr_bool(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name);
പൂർണ്ണസംഖ്യ cg_remove_xattr(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name);

പൂർണ്ണസംഖ്യ cg_is_empty(സ്ഥിരം അക്ഷരം *path);

പൂർണ്ണസംഖ്യ cg_get_വേര്_path(അക്ഷരം **path);

പൂർണ്ണസംഖ്യ cg_path_get_session(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_session);
പൂർണ്ണസംഖ്യ cg_path_get_owner_uid(സ്ഥിരം അക്ഷരം *path, uid_t *ret_uid);
പൂർണ്ണസംഖ്യ cg_path_get_unit_full(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_unit, അക്ഷരം **ret_subgroup);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ cg_path_get_unit(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_unit) {
        തിരികെ cg_path_get_unit_full(path, ret_unit, ശൂന്യ);
}
പൂർണ്ണസംഖ്യ cg_path_get_unit_path(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_unit);
പൂർണ്ണസംഖ്യ cg_path_get_user_unit_full(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_unit, അക്ഷരം **ret_subgroup);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ cg_path_get_user_unit(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_unit) {
        തിരികെ cg_path_get_user_unit_full(path, ret_unit, ശൂന്യ);
}
പൂർണ്ണസംഖ്യ cg_path_get_machine_name(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_machine);
പൂർണ്ണസംഖ്യ cg_path_get_slice(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_slice);
പൂർണ്ണസംഖ്യ cg_path_get_user_slice(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_slice);

പൂർണ്ണസംഖ്യ cg_shift_path(സ്ഥിരം അക്ഷരം *cgroup, സ്ഥിരം അക്ഷരം *cached_വേര്, സ്ഥിരം അക്ഷരം **ret_shifted);
പൂർണ്ണസംഖ്യ cg_pid_get_path_shifted(pid_t pid, സ്ഥിരം അക്ഷരം *cached_വേര്, അക്ഷരം **ret_cgroup);
പൂർണ്ണസംഖ്യ cg_pidref_get_path_shifted(സ്ഥിരം PidRef *pidref, സ്ഥിരം അക്ഷരം *cached_വേര്, അക്ഷരം **ret_cgroup);

പൂർണ്ണസംഖ്യ cg_pid_get_session(pid_t pid, അക്ഷരം **ret_session);
പൂർണ്ണസംഖ്യ cg_pidref_get_session(സ്ഥിരം PidRef *pidref, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
പൂർണ്ണസംഖ്യ cg_pidref_get_owner_uid(സ്ഥിരം PidRef *pidref, uid_t *ret);
പൂർണ്ണസംഖ്യ cg_pid_get_unit_full(pid_t pid, അക്ഷരം **ret_unit, അക്ഷരം **ret_subgroup);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ cg_pid_get_unit(pid_t pid, അക്ഷരം **ret_unit) {
        തിരികെ cg_pid_get_unit_full(pid, ret_unit, ശൂന്യ);
}
പൂർണ്ണസംഖ്യ cg_pidref_get_unit_full(സ്ഥിരം PidRef *pidref, അക്ഷരം **ret_unit, അക്ഷരം **ret_subgroup);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ cg_pidref_get_unit(സ്ഥിരം PidRef *pidref, അക്ഷരം **ret_unit) {
        തിരികെ cg_pidref_get_unit_full(pidref, ret_unit, ശൂന്യ);
}
പൂർണ്ണസംഖ്യ cg_pid_get_user_unit_full(pid_t pid, അക്ഷരം **ret_unit, അക്ഷരം **ret_subgroup);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ cg_pid_get_user_unit(pid_t pid, അക്ഷരം **ret_unit) {
        തിരികെ cg_pid_get_user_unit_full(pid, ret_unit, ശൂന്യ);
}
പൂർണ്ണസംഖ്യ cg_pidref_get_user_unit_full(സ്ഥിരം PidRef *pidref, അക്ഷരം **ret_unit, അക്ഷരം **ret_subgroup);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ cg_pidref_get_user_unit(സ്ഥിരം PidRef *pidref, അക്ഷരം **ret_unit) {
        തിരികെ cg_pidref_get_user_unit_full(pidref, ret_unit, ശൂന്യ);
}
പൂർണ്ണസംഖ്യ cg_pid_get_machine_name(pid_t pid, അക്ഷരം **ret_machine);
പൂർണ്ണസംഖ്യ cg_pid_get_slice(pid_t pid, അക്ഷരം **ret_slice);
പൂർണ്ണസംഖ്യ cg_pid_get_user_slice(pid_t pid, അക്ഷരം **ret_slice);

പൂർണ്ണസംഖ്യ cg_path_decode_unit(സ്ഥിരം അക്ഷരം *cgroup, അക്ഷരം **ret_unit);

bool cg_needs_escape(സ്ഥിരം അക്ഷരം *p) _pure_;
പൂർണ്ണസംഖ്യ cg_escape(സ്ഥിരം അക്ഷരം *p, അക്ഷരം **ret);
അക്ഷരം* cg_unescape(സ്ഥിരം അക്ഷരം *p) _pure_;

പൂർണ്ണസംഖ്യ cg_slice_to_path(സ്ഥിരം അക്ഷരം *unit, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ cg_mask_supported(CGroupMask *ret);
പൂർണ്ണസംഖ്യ cg_mask_supported_subtree(സ്ഥിരം അക്ഷരം *വേര്, CGroupMask *ret);
പൂർണ്ണസംഖ്യ cg_mask_from_string(സ്ഥിരം അക്ഷരം *s, CGroupMask *ret);
പൂർണ്ണസംഖ്യ cg_mask_to_string(CGroupMask mask, അക്ഷരം **ret);

bool cg_kill_supported(ശൂന്യം);

DECLARE_STRING_TABLE_LOOKUP(cgroup_controller, CGroupController);

തരനിർവചനം എണ്ണൽ ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        _MANAGED_OOM_MODE_MAX,
        _MANAGED_OOM_MODE_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

തരനിർവചനം എണ്ണൽ ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

#define SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
પ્રકારવ્યાખ્યા ગણના CGroupController {
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
પ્રકારવ્યાખ્યા ગણના CGroupMask {
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

સ્થિર અંતઃરેખ bool CGROUP_WEIGHT_IS_OK(uint64_t x) {
        પાછા
            x == CGROUP_WEIGHT_INVALID ||
            (x >= CGROUP_WEIGHT_MIN && x <= CGROUP_WEIGHT_MAX);
}

/* IO limits on unified hierarchy */
પ્રકારવ્યાખ્યા ગણના CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        _CGROUP_IO_LIMIT_TYPE_MAX,
        _CGROUP_IO_LIMIT_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

બાહ્ય અચલ uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX];

DECLARE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);
શૂન્ય cgroup_io_limits_list(શૂન્ય);

/* Special values for the io.bfq.weight attribute */
#define CGROUP_BFQ_WEIGHT_INVALID UINT64_MAX
#define CGROUP_BFQ_WEIGHT_MIN UINT64_C(1)
#define CGROUP_BFQ_WEIGHT_MAX UINT64_C(1000)
#define CGROUP_BFQ_WEIGHT_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
સ્થિર અંતઃરેખ uint64_t BFQ_WEIGHT(uint64_t io_weight) {
        પાછા
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

પૂર્ણાંક cg_is_available(શૂન્ય);

પૂર્ણાંક cg_path_open(અચલ અક્ષર *path);
પૂર્ણાંક cg_cgroupid_open(પૂર્ણાંક cgroupfs_fd, uint64_t id);

પૂર્ણાંક cg_path_from_cgroupid(પૂર્ણાંક cgroupfs_fd, uint64_t id, અક્ષર **ret);

પ્રકારવ્યાખ્યા ગણના CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

પૂર્ણાંક cg_enumerate_processes(અચલ અક્ષર *path, FILE **ret);
પૂર્ણાંક cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
પૂર્ણાંક cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

પૂર્ણાંક cg_enumerate_subgroups(અચલ અક્ષર *path, DIR **ret);
પૂર્ણાંક cg_read_subgroup(DIR *d, અક્ષર **ret);

પ્રકારવ્યાખ્યા પૂર્ણાંક (*cg_kill_log_func_t)(અચલ PidRef *pid, પૂર્ણાંક sig, શૂન્ય *userdata);

પૂર્ણાંક cg_kill(અચલ અક્ષર *path, પૂર્ણાંક sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, શૂન્ય *userdata);
પૂર્ણાંક cg_kill_kernel_sigkill(અચલ અક્ષર *path, uint64_t *ret_n_pids_killed);
પૂર્ણાંક cg_kill_recursive(અચલ અક્ષર *path, પૂર્ણાંક sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, શૂન્ય *userdata);

પૂર્ણાંક cg_split_spec(અચલ અક્ષર *spec, અક્ષર **ret_controller, અક્ષર **ret_path);

પૂર્ણાંક cg_get_path(અચલ અક્ષર *path, અચલ અક્ષર *suffix, અક્ષર **ret);

પૂર્ણાંક cg_pid_get_path(pid_t pid, અક્ષર **ret);
પૂર્ણાંક cg_pidref_get_path(અચલ PidRef *pidref, અક્ષર **ret);

પૂર્ણાંક cg_is_threaded(અચલ અક્ષર *path);

પૂર્ણાંક cg_is_delegated(અચલ અક્ષર *path);
પૂર્ણાંક cg_is_delegated_fd(પૂર્ણાંક fd);

પૂર્ણાંક cg_has_coredump_receive(અચલ અક્ષર *path);

પૂર્ણાંક cg_set_attribute(અચલ અક્ષર *path, અચલ અક્ષર *attribute, અચલ અક્ષર *કિંમત);
પૂર્ણાંક cg_get_attribute(અચલ અક્ષર *path, અચલ અક્ષર *attribute, અક્ષર **ret);
પૂર્ણાંક cg_get_attribute_as_uint64(અચલ અક્ષર *path, અચલ અક્ષર *attribute, uint64_t *ret);
પૂર્ણાંક cg_get_attribute_as_bool(અચલ અક્ષર *path, અચલ અક્ષર *attribute);

પૂર્ણાંક cg_get_keyed_attribute(અચલ અક્ષર *path, અચલ અક્ષર *attribute, અક્ષર * અચલ *ચાવીઓ, અક્ષર **values);
પૂર્ણાંક cg_get_keyed_attribute_uint64(અચલ અક્ષર *path, અચલ અક્ષર *attribute, અચલ અક્ષર *ચાવી, uint64_t *ret);

પૂર્ણાંક cg_get_owner(અચલ અક્ષર *path, uid_t *ret_uid);

પૂર્ણાંક cg_set_xattr(અચલ અક્ષર *path, અચલ અક્ષર *name, અચલ શૂન્ય *કિંમત, size_t size, પૂર્ણાંક flags);
પૂર્ણાંક cg_get_xattr(અચલ અક્ષર *path, અચલ અક્ષર *name, અક્ષર **ret, size_t *ret_size);
/* Returns negative on error, and 0 or 1 on success for the bool value */
પૂર્ણાંક cg_get_xattr_bool(અચલ અક્ષર *path, અચલ અક્ષર *name);
પૂર્ણાંક cg_remove_xattr(અચલ અક્ષર *path, અચલ અક્ષર *name);

પૂર્ણાંક cg_is_empty(અચલ અક્ષર *path);

પૂર્ણાંક cg_get_મૂળ_path(અક્ષર **path);

પૂર્ણાંક cg_path_get_session(અચલ અક્ષર *path, અક્ષર **ret_session);
પૂર્ણાંક cg_path_get_owner_uid(અચલ અક્ષર *path, uid_t *ret_uid);
પૂર્ણાંક cg_path_get_unit_full(અચલ અક્ષર *path, અક્ષર **ret_unit, અક્ષર **ret_subgroup);
સ્થિર અંતઃરેખ પૂર્ણાંક cg_path_get_unit(અચલ અક્ષર *path, અક્ષર **ret_unit) {
        પાછા cg_path_get_unit_full(path, ret_unit, ખાલી);
}
પૂર્ણાંક cg_path_get_unit_path(અચલ અક્ષર *path, અક્ષર **ret_unit);
પૂર્ણાંક cg_path_get_user_unit_full(અચલ અક્ષર *path, અક્ષર **ret_unit, અક્ષર **ret_subgroup);
સ્થિર અંતઃરેખ પૂર્ણાંક cg_path_get_user_unit(અચલ અક્ષર *path, અક્ષર **ret_unit) {
        પાછા cg_path_get_user_unit_full(path, ret_unit, ખાલી);
}
પૂર્ણાંક cg_path_get_machine_name(અચલ અક્ષર *path, અક્ષર **ret_machine);
પૂર્ણાંક cg_path_get_slice(અચલ અક્ષર *path, અક્ષર **ret_slice);
પૂર્ણાંક cg_path_get_user_slice(અચલ અક્ષર *path, અક્ષર **ret_slice);

પૂર્ણાંક cg_shift_path(અચલ અક્ષર *cgroup, અચલ અક્ષર *cached_મૂળ, અચલ અક્ષર **ret_shifted);
પૂર્ણાંક cg_pid_get_path_shifted(pid_t pid, અચલ અક્ષર *cached_મૂળ, અક્ષર **ret_cgroup);
પૂર્ણાંક cg_pidref_get_path_shifted(અચલ PidRef *pidref, અચલ અક્ષર *cached_મૂળ, અક્ષર **ret_cgroup);

પૂર્ણાંક cg_pid_get_session(pid_t pid, અક્ષર **ret_session);
પૂર્ણાંક cg_pidref_get_session(અચલ PidRef *pidref, અક્ષર **ret);
પૂર્ણાંક cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
પૂર્ણાંક cg_pidref_get_owner_uid(અચલ PidRef *pidref, uid_t *ret);
પૂર્ણાંક cg_pid_get_unit_full(pid_t pid, અક્ષર **ret_unit, અક્ષર **ret_subgroup);
સ્થિર અંતઃરેખ પૂર્ણાંક cg_pid_get_unit(pid_t pid, અક્ષર **ret_unit) {
        પાછા cg_pid_get_unit_full(pid, ret_unit, ખાલી);
}
પૂર્ણાંક cg_pidref_get_unit_full(અચલ PidRef *pidref, અક્ષર **ret_unit, અક્ષર **ret_subgroup);
સ્થિર અંતઃરેખ પૂર્ણાંક cg_pidref_get_unit(અચલ PidRef *pidref, અક્ષર **ret_unit) {
        પાછા cg_pidref_get_unit_full(pidref, ret_unit, ખાલી);
}
પૂર્ણાંક cg_pid_get_user_unit_full(pid_t pid, અક્ષર **ret_unit, અક્ષર **ret_subgroup);
સ્થિર અંતઃરેખ પૂર્ણાંક cg_pid_get_user_unit(pid_t pid, અક્ષર **ret_unit) {
        પાછા cg_pid_get_user_unit_full(pid, ret_unit, ખાલી);
}
પૂર્ણાંક cg_pidref_get_user_unit_full(અચલ PidRef *pidref, અક્ષર **ret_unit, અક્ષર **ret_subgroup);
સ્થિર અંતઃરેખ પૂર્ણાંક cg_pidref_get_user_unit(અચલ PidRef *pidref, અક્ષર **ret_unit) {
        પાછા cg_pidref_get_user_unit_full(pidref, ret_unit, ખાલી);
}
પૂર્ણાંક cg_pid_get_machine_name(pid_t pid, અક્ષર **ret_machine);
પૂર્ણાંક cg_pid_get_slice(pid_t pid, અક્ષર **ret_slice);
પૂર્ણાંક cg_pid_get_user_slice(pid_t pid, અક્ષર **ret_slice);

પૂર્ણાંક cg_path_decode_unit(અચલ અક્ષર *cgroup, અક્ષર **ret_unit);

bool cg_needs_escape(અચલ અક્ષર *p) _pure_;
પૂર્ણાંક cg_escape(અચલ અક્ષર *p, અક્ષર **ret);
અક્ષર* cg_unescape(અચલ અક્ષર *p) _pure_;

પૂર્ણાંક cg_slice_to_path(અચલ અક્ષર *unit, અક્ષર **ret);

પૂર્ણાંક cg_mask_supported(CGroupMask *ret);
પૂર્ણાંક cg_mask_supported_subtree(અચલ અક્ષર *મૂળ, CGroupMask *ret);
પૂર્ણાંક cg_mask_from_string(અચલ અક્ષર *s, CGroupMask *ret);
પૂર્ણાંક cg_mask_to_string(CGroupMask mask, અક્ષર **ret);

bool cg_kill_supported(શૂન્ય);

DECLARE_STRING_TABLE_LOOKUP(cgroup_controller, CGroupController);

પ્રકારવ્યાખ્યા ગણના ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        _MANAGED_OOM_MODE_MAX,
        _MANAGED_OOM_MODE_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

પ્રકારવ્યાખ્યા ગણના ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);

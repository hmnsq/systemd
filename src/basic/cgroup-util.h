/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#define SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
प्रकारनिर्देश गणना CGroupController {
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
प्रकारनिर्देश गणना CGroupMask {
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

स्थिर अन्तःस्थ bool CGROUP_WEIGHT_IS_OK(uint64_t x) {
        पटिनिवत्त
            x == CGROUP_WEIGHT_INVALID ||
            (x >= CGROUP_WEIGHT_MIN && x <= CGROUP_WEIGHT_MAX);
}

/* IO limits on unified hierarchy */
प्रकारनिर्देश गणना CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        _CGROUP_IO_LIMIT_TYPE_MAX,
        _CGROUP_IO_LIMIT_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

बाह्य ध्रुव uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX];

DECLARE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);
शून्य cgroup_io_limits_list(शून्य);

/* Special values for the io.bfq.weight attribute */
#define CGROUP_BFQ_WEIGHT_INVALID UINT64_MAX
#define CGROUP_BFQ_WEIGHT_MIN UINT64_C(1)
#define CGROUP_BFQ_WEIGHT_MAX UINT64_C(1000)
#define CGROUP_BFQ_WEIGHT_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
स्थिर अन्तःस्थ uint64_t BFQ_WEIGHT(uint64_t io_weight) {
        पटिनिवत्त
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

पूर्णाङ्क cg_is_available(शून्य);

पूर्णाङ्क cg_path_open(ध्रुव वर्ण *path);
पूर्णाङ्क cg_cgroupid_open(पूर्णाङ्क cgroupfs_fd, uint64_t id);

पूर्णाङ्क cg_path_from_cgroupid(पूर्णाङ्क cgroupfs_fd, uint64_t id, वर्ण **ret);

प्रकारनिर्देश गणना CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

पूर्णाङ्क cg_enumerate_processes(ध्रुव वर्ण *path, FILE **ret);
पूर्णाङ्क cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
पूर्णाङ्क cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

पूर्णाङ्क cg_enumerate_subgroups(ध्रुव वर्ण *path, DIR **ret);
पूर्णाङ्क cg_read_subgroup(DIR *d, वर्ण **ret);

प्रकारनिर्देश पूर्णाङ्क (*cg_kill_log_func_t)(ध्रुव PidRef *pid, पूर्णाङ्क sig, शून्य *userdata);

पूर्णाङ्क cg_kill(ध्रुव वर्ण *path, पूर्णाङ्क sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, शून्य *userdata);
पूर्णाङ्क cg_kill_kernel_sigkill(ध्रुव वर्ण *path, uint64_t *ret_n_pids_killed);
पूर्णाङ्क cg_kill_recursive(ध्रुव वर्ण *path, पूर्णाङ्क sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, शून्य *userdata);

पूर्णाङ्क cg_split_spec(ध्रुव वर्ण *spec, वर्ण **ret_controller, वर्ण **ret_path);

पूर्णाङ्क cg_get_path(ध्रुव वर्ण *path, ध्रुव वर्ण *suffix, वर्ण **ret);

पूर्णाङ्क cg_pid_get_path(pid_t pid, वर्ण **ret);
पूर्णाङ्क cg_pidref_get_path(ध्रुव PidRef *pidref, वर्ण **ret);

पूर्णाङ्क cg_is_threaded(ध्रुव वर्ण *path);

पूर्णाङ्क cg_is_delegated(ध्रुव वर्ण *path);
पूर्णाङ्क cg_is_delegated_fd(पूर्णाङ्क fd);

पूर्णाङ्क cg_has_coredump_receive(ध्रुव वर्ण *path);

पूर्णाङ्क cg_set_attribute(ध्रुव वर्ण *path, ध्रुव वर्ण *attribute, ध्रुव वर्ण *मानम्);
पूर्णाङ्क cg_get_attribute(ध्रुव वर्ण *path, ध्रुव वर्ण *attribute, वर्ण **ret);
पूर्णाङ्क cg_get_attribute_as_uint64(ध्रुव वर्ण *path, ध्रुव वर्ण *attribute, uint64_t *ret);
पूर्णाङ्क cg_get_attribute_as_bool(ध्रुव वर्ण *path, ध्रुव वर्ण *attribute);

पूर्णाङ्क cg_get_keyed_attribute(ध्रुव वर्ण *path, ध्रुव वर्ण *attribute, वर्ण * ध्रुव *कुञ्चिकाः, वर्ण **values);
पूर्णाङ्क cg_get_keyed_attribute_uint64(ध्रुव वर्ण *path, ध्रुव वर्ण *attribute, ध्रुव वर्ण *कुञ्चिका, uint64_t *ret);

पूर्णाङ्क cg_get_owner(ध्रुव वर्ण *path, uid_t *ret_uid);

पूर्णाङ्क cg_set_xattr(ध्रुव वर्ण *path, ध्रुव वर्ण *name, ध्रुव शून्य *मानम्, size_t size, पूर्णाङ्क flags);
पूर्णाङ्क cg_get_xattr(ध्रुव वर्ण *path, ध्रुव वर्ण *name, वर्ण **ret, size_t *ret_size);
/* Returns negative on error, and 0 or 1 on success for the bool value */
पूर्णाङ्क cg_get_xattr_bool(ध्रुव वर्ण *path, ध्रुव वर्ण *name);
पूर्णाङ्क cg_remove_xattr(ध्रुव वर्ण *path, ध्रुव वर्ण *name);

पूर्णाङ्क cg_is_empty(ध्रुव वर्ण *path);

पूर्णाङ्क cg_get_मूलम्_path(वर्ण **path);

पूर्णाङ्क cg_path_get_session(ध्रुव वर्ण *path, वर्ण **ret_session);
पूर्णाङ्क cg_path_get_owner_uid(ध्रुव वर्ण *path, uid_t *ret_uid);
पूर्णाङ्क cg_path_get_unit_full(ध्रुव वर्ण *path, वर्ण **ret_unit, वर्ण **ret_subgroup);
स्थिर अन्तःस्थ पूर्णाङ्क cg_path_get_unit(ध्रुव वर्ण *path, वर्ण **ret_unit) {
        पटिनिवत्त cg_path_get_unit_full(path, ret_unit, रिक्त);
}
पूर्णाङ्क cg_path_get_unit_path(ध्रुव वर्ण *path, वर्ण **ret_unit);
पूर्णाङ्क cg_path_get_user_unit_full(ध्रुव वर्ण *path, वर्ण **ret_unit, वर्ण **ret_subgroup);
स्थिर अन्तःस्थ पूर्णाङ्क cg_path_get_user_unit(ध्रुव वर्ण *path, वर्ण **ret_unit) {
        पटिनिवत्त cg_path_get_user_unit_full(path, ret_unit, रिक्त);
}
पूर्णाङ्क cg_path_get_machine_name(ध्रुव वर्ण *path, वर्ण **ret_machine);
पूर्णाङ्क cg_path_get_slice(ध्रुव वर्ण *path, वर्ण **ret_slice);
पूर्णाङ्क cg_path_get_user_slice(ध्रुव वर्ण *path, वर्ण **ret_slice);

पूर्णाङ्क cg_shift_path(ध्रुव वर्ण *cgroup, ध्रुव वर्ण *cached_मूलम्, ध्रुव वर्ण **ret_shifted);
पूर्णाङ्क cg_pid_get_path_shifted(pid_t pid, ध्रुव वर्ण *cached_मूलम्, वर्ण **ret_cgroup);
पूर्णाङ्क cg_pidref_get_path_shifted(ध्रुव PidRef *pidref, ध्रुव वर्ण *cached_मूलम्, वर्ण **ret_cgroup);

पूर्णाङ्क cg_pid_get_session(pid_t pid, वर्ण **ret_session);
पूर्णाङ्क cg_pidref_get_session(ध्रुव PidRef *pidref, वर्ण **ret);
पूर्णाङ्क cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
पूर्णाङ्क cg_pidref_get_owner_uid(ध्रुव PidRef *pidref, uid_t *ret);
पूर्णाङ्क cg_pid_get_unit_full(pid_t pid, वर्ण **ret_unit, वर्ण **ret_subgroup);
स्थिर अन्तःस्थ पूर्णाङ्क cg_pid_get_unit(pid_t pid, वर्ण **ret_unit) {
        पटिनिवत्त cg_pid_get_unit_full(pid, ret_unit, रिक्त);
}
पूर्णाङ्क cg_pidref_get_unit_full(ध्रुव PidRef *pidref, वर्ण **ret_unit, वर्ण **ret_subgroup);
स्थिर अन्तःस्थ पूर्णाङ्क cg_pidref_get_unit(ध्रुव PidRef *pidref, वर्ण **ret_unit) {
        पटिनिवत्त cg_pidref_get_unit_full(pidref, ret_unit, रिक्त);
}
पूर्णाङ्क cg_pid_get_user_unit_full(pid_t pid, वर्ण **ret_unit, वर्ण **ret_subgroup);
स्थिर अन्तःस्थ पूर्णाङ्क cg_pid_get_user_unit(pid_t pid, वर्ण **ret_unit) {
        पटिनिवत्त cg_pid_get_user_unit_full(pid, ret_unit, रिक्त);
}
पूर्णाङ्क cg_pidref_get_user_unit_full(ध्रुव PidRef *pidref, वर्ण **ret_unit, वर्ण **ret_subgroup);
स्थिर अन्तःस्थ पूर्णाङ्क cg_pidref_get_user_unit(ध्रुव PidRef *pidref, वर्ण **ret_unit) {
        पटिनिवत्त cg_pidref_get_user_unit_full(pidref, ret_unit, रिक्त);
}
पूर्णाङ्क cg_pid_get_machine_name(pid_t pid, वर्ण **ret_machine);
पूर्णाङ्क cg_pid_get_slice(pid_t pid, वर्ण **ret_slice);
पूर्णाङ्क cg_pid_get_user_slice(pid_t pid, वर्ण **ret_slice);

पूर्णाङ्क cg_path_decode_unit(ध्रुव वर्ण *cgroup, वर्ण **ret_unit);

bool cg_needs_escape(ध्रुव वर्ण *p) _pure_;
पूर्णाङ्क cg_escape(ध्रुव वर्ण *p, वर्ण **ret);
वर्ण* cg_unescape(ध्रुव वर्ण *p) _pure_;

पूर्णाङ्क cg_slice_to_path(ध्रुव वर्ण *unit, वर्ण **ret);

पूर्णाङ्क cg_mask_supported(CGroupMask *ret);
पूर्णाङ्क cg_mask_supported_subtree(ध्रुव वर्ण *मूलम्, CGroupMask *ret);
पूर्णाङ्क cg_mask_from_string(ध्रुव वर्ण *s, CGroupMask *ret);
पूर्णाङ्क cg_mask_to_string(CGroupMask mask, वर्ण **ret);

bool cg_kill_supported(शून्य);

DECLARE_STRING_TABLE_LOOKUP(cgroup_controller, CGroupController);

प्रकारनिर्देश गणना ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        _MANAGED_OOM_MODE_MAX,
        _MANAGED_OOM_MODE_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

प्रकारनिर्देश गणना ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);

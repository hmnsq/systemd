/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

#घोषणा SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
प्रकार क्रमागत CGroupController {
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

#घोषणा CGROUP_CONTROLLER_TO_MASK(c) (1U << (c))

/* A bit mask of well known cgroup controllers */
प्रकार क्रमागत CGroupMask {
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
#घोषणा CGROUP_WEIGHT_INVALID UINT64_MAX
#घोषणा CGROUP_WEIGHT_IDLE UINT64_C(0)
#घोषणा CGROUP_WEIGHT_MIN UINT64_C(1)
#घोषणा CGROUP_WEIGHT_MAX UINT64_C(10000)
#घोषणा CGROUP_WEIGHT_DEFAULT UINT64_C(100)

#घोषणा CGROUP_LIMIT_MIN UINT64_C(0)
#घोषणा CGROUP_LIMIT_MAX UINT64_MAX

अटल अंतरभूत bool CGROUP_WEIGHT_IS_OK(uint64_t x) {
        वापस
            x == CGROUP_WEIGHT_INVALID ||
            (x >= CGROUP_WEIGHT_MIN && x <= CGROUP_WEIGHT_MAX);
}

/* IO limits on unified hierarchy */
प्रकार क्रमागत CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        _CGROUP_IO_LIMIT_TYPE_MAX,
        _CGROUP_IO_LIMIT_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

बाह्य स्थिर uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX];

DECLARE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);
व्योम cgroup_io_limits_list(व्योम);

/* Special values for the io.bfq.weight attribute */
#घोषणा CGROUP_BFQ_WEIGHT_INVALID UINT64_MAX
#घोषणा CGROUP_BFQ_WEIGHT_MIN UINT64_C(1)
#घोषणा CGROUP_BFQ_WEIGHT_MAX UINT64_C(1000)
#घोषणा CGROUP_BFQ_WEIGHT_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
अटल अंतरभूत uint64_t BFQ_WEIGHT(uint64_t io_weight) {
        वापस
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

पूर्णांक cg_is_available(व्योम);

पूर्णांक cg_path_open(स्थिर अक्षर *path);
पूर्णांक cg_cgroupid_open(पूर्णांक cgroupfs_fd, uint64_t id);

पूर्णांक cg_path_from_cgroupid(पूर्णांक cgroupfs_fd, uint64_t id, अक्षर **ret);

प्रकार क्रमागत CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

पूर्णांक cg_enumerate_processes(स्थिर अक्षर *path, खाता **ret);
पूर्णांक cg_read_pid(खाता *f, pid_t *ret, CGroupFlags flags);
पूर्णांक cg_read_pidref(खाता *f, PidRef *ret, CGroupFlags flags);

पूर्णांक cg_enumerate_subgroups(स्थिर अक्षर *path, सूची **ret);
पूर्णांक cg_read_subgroup(सूची *d, अक्षर **ret);

प्रकार पूर्णांक (*cg_kill_log_func_t)(स्थिर PidRef *pid, पूर्णांक sig, व्योम *userdata);

पूर्णांक cg_kill(स्थिर अक्षर *path, पूर्णांक sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, व्योम *userdata);
पूर्णांक cg_kill_kernel_sigkill(स्थिर अक्षर *path, uint64_t *ret_n_pids_killed);
पूर्णांक cg_kill_recursive(स्थिर अक्षर *path, पूर्णांक sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, व्योम *userdata);

पूर्णांक cg_split_spec(स्थिर अक्षर *spec, अक्षर **ret_controller, अक्षर **ret_path);

पूर्णांक cg_get_path(स्थिर अक्षर *path, स्थिर अक्षर *suffix, अक्षर **ret);

पूर्णांक cg_pid_get_path(pid_t pid, अक्षर **ret);
पूर्णांक cg_pidref_get_path(स्थिर PidRef *pidref, अक्षर **ret);

पूर्णांक cg_is_threaded(स्थिर अक्षर *path);

पूर्णांक cg_is_delegated(स्थिर अक्षर *path);
पूर्णांक cg_is_delegated_fd(पूर्णांक fd);

पूर्णांक cg_has_coredump_receive(स्थिर अक्षर *path);

पूर्णांक cg_set_attribute(स्थिर अक्षर *path, स्थिर अक्षर *attribute, स्थिर अक्षर *मूल्य);
पूर्णांक cg_get_attribute(स्थिर अक्षर *path, स्थिर अक्षर *attribute, अक्षर **ret);
पूर्णांक cg_get_attribute_as_uint64(स्थिर अक्षर *path, स्थिर अक्षर *attribute, uint64_t *ret);
पूर्णांक cg_get_attribute_as_bool(स्थिर अक्षर *path, स्थिर अक्षर *attribute);

पूर्णांक cg_get_keyed_attribute(स्थिर अक्षर *path, स्थिर अक्षर *attribute, अक्षर * स्थिर *कुंजियाँ, अक्षर **values);
पूर्णांक cg_get_keyed_attribute_uint64(स्थिर अक्षर *path, स्थिर अक्षर *attribute, स्थिर अक्षर *कुंजी, uint64_t *ret);

पूर्णांक cg_get_owner(स्थिर अक्षर *path, uid_t *ret_uid);

पूर्णांक cg_set_xattr(स्थिर अक्षर *path, स्थिर अक्षर *name, स्थिर व्योम *मूल्य, माप_प्रकार आकार, पूर्णांक flags);
पूर्णांक cg_get_xattr(स्थिर अक्षर *path, स्थिर अक्षर *name, अक्षर **ret, माप_प्रकार *ret_आकार);
/* Returns negative on error, and 0 or 1 on success for the bool value */
पूर्णांक cg_get_xattr_bool(स्थिर अक्षर *path, स्थिर अक्षर *name);
पूर्णांक cg_remove_xattr(स्थिर अक्षर *path, स्थिर अक्षर *name);

पूर्णांक cg_is_empty(स्थिर अक्षर *path);

पूर्णांक cg_get_मूल_path(अक्षर **path);

पूर्णांक cg_path_get_session(स्थिर अक्षर *path, अक्षर **ret_session);
पूर्णांक cg_path_get_owner_uid(स्थिर अक्षर *path, uid_t *ret_uid);
पूर्णांक cg_path_get_unit_full(स्थिर अक्षर *path, अक्षर **ret_unit, अक्षर **ret_subgroup);
अटल अंतरभूत पूर्णांक cg_path_get_unit(स्थिर अक्षर *path, अक्षर **ret_unit) {
        वापस cg_path_get_unit_full(path, ret_unit, शून्य);
}
पूर्णांक cg_path_get_unit_path(स्थिर अक्षर *path, अक्षर **ret_unit);
पूर्णांक cg_path_get_user_unit_full(स्थिर अक्षर *path, अक्षर **ret_unit, अक्षर **ret_subgroup);
अटल अंतरभूत पूर्णांक cg_path_get_user_unit(स्थिर अक्षर *path, अक्षर **ret_unit) {
        वापस cg_path_get_user_unit_full(path, ret_unit, शून्य);
}
पूर्णांक cg_path_get_machine_name(स्थिर अक्षर *path, अक्षर **ret_machine);
पूर्णांक cg_path_get_slice(स्थिर अक्षर *path, अक्षर **ret_slice);
पूर्णांक cg_path_get_user_slice(स्थिर अक्षर *path, अक्षर **ret_slice);

पूर्णांक cg_shift_path(स्थिर अक्षर *cgroup, स्थिर अक्षर *cached_मूल, स्थिर अक्षर **ret_shifted);
पूर्णांक cg_pid_get_path_shifted(pid_t pid, स्थिर अक्षर *cached_मूल, अक्षर **ret_cgroup);
पूर्णांक cg_pidref_get_path_shifted(स्थिर PidRef *pidref, स्थिर अक्षर *cached_मूल, अक्षर **ret_cgroup);

पूर्णांक cg_pid_get_session(pid_t pid, अक्षर **ret_session);
पूर्णांक cg_pidref_get_session(स्थिर PidRef *pidref, अक्षर **ret);
पूर्णांक cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
पूर्णांक cg_pidref_get_owner_uid(स्थिर PidRef *pidref, uid_t *ret);
पूर्णांक cg_pid_get_unit_full(pid_t pid, अक्षर **ret_unit, अक्षर **ret_subgroup);
अटल अंतरभूत पूर्णांक cg_pid_get_unit(pid_t pid, अक्षर **ret_unit) {
        वापस cg_pid_get_unit_full(pid, ret_unit, शून्य);
}
पूर्णांक cg_pidref_get_unit_full(स्थिर PidRef *pidref, अक्षर **ret_unit, अक्षर **ret_subgroup);
अटल अंतरभूत पूर्णांक cg_pidref_get_unit(स्थिर PidRef *pidref, अक्षर **ret_unit) {
        वापस cg_pidref_get_unit_full(pidref, ret_unit, शून्य);
}
पूर्णांक cg_pid_get_user_unit_full(pid_t pid, अक्षर **ret_unit, अक्षर **ret_subgroup);
अटल अंतरभूत पूर्णांक cg_pid_get_user_unit(pid_t pid, अक्षर **ret_unit) {
        वापस cg_pid_get_user_unit_full(pid, ret_unit, शून्य);
}
पूर्णांक cg_pidref_get_user_unit_full(स्थिर PidRef *pidref, अक्षर **ret_unit, अक्षर **ret_subgroup);
अटल अंतरभूत पूर्णांक cg_pidref_get_user_unit(स्थिर PidRef *pidref, अक्षर **ret_unit) {
        वापस cg_pidref_get_user_unit_full(pidref, ret_unit, शून्य);
}
पूर्णांक cg_pid_get_machine_name(pid_t pid, अक्षर **ret_machine);
पूर्णांक cg_pid_get_slice(pid_t pid, अक्षर **ret_slice);
पूर्णांक cg_pid_get_user_slice(pid_t pid, अक्षर **ret_slice);

पूर्णांक cg_path_decode_unit(स्थिर अक्षर *cgroup, अक्षर **ret_unit);

bool cg_needs_escape(स्थिर अक्षर *p) _pure_;
पूर्णांक cg_escape(स्थिर अक्षर *p, अक्षर **ret);
अक्षर* cg_unescape(स्थिर अक्षर *p) _pure_;

पूर्णांक cg_slice_to_path(स्थिर अक्षर *unit, अक्षर **ret);

पूर्णांक cg_mask_supported(CGroupMask *ret);
पूर्णांक cg_mask_supported_subtree(स्थिर अक्षर *मूल, CGroupMask *ret);
पूर्णांक cg_mask_from_string(स्थिर अक्षर *s, CGroupMask *ret);
पूर्णांक cg_mask_to_string(CGroupMask mask, अक्षर **ret);

bool cg_kill_supported(व्योम);

DECLARE_STRING_TABLE_LOOKUP(cgroup_controller, CGroupController);

प्रकार क्रमागत ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        _MANAGED_OOM_MODE_MAX,
        _MANAGED_OOM_MODE_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

प्रकार क्रमागत ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);

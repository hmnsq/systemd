/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ CGroupController {
        /* Original cgroup controllers */
        CGROUP_CONTROLLER_CPU,
        CGROUP_CONTROLLER_CPUACCT,    /* v1 only */
        CGROUP_CONTROLLER_CPUSET,     /* v2 only */
        CGROUP_CONTROLLER_IO,         /* v2 only */
        CGROUP_CONTROLLER_BLKIO,      /* v1 only */
        CGROUP_CONTROLLER_ਸਮ੍ਰਿਤੀ,
        CGROUP_CONTROLLER_DEVICES,    /* v1 only */
        CGROUP_CONTROLLER_PIDS,

        /* BPF-based pseudo-controllers, v2 only */
        CGROUP_CONTROLLER_BPF_FIREWALL,
        CGROUP_CONTROLLER_BPF_DEVICES,
        CGROUP_CONTROLLER_BPF_FOREIGN,
        CGROUP_CONTROLLER_BPF_SOCKET_BIND,
        CGROUP_CONTROLLER_BPF_RESTRICT_ਜਾਲ_INTERFACES,
        CGROUP_CONTROLLER_BPF_BIND_ਜਾਲ_INTERFACE,
        /* The BPF hook implementing RestrictFileSystems= is not defined here.
         * It's applied as late as possible in exec_invoke() so we don't block
         * our own unit setup code. */

        _CGROUP_CONTROLLER_MAX,
        _CGROUP_CONTROLLER_INVALID = -EINVAL,
} CGroupController;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(c) (1U << (c))

/* A bit mask of well known cgroup controllers */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ CGroup_ਨਕਾਬ {
        CGROUP_ਨਕਾਬ_CPU = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(CGROUP_CONTROLLER_CPU),
        CGROUP_ਨਕਾਬ_CPUACCT = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(CGROUP_CONTROLLER_CPUACCT),
        CGROUP_ਨਕਾਬ_CPUSET = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(CGROUP_CONTROLLER_CPUSET),
        CGROUP_ਨਕਾਬ_IO = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(CGROUP_CONTROLLER_IO),
        CGROUP_ਨਕਾਬ_BLKIO = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(CGROUP_CONTROLLER_BLKIO),
        CGROUP_ਨਕਾਬ_ਸਮ੍ਰਿਤੀ = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(CGROUP_CONTROLLER_ਸਮ੍ਰਿਤੀ),
        CGROUP_ਨਕਾਬ_DEVICES = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(CGROUP_CONTROLLER_DEVICES),
        CGROUP_ਨਕਾਬ_PIDS = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(CGROUP_CONTROLLER_PIDS),
        CGROUP_ਨਕਾਬ_BPF_FIREWALL = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(CGROUP_CONTROLLER_BPF_FIREWALL),
        CGROUP_ਨਕਾਬ_BPF_DEVICES = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(CGROUP_CONTROLLER_BPF_DEVICES),
        CGROUP_ਨਕਾਬ_BPF_FOREIGN = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(CGROUP_CONTROLLER_BPF_FOREIGN),
        CGROUP_ਨਕਾਬ_BPF_SOCKET_BIND = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(CGROUP_CONTROLLER_BPF_SOCKET_BIND),
        CGROUP_ਨਕਾਬ_BPF_RESTRICT_ਜਾਲ_INTERFACES = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(CGROUP_CONTROLLER_BPF_RESTRICT_ਜਾਲ_INTERFACES),
        CGROUP_ਨਕਾਬ_BPF_BIND_ਜਾਲ_INTERFACE = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(CGROUP_CONTROLLER_BPF_BIND_ਜਾਲ_INTERFACE),

        /* All real cgroup v1 controllers */
        CGROUP_ਨਕਾਬ_V1 = CGROUP_ਨਕਾਬ_CPU|CGROUP_ਨਕਾਬ_CPUACCT|CGROUP_ਨਕਾਬ_BLKIO|CGROUP_ਨਕਾਬ_ਸਮ੍ਰਿਤੀ|CGROUP_ਨਕਾਬ_DEVICES|CGROUP_ਨਕਾਬ_PIDS,

        /* All real cgroup v2 controllers */
        CGROUP_ਨਕਾਬ_V2 = CGROUP_ਨਕਾਬ_CPU|CGROUP_ਨਕਾਬ_CPUSET|CGROUP_ਨਕਾਬ_IO|CGROUP_ਨਕਾਬ_ਸਮ੍ਰਿਤੀ|CGROUP_ਨਕਾਬ_PIDS,

        /* All controllers we want to delegate in case of Delegate=yes. Which are pretty much the v2 controllers only, as delegation on v1 is not safe, and bpf stuff isn't a real controller */
        CGROUP_ਨਕਾਬ_DELEGATE = CGROUP_ਨਕਾਬ_V2,

        /* All cgroup v2 BPF pseudo-controllers */
        CGROUP_ਨਕਾਬ_BPF = CGROUP_ਨਕਾਬ_BPF_FIREWALL|CGROUP_ਨਕਾਬ_BPF_DEVICES|CGROUP_ਨਕਾਬ_BPF_FOREIGN|CGROUP_ਨਕਾਬ_BPF_SOCKET_BIND|CGROUP_ਨਕਾਬ_BPF_RESTRICT_ਜਾਲ_INTERFACES|CGROUP_ਨਕਾਬ_BPF_BIND_ਜਾਲ_INTERFACE,

        CGROUP_ਨਕਾਬ_ALL = CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(_CGROUP_CONTROLLER_MAX) - 1,
} CGroup_ਨਕਾਬ;

/* Special values for all weight knobs on unified hierarchy */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CGROUP_ਭਾਰ_INVALID UINT64_MAX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CGROUP_ਭਾਰ_IDLE UINT64_C(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CGROUP_ਭਾਰ_MIN UINT64_C(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CGROUP_ਭਾਰ_MAX UINT64_C(10000)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CGROUP_ਭਾਰ_DEFAULT UINT64_C(100)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CGROUP_ਹੱਦਬੰਦੀ_MIN UINT64_C(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CGROUP_ਹੱਦਬੰਦੀ_MAX UINT64_MAX

ਸਥਿਰ ਸਤਰਅੰਦਰ bool CGROUP_ਭਾਰ_IS_OK(uint64_t x) {
        ਵਾਪਸ
            x == CGROUP_ਭਾਰ_INVALID ||
            (x >= CGROUP_ਭਾਰ_MIN && x <= CGROUP_ਭਾਰ_MAX);
}

/* IO limits on unified hierarchy */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        CGROUP_IO_ਹੱਦਬੰਦੀ_TYPE_MAX,
        CGROUP_IO_ਹੱਦਬੰਦੀ_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

ਬਾਹਰੀ ਅਚਲ uint64_t cgroup_io_ਹੱਦਬੰਦੀ_defaults[CGROUP_IO_ਹੱਦਬੰਦੀ_TYPE_MAX];

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(cgroup_io_ਹੱਦਬੰਦੀ_type, CGroupIOLimitType);
ਖਾਲੀ cgroup_io_limits_list(ਖਾਲੀ);

/* Special values for the io.bfq.weight attribute */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CGROUP_BFQ_ਭਾਰ_INVALID UINT64_MAX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CGROUP_BFQ_ਭਾਰ_MIN UINT64_C(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CGROUP_BFQ_ਭਾਰ_MAX UINT64_C(1000)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CGROUP_BFQ_ਭਾਰ_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t BFQ_ਭਾਰ(uint64_t io_ਭਾਰ) {
        ਵਾਪਸ
            io_ਭਾਰ <= CGROUP_ਭਾਰ_DEFAULT ?
            CGROUP_BFQ_ਭਾਰ_DEFAULT - (CGROUP_ਭਾਰ_DEFAULT - io_ਭਾਰ) * (CGROUP_BFQ_ਭਾਰ_DEFAULT - CGROUP_BFQ_ਭਾਰ_MIN) / (CGROUP_ਭਾਰ_DEFAULT - CGROUP_ਭਾਰ_MIN) :
            CGROUP_BFQ_ਭਾਰ_DEFAULT + (io_ਭਾਰ - CGROUP_ਭਾਰ_DEFAULT) * (CGROUP_BFQ_ਭਾਰ_MAX - CGROUP_BFQ_ਭਾਰ_DEFAULT) / (CGROUP_ਭਾਰ_MAX - CGROUP_ਭਾਰ_DEFAULT);
}

/*
 * General rules:
 *
 * We require absolute cgroup paths. When returning, we will always
 * generate paths with multiple adjacent / removed.
 */

ਪੂਰਨ cg_is_ਉਪਲਬਧ(ਖਾਲੀ);

ਪੂਰਨ cg_ਰਸਤਾ_open(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);
ਪੂਰਨ cg_cgroupid_open(ਪੂਰਨ cgroupfs_fd, uint64_t id);

ਪੂਰਨ cg_ਰਸਤਾ_from_cgroupid(ਪੂਰਨ cgroupfs_fd, uint64_t id, ਅੱਖਰ **ret);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

ਪੂਰਨ cg_enumerate_processes(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, FILE **ret);
ਪੂਰਨ cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
ਪੂਰਨ cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

ਪੂਰਨ cg_enumerate_subgroups(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, DIR **ret);
ਪੂਰਨ cg_read_subgroup(DIR *d, ਅੱਖਰ **ret);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*cg_kill_log_func_t)(ਅਚਲ PidRef *pid, ਪੂਰਨ sig, ਖਾਲੀ *userdata);

ਪੂਰਨ cg_kill(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ sig, CGroupFlags flags, ਸੈੱਟ_2 *killed_pids, cg_kill_log_func_t log_kill, ਖਾਲੀ *userdata);
ਪੂਰਨ cg_kill_kernel_sigkill(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, uint64_t *ret_n_pids_killed);
ਪੂਰਨ cg_kill_recursive(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ sig, CGroupFlags flags, ਸੈੱਟ_2 *killed_pids, cg_kill_log_func_t log_kill, ਖਾਲੀ *userdata);

ਪੂਰਨ cg_ਵੰਡੋ_spec(ਅਚਲ ਅੱਖਰ *spec, ਅੱਖਰ **ret_controller, ਅੱਖਰ **ret_ਰਸਤਾ);

ਪੂਰਨ cg_get_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *suffix, ਅੱਖਰ **ret);

ਪੂਰਨ cg_pid_get_ਰਸਤਾ(pid_t pid, ਅੱਖਰ **ret);
ਪੂਰਨ cg_pidref_get_ਰਸਤਾ(ਅਚਲ PidRef *pidref, ਅੱਖਰ **ret);

ਪੂਰਨ cg_is_threaded(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);

ਪੂਰਨ cg_is_delegated(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);
ਪੂਰਨ cg_is_delegated_fd(ਪੂਰਨ fd);

ਪੂਰਨ cg_has_coredump_receive(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);

ਪੂਰਨ cg_ਸੈੱਟ_attribute(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *attribute, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);
ਪੂਰਨ cg_get_attribute(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *attribute, ਅੱਖਰ **ret);
ਪੂਰਨ cg_get_attribute_as_uint64(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *attribute, uint64_t *ret);
ਪੂਰਨ cg_get_attribute_as_bool(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *attribute);

ਪੂਰਨ cg_get_keyed_attribute(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *attribute, ਅੱਖਰ * ਅਚਲ *ਕੁੰਜੀਆਂ, ਅੱਖਰ **values);
ਪੂਰਨ cg_get_keyed_attribute_uint64(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *attribute, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, uint64_t *ret);

ਪੂਰਨ cg_get_owner(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, uid_t *ret_uid);

ਪੂਰਨ cg_ਸੈੱਟ_xattr(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਖਾਲੀ *ਮੁੱਲ, size_t ਮਾਪ, ਪੂਰਨ flags);
ਪੂਰਨ cg_get_xattr(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ret, size_t *ret_ਮਾਪ);
/* Returns negative on error, and 0 or 1 on success for the bool value */
ਪੂਰਨ cg_get_xattr_bool(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name);
ਪੂਰਨ cg_remove_xattr(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name);

ਪੂਰਨ cg_is_ਖਾਲੀ_ਥਾਵਾਂ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);

ਪੂਰਨ cg_get_ਜੜ੍ਹ_ਰਸਤਾ(ਅੱਖਰ **ਰਸਤਾ);

ਪੂਰਨ cg_ਰਸਤਾ_get_session(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_session);
ਪੂਰਨ cg_ਰਸਤਾ_get_owner_uid(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, uid_t *ret_uid);
ਪੂਰਨ cg_ਰਸਤਾ_get_unit_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_unit, ਅੱਖਰ **ret_subgroup);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ cg_ਰਸਤਾ_get_unit(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_unit) {
        ਵਾਪਸ cg_ਰਸਤਾ_get_unit_ਭਰਿਆ(ਰਸਤਾ, ret_unit, ਨਿਰਾ);
}
ਪੂਰਨ cg_ਰਸਤਾ_get_unit_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_unit);
ਪੂਰਨ cg_ਰਸਤਾ_get_user_unit_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_unit, ਅੱਖਰ **ret_subgroup);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ cg_ਰਸਤਾ_get_user_unit(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_unit) {
        ਵਾਪਸ cg_ਰਸਤਾ_get_user_unit_ਭਰਿਆ(ਰਸਤਾ, ret_unit, ਨਿਰਾ);
}
ਪੂਰਨ cg_ਰਸਤਾ_get_machine_name(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_machine);
ਪੂਰਨ cg_ਰਸਤਾ_get_ਟੁਕੜਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_ਟੁਕੜਾ);
ਪੂਰਨ cg_ਰਸਤਾ_get_user_ਟੁਕੜਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_ਟੁਕੜਾ);

ਪੂਰਨ cg_ਖਿਸਕਾਓ_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *cgroup, ਅਚਲ ਅੱਖਰ *cached_ਜੜ੍ਹ, ਅਚਲ ਅੱਖਰ **ret_shifted);
ਪੂਰਨ cg_pid_get_ਰਸਤਾ_shifted(pid_t pid, ਅਚਲ ਅੱਖਰ *cached_ਜੜ੍ਹ, ਅੱਖਰ **ret_cgroup);
ਪੂਰਨ cg_pidref_get_ਰਸਤਾ_shifted(ਅਚਲ PidRef *pidref, ਅਚਲ ਅੱਖਰ *cached_ਜੜ੍ਹ, ਅੱਖਰ **ret_cgroup);

ਪੂਰਨ cg_pid_get_session(pid_t pid, ਅੱਖਰ **ret_session);
ਪੂਰਨ cg_pidref_get_session(ਅਚਲ PidRef *pidref, ਅੱਖਰ **ret);
ਪੂਰਨ cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
ਪੂਰਨ cg_pidref_get_owner_uid(ਅਚਲ PidRef *pidref, uid_t *ret);
ਪੂਰਨ cg_pid_get_unit_ਭਰਿਆ(pid_t pid, ਅੱਖਰ **ret_unit, ਅੱਖਰ **ret_subgroup);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ cg_pid_get_unit(pid_t pid, ਅੱਖਰ **ret_unit) {
        ਵਾਪਸ cg_pid_get_unit_ਭਰਿਆ(pid, ret_unit, ਨਿਰਾ);
}
ਪੂਰਨ cg_pidref_get_unit_ਭਰਿਆ(ਅਚਲ PidRef *pidref, ਅੱਖਰ **ret_unit, ਅੱਖਰ **ret_subgroup);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ cg_pidref_get_unit(ਅਚਲ PidRef *pidref, ਅੱਖਰ **ret_unit) {
        ਵਾਪਸ cg_pidref_get_unit_ਭਰਿਆ(pidref, ret_unit, ਨਿਰਾ);
}
ਪੂਰਨ cg_pid_get_user_unit_ਭਰਿਆ(pid_t pid, ਅੱਖਰ **ret_unit, ਅੱਖਰ **ret_subgroup);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ cg_pid_get_user_unit(pid_t pid, ਅੱਖਰ **ret_unit) {
        ਵਾਪਸ cg_pid_get_user_unit_ਭਰਿਆ(pid, ret_unit, ਨਿਰਾ);
}
ਪੂਰਨ cg_pidref_get_user_unit_ਭਰਿਆ(ਅਚਲ PidRef *pidref, ਅੱਖਰ **ret_unit, ਅੱਖਰ **ret_subgroup);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ cg_pidref_get_user_unit(ਅਚਲ PidRef *pidref, ਅੱਖਰ **ret_unit) {
        ਵਾਪਸ cg_pidref_get_user_unit_ਭਰਿਆ(pidref, ret_unit, ਨਿਰਾ);
}
ਪੂਰਨ cg_pid_get_machine_name(pid_t pid, ਅੱਖਰ **ret_machine);
ਪੂਰਨ cg_pid_get_ਟੁਕੜਾ(pid_t pid, ਅੱਖਰ **ret_ਟੁਕੜਾ);
ਪੂਰਨ cg_pid_get_user_ਟੁਕੜਾ(pid_t pid, ਅੱਖਰ **ret_ਟੁਕੜਾ);

ਪੂਰਨ cg_ਰਸਤਾ_decode_unit(ਅਚਲ ਅੱਖਰ *cgroup, ਅੱਖਰ **ret_unit);

bool cg_needs_escape(ਅਚਲ ਅੱਖਰ *p) ਸ਼ੁੱਧ;
ਪੂਰਨ cg_escape(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ **ret);
ਅੱਖਰ* cg_unescape(ਅਚਲ ਅੱਖਰ *p) ਸ਼ੁੱਧ;

ਪੂਰਨ cg_ਟੁਕੜਾ_ਮੰਜ਼ਿਲ_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *unit, ਅੱਖਰ **ret);

ਪੂਰਨ cg_ਨਕਾਬ_supported(CGroup_ਨਕਾਬ *ret);
ਪੂਰਨ cg_ਨਕਾਬ_supported_subtree(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, CGroup_ਨਕਾਬ *ret);
ਪੂਰਨ cg_ਨਕਾਬ_from_string(ਅਚਲ ਅੱਖਰ *s, CGroup_ਨਕਾਬ *ret);
ਪੂਰਨ cg_ਨਕਾਬ_ਮੰਜ਼ਿਲ_string(CGroup_ਨਕਾਬ ਨਕਾਬ_3, ਅੱਖਰ **ret);

bool cg_kill_supported(ਖਾਲੀ);

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(cgroup_controller, CGroupController);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        MANAGED_OOM_ਬਹੁਲਕ_MAX,
        MANAGED_OOM_ਬਹੁਲਕ_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(managed_oom_ਬਹੁਲਕ, ManagedOOMMode);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(managed_oom_preference, ManagedOOMPreference);

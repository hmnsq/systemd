/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

#ವ್ಯಾಖ್ಯಾನಿಸು SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ CGroupController {
        /* Original cgroup controllers */
        CGROUP_CONTROLLER_CPU,
        CGROUP_CONTROLLER_CPUACCT,    /* v1 only */
        CGROUP_CONTROLLER_CPUSET,     /* v2 only */
        CGROUP_CONTROLLER_IO,         /* v2 only */
        CGROUP_CONTROLLER_BLKIO,      /* v1 only */
        CGROUP_CONTROLLER_ಸ್ಮರಣೆ,
        CGROUP_CONTROLLER_DEVICES,    /* v1 only */
        CGROUP_CONTROLLER_PIDS,

        /* BPF-based pseudo-controllers, v2 only */
        CGROUP_CONTROLLER_BPF_FIREWALL,
        CGROUP_CONTROLLER_BPF_DEVICES,
        CGROUP_CONTROLLER_BPF_FOREIGN,
        CGROUP_CONTROLLER_BPF_SOCKET_BIND,
        CGROUP_CONTROLLER_BPF_RESTRICT_ಜಾಲ_INTERFACES,
        CGROUP_CONTROLLER_BPF_BIND_ಜಾಲ_INTERFACE,
        /* The BPF hook implementing RestrictFileSystems= is not defined here.
         * It's applied as late as possible in exec_invoke() so we don't block
         * our own unit setup code. */

        _CGROUP_CONTROLLER_MAX,
        _CGROUP_CONTROLLER_INVALID = -EINVAL,
} CGroupController;

#ವ್ಯಾಖ್ಯಾನಿಸು CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(c) (1U << (c))

/* A bit mask of well known cgroup controllers */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ CGroup_ಮುಖವಾಡ {
        CGROUP_ಮುಖವಾಡ_CPU = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(CGROUP_CONTROLLER_CPU),
        CGROUP_ಮುಖವಾಡ_CPUACCT = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(CGROUP_CONTROLLER_CPUACCT),
        CGROUP_ಮುಖವಾಡ_CPUSET = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(CGROUP_CONTROLLER_CPUSET),
        CGROUP_ಮುಖವಾಡ_IO = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(CGROUP_CONTROLLER_IO),
        CGROUP_ಮುಖವಾಡ_BLKIO = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(CGROUP_CONTROLLER_BLKIO),
        CGROUP_ಮುಖವಾಡ_ಸ್ಮರಣೆ = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(CGROUP_CONTROLLER_ಸ್ಮರಣೆ),
        CGROUP_ಮುಖವಾಡ_DEVICES = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(CGROUP_CONTROLLER_DEVICES),
        CGROUP_ಮುಖವಾಡ_PIDS = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(CGROUP_CONTROLLER_PIDS),
        CGROUP_ಮುಖವಾಡ_BPF_FIREWALL = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(CGROUP_CONTROLLER_BPF_FIREWALL),
        CGROUP_ಮುಖವಾಡ_BPF_DEVICES = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(CGROUP_CONTROLLER_BPF_DEVICES),
        CGROUP_ಮುಖವಾಡ_BPF_FOREIGN = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(CGROUP_CONTROLLER_BPF_FOREIGN),
        CGROUP_ಮುಖವಾಡ_BPF_SOCKET_BIND = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(CGROUP_CONTROLLER_BPF_SOCKET_BIND),
        CGROUP_ಮುಖವಾಡ_BPF_RESTRICT_ಜಾಲ_INTERFACES = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(CGROUP_CONTROLLER_BPF_RESTRICT_ಜಾಲ_INTERFACES),
        CGROUP_ಮುಖವಾಡ_BPF_BIND_ಜಾಲ_INTERFACE = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(CGROUP_CONTROLLER_BPF_BIND_ಜಾಲ_INTERFACE),

        /* All real cgroup v1 controllers */
        CGROUP_ಮುಖವಾಡ_V1 = CGROUP_ಮುಖವಾಡ_CPU|CGROUP_ಮುಖವಾಡ_CPUACCT|CGROUP_ಮುಖವಾಡ_BLKIO|CGROUP_ಮುಖವಾಡ_ಸ್ಮರಣೆ|CGROUP_ಮುಖವಾಡ_DEVICES|CGROUP_ಮುಖವಾಡ_PIDS,

        /* All real cgroup v2 controllers */
        CGROUP_ಮುಖವಾಡ_V2 = CGROUP_ಮುಖವಾಡ_CPU|CGROUP_ಮುಖವಾಡ_CPUSET|CGROUP_ಮುಖವಾಡ_IO|CGROUP_ಮುಖವಾಡ_ಸ್ಮರಣೆ|CGROUP_ಮುಖವಾಡ_PIDS,

        /* All controllers we want to delegate in case of Delegate=yes. Which are pretty much the v2 controllers only, as delegation on v1 is not safe, and bpf stuff isn't a real controller */
        CGROUP_ಮುಖವಾಡ_DELEGATE = CGROUP_ಮುಖವಾಡ_V2,

        /* All cgroup v2 BPF pseudo-controllers */
        CGROUP_ಮುಖವಾಡ_BPF = CGROUP_ಮುಖವಾಡ_BPF_FIREWALL|CGROUP_ಮುಖವಾಡ_BPF_DEVICES|CGROUP_ಮುಖವಾಡ_BPF_FOREIGN|CGROUP_ಮುಖವಾಡ_BPF_SOCKET_BIND|CGROUP_ಮುಖವಾಡ_BPF_RESTRICT_ಜಾಲ_INTERFACES|CGROUP_ಮುಖವಾಡ_BPF_BIND_ಜಾಲ_INTERFACE,

        CGROUP_ಮುಖವಾಡ_ALL = CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(_CGROUP_CONTROLLER_MAX) - 1,
} CGroup_ಮುಖವಾಡ;

/* Special values for all weight knobs on unified hierarchy */
#ವ್ಯಾಖ್ಯಾನಿಸು CGROUP_ತೂಕ_INVALID UINT64_MAX
#ವ್ಯಾಖ್ಯಾನಿಸು CGROUP_ತೂಕ_IDLE UINT64_C(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CGROUP_ತೂಕ_MIN UINT64_C(1)
#ವ್ಯಾಖ್ಯಾನಿಸು CGROUP_ತೂಕ_MAX UINT64_C(10000)
#ವ್ಯಾಖ್ಯಾನಿಸು CGROUP_ತೂಕ_DEFAULT UINT64_C(100)

#ವ್ಯಾಖ್ಯಾನಿಸು CGROUP_ಎಲ್ಲೆ_MIN UINT64_C(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CGROUP_ಎಲ್ಲೆ_MAX UINT64_MAX

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool CGROUP_ತೂಕ_IS_OK(uint64_t x) {
        ಹಿಂತಿರುಗಿ
            x == CGROUP_ತೂಕ_INVALID ||
            (x >= CGROUP_ತೂಕ_MIN && x <= CGROUP_ತೂಕ_MAX);
}

/* IO limits on unified hierarchy */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        CGROUP_IO_ಎಲ್ಲೆ_TYPE_MAX,
        CGROUP_IO_ಎಲ್ಲೆ_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ uint64_t cgroup_io_ಎಲ್ಲೆ_defaults[CGROUP_IO_ಎಲ್ಲೆ_TYPE_MAX];

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(cgroup_io_ಎಲ್ಲೆ_type, CGroupIOLimitType);
ಶೂನ್ಯ cgroup_io_limits_list(ಶೂನ್ಯ);

/* Special values for the io.bfq.weight attribute */
#ವ್ಯಾಖ್ಯಾನಿಸು CGROUP_BFQ_ತೂಕ_INVALID UINT64_MAX
#ವ್ಯಾಖ್ಯಾನಿಸು CGROUP_BFQ_ತೂಕ_MIN UINT64_C(1)
#ವ್ಯಾಖ್ಯಾನಿಸು CGROUP_BFQ_ತೂಕ_MAX UINT64_C(1000)
#ವ್ಯಾಖ್ಯಾನಿಸು CGROUP_BFQ_ತೂಕ_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t BFQ_ತೂಕ(uint64_t io_ತೂಕ) {
        ಹಿಂತಿರುಗಿ
            io_ತೂಕ <= CGROUP_ತೂಕ_DEFAULT ?
            CGROUP_BFQ_ತೂಕ_DEFAULT - (CGROUP_ತೂಕ_DEFAULT - io_ತೂಕ) * (CGROUP_BFQ_ತೂಕ_DEFAULT - CGROUP_BFQ_ತೂಕ_MIN) / (CGROUP_ತೂಕ_DEFAULT - CGROUP_ತೂಕ_MIN) :
            CGROUP_BFQ_ತೂಕ_DEFAULT + (io_ತೂಕ - CGROUP_ತೂಕ_DEFAULT) * (CGROUP_BFQ_ತೂಕ_MAX - CGROUP_BFQ_ತೂಕ_DEFAULT) / (CGROUP_ತೂಕ_MAX - CGROUP_ತೂಕ_DEFAULT);
}

/*
 * General rules:
 *
 * We require absolute cgroup paths. When returning, we will always
 * generate paths with multiple adjacent / removed.
 */

ಪೂರ್ಣಾಂಕ cg_is_ಲಭ್ಯ(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);
ಪೂರ್ಣಾಂಕ cg_cgroupid_open(ಪೂರ್ಣಾಂಕ cgroupfs_fd, uint64_t id);

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_from_cgroupid(ಪೂರ್ಣಾಂಕ cgroupfs_fd, uint64_t id, ಅಕ್ಷರ **ret);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

ಪೂರ್ಣಾಂಕ cg_enumerate_processes(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, FILE **ret);
ಪೂರ್ಣಾಂಕ cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
ಪೂರ್ಣಾಂಕ cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

ಪೂರ್ಣಾಂಕ cg_enumerate_subgroups(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, DIR **ret);
ಪೂರ್ಣಾಂಕ cg_read_subgroup(DIR *d, ಅಕ್ಷರ **ret);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*cg_kill_log_func_t)(ಸ್ಥಿರಾಂಕ PidRef *pid, ಪೂರ್ಣಾಂಕ sig, ಶೂನ್ಯ *userdata);

ಪೂರ್ಣಾಂಕ cg_kill(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ sig, CGroupFlags flags, ಗಣ_2 *killed_pids, cg_kill_log_func_t log_kill, ಶೂನ್ಯ *userdata);
ಪೂರ್ಣಾಂಕ cg_kill_kernel_sigkill(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, uint64_t *ret_n_pids_killed);
ಪೂರ್ಣಾಂಕ cg_kill_recursive(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ sig, CGroupFlags flags, ಗಣ_2 *killed_pids, cg_kill_log_func_t log_kill, ಶೂನ್ಯ *userdata);

ಪೂರ್ಣಾಂಕ cg_ವಿಭಜಿಸು_spec(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *spec, ಅಕ್ಷರ **ret_controller, ಅಕ್ಷರ **ret_ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ cg_get_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ cg_pid_get_ಮಾರ್ಗ(pid_t pid, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ cg_pidref_get_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ cg_is_threaded(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ cg_is_delegated(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);
ಪೂರ್ಣಾಂಕ cg_is_delegated_fd(ಪೂರ್ಣಾಂಕ fd);

ಪೂರ್ಣಾಂಕ cg_has_coredump_receive(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ cg_ಗಣ_attribute(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attribute, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);
ಪೂರ್ಣಾಂಕ cg_get_attribute(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attribute, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ cg_get_attribute_as_uint64(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attribute, uint64_t *ret);
ಪೂರ್ಣಾಂಕ cg_get_attribute_as_bool(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attribute);

ಪೂರ್ಣಾಂಕ cg_get_keyed_attribute(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attribute, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಕೀಲಿಗಳು, ಅಕ್ಷರ **values);
ಪೂರ್ಣಾಂಕ cg_get_keyed_attribute_uint64(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attribute, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, uint64_t *ret);

ಪೂರ್ಣಾಂಕ cg_get_owner(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, uid_t *ret_uid);

ಪೂರ್ಣಾಂಕ cg_ಗಣ_xattr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಮೌಲ್ಯ, size_t ಗಾತ್ರ_2, ಪೂರ್ಣಾಂಕ flags);
ಪೂರ್ಣಾಂಕ cg_get_xattr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ret, size_t *ret_ಗಾತ್ರ);
/* Returns negative on error, and 0 or 1 on success for the bool value */
ಪೂರ್ಣಾಂಕ cg_get_xattr_bool(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
ಪೂರ್ಣಾಂಕ cg_remove_xattr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);

ಪೂರ್ಣಾಂಕ cg_is_ಖಾಲಿಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ cg_get_ಬೇರು_ಮಾರ್ಗ(ಅಕ್ಷರ **ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_session(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_session);
ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_owner_uid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, uid_t *ret_uid);
ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_unit_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_unit, ಅಕ್ಷರ **ret_subgroup);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_unit(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_unit) {
        ಹಿಂತಿರುಗಿ cg_ಮಾರ್ಗ_get_unit_ತುಂಬಿದ(ಮಾರ್ಗ, ret_unit, ಖಾಲಿ);
}
ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_unit_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_unit);
ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_user_unit_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_unit, ಅಕ್ಷರ **ret_subgroup);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_user_unit(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_unit) {
        ಹಿಂತಿರುಗಿ cg_ಮಾರ್ಗ_get_user_unit_ತುಂಬಿದ(ಮಾರ್ಗ, ret_unit, ಖಾಲಿ);
}
ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_machine_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_machine);
ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_ತುಂಡು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_ತುಂಡು);
ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_user_ತುಂಡು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_ತುಂಡು);

ಪೂರ್ಣಾಂಕ cg_ಸರಿಸು_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cgroup, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cached_ಬೇರು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret_shifted);
ಪೂರ್ಣಾಂಕ cg_pid_get_ಮಾರ್ಗ_shifted(pid_t pid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cached_ಬೇರು, ಅಕ್ಷರ **ret_cgroup);
ಪೂರ್ಣಾಂಕ cg_pidref_get_ಮಾರ್ಗ_shifted(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cached_ಬೇರು, ಅಕ್ಷರ **ret_cgroup);

ಪೂರ್ಣಾಂಕ cg_pid_get_session(pid_t pid, ಅಕ್ಷರ **ret_session);
ಪೂರ್ಣಾಂಕ cg_pidref_get_session(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
ಪೂರ್ಣಾಂಕ cg_pidref_get_owner_uid(ಸ್ಥಿರಾಂಕ PidRef *pidref, uid_t *ret);
ಪೂರ್ಣಾಂಕ cg_pid_get_unit_ತುಂಬಿದ(pid_t pid, ಅಕ್ಷರ **ret_unit, ಅಕ್ಷರ **ret_subgroup);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ cg_pid_get_unit(pid_t pid, ಅಕ್ಷರ **ret_unit) {
        ಹಿಂತಿರುಗಿ cg_pid_get_unit_ತುಂಬಿದ(pid, ret_unit, ಖಾಲಿ);
}
ಪೂರ್ಣಾಂಕ cg_pidref_get_unit_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಅಕ್ಷರ **ret_unit, ಅಕ್ಷರ **ret_subgroup);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ cg_pidref_get_unit(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಅಕ್ಷರ **ret_unit) {
        ಹಿಂತಿರುಗಿ cg_pidref_get_unit_ತುಂಬಿದ(pidref, ret_unit, ಖಾಲಿ);
}
ಪೂರ್ಣಾಂಕ cg_pid_get_user_unit_ತುಂಬಿದ(pid_t pid, ಅಕ್ಷರ **ret_unit, ಅಕ್ಷರ **ret_subgroup);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ cg_pid_get_user_unit(pid_t pid, ಅಕ್ಷರ **ret_unit) {
        ಹಿಂತಿರುಗಿ cg_pid_get_user_unit_ತುಂಬಿದ(pid, ret_unit, ಖಾಲಿ);
}
ಪೂರ್ಣಾಂಕ cg_pidref_get_user_unit_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಅಕ್ಷರ **ret_unit, ಅಕ್ಷರ **ret_subgroup);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ cg_pidref_get_user_unit(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಅಕ್ಷರ **ret_unit) {
        ಹಿಂತಿರುಗಿ cg_pidref_get_user_unit_ತುಂಬಿದ(pidref, ret_unit, ಖಾಲಿ);
}
ಪೂರ್ಣಾಂಕ cg_pid_get_machine_name(pid_t pid, ಅಕ್ಷರ **ret_machine);
ಪೂರ್ಣಾಂಕ cg_pid_get_ತುಂಡು(pid_t pid, ಅಕ್ಷರ **ret_ತುಂಡು);
ಪೂರ್ಣಾಂಕ cg_pid_get_user_ತುಂಡು(pid_t pid, ಅಕ್ಷರ **ret_ತುಂಡು);

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_decode_unit(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cgroup, ಅಕ್ಷರ **ret_unit);

bool cg_needs_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ cg_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ **ret);
ಅಕ್ಷರ* cg_unescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) ಶುದ್ಧ;

ಪೂರ್ಣಾಂಕ cg_ತುಂಡು_ಗಮ್ಯ_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *unit, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ cg_ಮುಖವಾಡ_supported(CGroup_ಮುಖವಾಡ *ret);
ಪೂರ್ಣಾಂಕ cg_ಮುಖವಾಡ_supported_subtree(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, CGroup_ಮುಖವಾಡ *ret);
ಪೂರ್ಣಾಂಕ cg_ಮುಖವಾಡ_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, CGroup_ಮುಖವಾಡ *ret);
ಪೂರ್ಣಾಂಕ cg_ಮುಖವಾಡ_ಗಮ್ಯ_string(CGroup_ಮುಖವಾಡ ಮುಖವಾಡ_3, ಅಕ್ಷರ **ret);

bool cg_kill_supported(ಶೂನ್ಯ);

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(cgroup_controller, CGroupController);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        MANAGED_OOM_ಬಹುಲಕ_MAX,
        MANAGED_OOM_ಬಹುಲಕ_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(managed_oom_ಬಹುಲಕ, ManagedOOMMode);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(managed_oom_preference, ManagedOOMPreference);

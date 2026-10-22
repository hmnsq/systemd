/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

#define SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା CGroupController {
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା CGroupMask {
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

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool CGROUP_WEIGHT_IS_OK(uint64_t x) {
        ଫେରନ୍ତୁ
            x == CGROUP_WEIGHT_INVALID ||
            (x >= CGROUP_WEIGHT_MIN && x <= CGROUP_WEIGHT_MAX);
}

/* IO limits on unified hierarchy */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        _CGROUP_IO_LIMIT_TYPE_MAX,
        _CGROUP_IO_LIMIT_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

ବାହ୍ୟ ଅଚଳ uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX];

DECLARE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);
ଶୂନ୍ୟ cgroup_io_limits_list(ଶୂନ୍ୟ);

/* Special values for the io.bfq.weight attribute */
#define CGROUP_BFQ_WEIGHT_INVALID UINT64_MAX
#define CGROUP_BFQ_WEIGHT_MIN UINT64_C(1)
#define CGROUP_BFQ_WEIGHT_MAX UINT64_C(1000)
#define CGROUP_BFQ_WEIGHT_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint64_t BFQ_WEIGHT(uint64_t io_weight) {
        ଫେରନ୍ତୁ
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_is_available(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_open(ଅଚଳ ଅକ୍ଷର *path);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_cgroupid_open(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cgroupfs_fd, uint64_t id);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_from_cgroupid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cgroupfs_fd, uint64_t id, ଅକ୍ଷର **ret);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_enumerate_processes(ଅଚଳ ଅକ୍ଷର *path, FILE **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_enumerate_subgroups(ଅଚଳ ଅକ୍ଷର *path, DIR **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_read_subgroup(DIR *d, ଅକ୍ଷର **ret);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*cg_kill_log_func_t)(ଅଚଳ PidRef *pid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig, ଶୂନ୍ୟ *userdata);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_kill(ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, ଶୂନ୍ୟ *userdata);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_kill_kernel_sigkill(ଅଚଳ ଅକ୍ଷର *path, uint64_t *ret_n_pids_killed);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_kill_recursive(ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig, CGroupFlags flags, Set *killed_pids, cg_kill_log_func_t log_kill, ଶୂନ୍ୟ *userdata);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_split_spec(ଅଚଳ ଅକ୍ଷର *spec, ଅକ୍ଷର **ret_controller, ଅକ୍ଷର **ret_path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_path(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *suffix, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_path(pid_t pid, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pidref_get_path(ଅଚଳ PidRef *pidref, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_is_threaded(ଅଚଳ ଅକ୍ଷର *path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_is_delegated(ଅଚଳ ଅକ୍ଷର *path);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_is_delegated_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_has_coredump_receive(ଅଚଳ ଅକ୍ଷର *path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_set_attribute(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *attribute, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_attribute(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *attribute, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_attribute_as_uint64(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *attribute, uint64_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_attribute_as_bool(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *attribute);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_keyed_attribute(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *attribute, ଅକ୍ଷର * ଅଚଳ *ଚାବିଗୁଡ଼ିକ, ଅକ୍ଷର **values);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_keyed_attribute_uint64(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *attribute, ଅଚଳ ଅକ୍ଷର *ଚାବି, uint64_t *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_owner(ଅଚଳ ଅକ୍ଷର *path, uid_t *ret_uid);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_set_xattr(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଶୂନ୍ୟ *ମୂଲ୍ୟ, size_t size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_xattr(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ret, size_t *ret_size);
/* Returns negative on error, and 0 or 1 on success for the bool value */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_xattr_bool(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_remove_xattr(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_is_empty(ଅଚଳ ଅକ୍ଷର *path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_ମୂଳ_path(ଅକ୍ଷର **path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_session(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_session);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_owner_uid(ଅଚଳ ଅକ୍ଷର *path, uid_t *ret_uid);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_unit_full(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_unit, ଅକ୍ଷର **ret_subgroup);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_unit(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_unit) {
        ଫେରନ୍ତୁ cg_path_get_unit_full(path, ret_unit, ଖାଲି);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_unit_path(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_unit);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_user_unit_full(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_unit, ଅକ୍ଷର **ret_subgroup);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_user_unit(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_unit) {
        ଫେରନ୍ତୁ cg_path_get_user_unit_full(path, ret_unit, ଖାଲି);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_machine_name(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_machine);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_slice(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_slice);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_user_slice(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_slice);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_shift_path(ଅଚଳ ଅକ୍ଷର *cgroup, ଅଚଳ ଅକ୍ଷର *cached_ମୂଳ, ଅଚଳ ଅକ୍ଷର **ret_shifted);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_path_shifted(pid_t pid, ଅଚଳ ଅକ୍ଷର *cached_ମୂଳ, ଅକ୍ଷର **ret_cgroup);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pidref_get_path_shifted(ଅଚଳ PidRef *pidref, ଅଚଳ ଅକ୍ଷର *cached_ମୂଳ, ଅକ୍ଷର **ret_cgroup);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_session(pid_t pid, ଅକ୍ଷର **ret_session);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pidref_get_session(ଅଚଳ PidRef *pidref, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pidref_get_owner_uid(ଅଚଳ PidRef *pidref, uid_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_unit_full(pid_t pid, ଅକ୍ଷର **ret_unit, ଅକ୍ଷର **ret_subgroup);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_unit(pid_t pid, ଅକ୍ଷର **ret_unit) {
        ଫେରନ୍ତୁ cg_pid_get_unit_full(pid, ret_unit, ଖାଲି);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pidref_get_unit_full(ଅଚଳ PidRef *pidref, ଅକ୍ଷର **ret_unit, ଅକ୍ଷର **ret_subgroup);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pidref_get_unit(ଅଚଳ PidRef *pidref, ଅକ୍ଷର **ret_unit) {
        ଫେରନ୍ତୁ cg_pidref_get_unit_full(pidref, ret_unit, ଖାଲି);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_user_unit_full(pid_t pid, ଅକ୍ଷର **ret_unit, ଅକ୍ଷର **ret_subgroup);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_user_unit(pid_t pid, ଅକ୍ଷର **ret_unit) {
        ଫେରନ୍ତୁ cg_pid_get_user_unit_full(pid, ret_unit, ଖାଲି);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pidref_get_user_unit_full(ଅଚଳ PidRef *pidref, ଅକ୍ଷର **ret_unit, ଅକ୍ଷର **ret_subgroup);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pidref_get_user_unit(ଅଚଳ PidRef *pidref, ଅକ୍ଷର **ret_unit) {
        ଫେରନ୍ତୁ cg_pidref_get_user_unit_full(pidref, ret_unit, ଖାଲି);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_machine_name(pid_t pid, ଅକ୍ଷର **ret_machine);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_slice(pid_t pid, ଅକ୍ଷର **ret_slice);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_user_slice(pid_t pid, ଅକ୍ଷର **ret_slice);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_decode_unit(ଅଚଳ ଅକ୍ଷର *cgroup, ଅକ୍ଷର **ret_unit);

bool cg_needs_escape(ଅଚଳ ଅକ୍ଷର *p) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_escape(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର **ret);
ଅକ୍ଷର* cg_unescape(ଅଚଳ ଅକ୍ଷର *p) _pure_;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_slice_to_path(ଅଚଳ ଅକ୍ଷର *unit, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_mask_supported(CGroupMask *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_mask_supported_subtree(ଅଚଳ ଅକ୍ଷର *ମୂଳ, CGroupMask *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_mask_from_string(ଅଚଳ ଅକ୍ଷର *s, CGroupMask *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_mask_to_string(CGroupMask mask, ଅକ୍ଷର **ret);

bool cg_kill_supported(ଶୂନ୍ୟ);

DECLARE_STRING_TABLE_LOOKUP(cgroup_controller, CGroupController);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        _MANAGED_OOM_MODE_MAX,
        _MANAGED_OOM_MODE_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

#تعریف_کرو SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
قسمتعریف شمار CGroupController {
        /* Original cgroup controllers */
        CGROUP_CONTROLLER_CPU,
        CGROUP_CONTROLLER_CPUACCT,    /* v1 only */
        CGROUP_CONTROLLER_CPUSET,     /* v2 only */
        CGROUP_CONTROLLER_IO,         /* v2 only */
        CGROUP_CONTROLLER_BLKIO,      /* v1 only */
        CGROUP_CONTROLLER_حافظہ,
        CGROUP_CONTROLLER_DEVICES,    /* v1 only */
        CGROUP_CONTROLLER_PIDS,

        /* BPF-based pseudo-controllers, v2 only */
        CGROUP_CONTROLLER_BPF_FIREWALL,
        CGROUP_CONTROLLER_BPF_DEVICES,
        CGROUP_CONTROLLER_BPF_FOREIGN,
        CGROUP_CONTROLLER_BPF_SOCKET_BIND,
        CGROUP_CONTROLLER_BPF_RESTRICT_جال_INTERFACES,
        CGROUP_CONTROLLER_BPF_BIND_جال_INTERFACE,
        /* The BPF hook implementing RestrictFileSystems= is not defined here.
         * It's applied as late as possible in exec_invoke() so we don't block
         * our own unit setup code. */

        _CGROUP_CONTROLLER_MAX,
        _CGROUP_CONTROLLER_INVALID = -EINVAL,
} CGroupController;

#تعریف_کرو CGROUP_CONTROLLER_مقصد_نقاب(c) (1U << (c))

/* A bit mask of well known cgroup controllers */
قسمتعریف شمار CGroup_نقاب {
        CGROUP_نقاب_CPU = CGROUP_CONTROLLER_مقصد_نقاب(CGROUP_CONTROLLER_CPU),
        CGROUP_نقاب_CPUACCT = CGROUP_CONTROLLER_مقصد_نقاب(CGROUP_CONTROLLER_CPUACCT),
        CGROUP_نقاب_CPUSET = CGROUP_CONTROLLER_مقصد_نقاب(CGROUP_CONTROLLER_CPUSET),
        CGROUP_نقاب_IO = CGROUP_CONTROLLER_مقصد_نقاب(CGROUP_CONTROLLER_IO),
        CGROUP_نقاب_BLKIO = CGROUP_CONTROLLER_مقصد_نقاب(CGROUP_CONTROLLER_BLKIO),
        CGROUP_نقاب_حافظہ = CGROUP_CONTROLLER_مقصد_نقاب(CGROUP_CONTROLLER_حافظہ),
        CGROUP_نقاب_DEVICES = CGROUP_CONTROLLER_مقصد_نقاب(CGROUP_CONTROLLER_DEVICES),
        CGROUP_نقاب_PIDS = CGROUP_CONTROLLER_مقصد_نقاب(CGROUP_CONTROLLER_PIDS),
        CGROUP_نقاب_BPF_FIREWALL = CGROUP_CONTROLLER_مقصد_نقاب(CGROUP_CONTROLLER_BPF_FIREWALL),
        CGROUP_نقاب_BPF_DEVICES = CGROUP_CONTROLLER_مقصد_نقاب(CGROUP_CONTROLLER_BPF_DEVICES),
        CGROUP_نقاب_BPF_FOREIGN = CGROUP_CONTROLLER_مقصد_نقاب(CGROUP_CONTROLLER_BPF_FOREIGN),
        CGROUP_نقاب_BPF_SOCKET_BIND = CGROUP_CONTROLLER_مقصد_نقاب(CGROUP_CONTROLLER_BPF_SOCKET_BIND),
        CGROUP_نقاب_BPF_RESTRICT_جال_INTERFACES = CGROUP_CONTROLLER_مقصد_نقاب(CGROUP_CONTROLLER_BPF_RESTRICT_جال_INTERFACES),
        CGROUP_نقاب_BPF_BIND_جال_INTERFACE = CGROUP_CONTROLLER_مقصد_نقاب(CGROUP_CONTROLLER_BPF_BIND_جال_INTERFACE),

        /* All real cgroup v1 controllers */
        CGROUP_نقاب_V1 = CGROUP_نقاب_CPU|CGROUP_نقاب_CPUACCT|CGROUP_نقاب_BLKIO|CGROUP_نقاب_حافظہ|CGROUP_نقاب_DEVICES|CGROUP_نقاب_PIDS,

        /* All real cgroup v2 controllers */
        CGROUP_نقاب_V2 = CGROUP_نقاب_CPU|CGROUP_نقاب_CPUSET|CGROUP_نقاب_IO|CGROUP_نقاب_حافظہ|CGROUP_نقاب_PIDS,

        /* All controllers we want to delegate in case of Delegate=yes. Which are pretty much the v2 controllers only, as delegation on v1 is not safe, and bpf stuff isn't a real controller */
        CGROUP_نقاب_DELEGATE = CGROUP_نقاب_V2,

        /* All cgroup v2 BPF pseudo-controllers */
        CGROUP_نقاب_BPF = CGROUP_نقاب_BPF_FIREWALL|CGROUP_نقاب_BPF_DEVICES|CGROUP_نقاب_BPF_FOREIGN|CGROUP_نقاب_BPF_SOCKET_BIND|CGROUP_نقاب_BPF_RESTRICT_جال_INTERFACES|CGROUP_نقاب_BPF_BIND_جال_INTERFACE,

        CGROUP_نقاب_ALL = CGROUP_CONTROLLER_مقصد_نقاب(_CGROUP_CONTROLLER_MAX) - 1,
} CGroup_نقاب;

/* Special values for all weight knobs on unified hierarchy */
#تعریف_کرو CGROUP_وزن_INVALID UINT64_MAX
#تعریف_کرو CGROUP_وزن_IDLE UINT64_C(0)
#تعریف_کرو CGROUP_وزن_MIN UINT64_C(1)
#تعریف_کرو CGROUP_وزن_MAX UINT64_C(10000)
#تعریف_کرو CGROUP_وزن_DEFAULT UINT64_C(100)

#تعریف_کرو CGROUP_حد_بندی_MIN UINT64_C(0)
#تعریف_کرو CGROUP_حد_بندی_MAX UINT64_MAX

ساکن اندرونی bool CGROUP_وزن_IS_OK(uint64_t x) {
        واپس
            x == CGROUP_وزن_INVALID ||
            (x >= CGROUP_وزن_MIN && x <= CGROUP_وزن_MAX);
}

/* IO limits on unified hierarchy */
قسمتعریف شمار CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        CGROUP_IO_حد_بندی_TYPE_MAX,
        CGROUP_IO_حد_بندی_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

خارجی ثابت uint64_t cgroup_io_حد_بندی_defaults[CGROUP_IO_حد_بندی_TYPE_MAX];

DECLARE_STRING_جدول_LOOKUP(cgroup_io_حد_بندی_type, CGroupIOLimitType);
خالی cgroup_io_limits_list(خالی);

/* Special values for the io.bfq.weight attribute */
#تعریف_کرو CGROUP_BFQ_وزن_INVALID UINT64_MAX
#تعریف_کرو CGROUP_BFQ_وزن_MIN UINT64_C(1)
#تعریف_کرو CGROUP_BFQ_وزن_MAX UINT64_C(1000)
#تعریف_کرو CGROUP_BFQ_وزن_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
ساکن اندرونی uint64_t BFQ_وزن(uint64_t io_وزن) {
        واپس
            io_وزن <= CGROUP_وزن_DEFAULT ?
            CGROUP_BFQ_وزن_DEFAULT - (CGROUP_وزن_DEFAULT - io_وزن) * (CGROUP_BFQ_وزن_DEFAULT - CGROUP_BFQ_وزن_MIN) / (CGROUP_وزن_DEFAULT - CGROUP_وزن_MIN) :
            CGROUP_BFQ_وزن_DEFAULT + (io_وزن - CGROUP_وزن_DEFAULT) * (CGROUP_BFQ_وزن_MAX - CGROUP_BFQ_وزن_DEFAULT) / (CGROUP_وزن_MAX - CGROUP_وزن_DEFAULT);
}

/*
 * General rules:
 *
 * We require absolute cgroup paths. When returning, we will always
 * generate paths with multiple adjacent / removed.
 */

صحیح cg_is_دستیاب(خالی);

صحیح cg_راستہ_open(ثابت حرف *راستہ);
صحیح cg_cgroupid_open(صحیح cgroupfs_fd, uint64_t id);

صحیح cg_راستہ_from_cgroupid(صحیح cgroupfs_fd, uint64_t id, حرف **ret);

قسمتعریف شمار CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

صحیح cg_enumerate_processes(ثابت حرف *راستہ, FILE **ret);
صحیح cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
صحیح cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

صحیح cg_enumerate_subgroups(ثابت حرف *راستہ, DIR **ret);
صحیح cg_read_subgroup(DIR *d, حرف **ret);

قسمتعریف صحیح (*cg_kill_log_func_t)(ثابت PidRef *pid, صحیح sig, خالی *userdata);

صحیح cg_kill(ثابت حرف *راستہ, صحیح sig, CGroupFlags flags, سیٹ_2 *killed_pids, cg_kill_log_func_t log_kill, خالی *userdata);
صحیح cg_kill_kernel_sigkill(ثابت حرف *راستہ, uint64_t *ret_n_pids_killed);
صحیح cg_kill_recursive(ثابت حرف *راستہ, صحیح sig, CGroupFlags flags, سیٹ_2 *killed_pids, cg_kill_log_func_t log_kill, خالی *userdata);

صحیح cg_بانٹو_spec(ثابت حرف *spec, حرف **ret_controller, حرف **ret_راستہ);

صحیح cg_get_راستہ(ثابت حرف *راستہ, ثابت حرف *suffix, حرف **ret);

صحیح cg_pid_get_راستہ(pid_t pid, حرف **ret);
صحیح cg_pidref_get_راستہ(ثابت PidRef *pidref, حرف **ret);

صحیح cg_is_threaded(ثابت حرف *راستہ);

صحیح cg_is_delegated(ثابت حرف *راستہ);
صحیح cg_is_delegated_fd(صحیح fd);

صحیح cg_has_coredump_receive(ثابت حرف *راستہ);

صحیح cg_سیٹ_attribute(ثابت حرف *راستہ, ثابت حرف *attribute, ثابت حرف *قدر);
صحیح cg_get_attribute(ثابت حرف *راستہ, ثابت حرف *attribute, حرف **ret);
صحیح cg_get_attribute_as_uint64(ثابت حرف *راستہ, ثابت حرف *attribute, uint64_t *ret);
صحیح cg_get_attribute_as_bool(ثابت حرف *راستہ, ثابت حرف *attribute);

صحیح cg_get_keyed_attribute(ثابت حرف *راستہ, ثابت حرف *attribute, حرف * ثابت *کلیدیں, حرف **values);
صحیح cg_get_keyed_attribute_uint64(ثابت حرف *راستہ, ثابت حرف *attribute, ثابت حرف *کلید, uint64_t *ret);

صحیح cg_get_owner(ثابت حرف *راستہ, uid_t *ret_uid);

صحیح cg_سیٹ_xattr(ثابت حرف *راستہ, ثابت حرف *name, ثابت خالی *قدر, size_t سائز, صحیح flags);
صحیح cg_get_xattr(ثابت حرف *راستہ, ثابت حرف *name, حرف **ret, size_t *ret_سائز);
/* Returns negative on error, and 0 or 1 on success for the bool value */
صحیح cg_get_xattr_bool(ثابت حرف *راستہ, ثابت حرف *name);
صحیح cg_remove_xattr(ثابت حرف *راستہ, ثابت حرف *name);

صحیح cg_is_خالی_خانے(ثابت حرف *راستہ);

صحیح cg_get_جڑ_راستہ(حرف **راستہ);

صحیح cg_راستہ_get_session(ثابت حرف *راستہ, حرف **ret_session);
صحیح cg_راستہ_get_owner_uid(ثابت حرف *راستہ, uid_t *ret_uid);
صحیح cg_راستہ_get_unit_بھرا(ثابت حرف *راستہ, حرف **ret_unit, حرف **ret_subgroup);
ساکن اندرونی صحیح cg_راستہ_get_unit(ثابت حرف *راستہ, حرف **ret_unit) {
        واپس cg_راستہ_get_unit_بھرا(راستہ, ret_unit, تہی);
}
صحیح cg_راستہ_get_unit_راستہ(ثابت حرف *راستہ, حرف **ret_unit);
صحیح cg_راستہ_get_user_unit_بھرا(ثابت حرف *راستہ, حرف **ret_unit, حرف **ret_subgroup);
ساکن اندرونی صحیح cg_راستہ_get_user_unit(ثابت حرف *راستہ, حرف **ret_unit) {
        واپس cg_راستہ_get_user_unit_بھرا(راستہ, ret_unit, تہی);
}
صحیح cg_راستہ_get_machine_name(ثابت حرف *راستہ, حرف **ret_machine);
صحیح cg_راستہ_get_قاش(ثابت حرف *راستہ, حرف **ret_قاش);
صحیح cg_راستہ_get_user_قاش(ثابت حرف *راستہ, حرف **ret_قاش);

صحیح cg_سرکاؤ_راستہ(ثابت حرف *cgroup, ثابت حرف *cached_جڑ, ثابت حرف **ret_shifted);
صحیح cg_pid_get_راستہ_shifted(pid_t pid, ثابت حرف *cached_جڑ, حرف **ret_cgroup);
صحیح cg_pidref_get_راستہ_shifted(ثابت PidRef *pidref, ثابت حرف *cached_جڑ, حرف **ret_cgroup);

صحیح cg_pid_get_session(pid_t pid, حرف **ret_session);
صحیح cg_pidref_get_session(ثابت PidRef *pidref, حرف **ret);
صحیح cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
صحیح cg_pidref_get_owner_uid(ثابت PidRef *pidref, uid_t *ret);
صحیح cg_pid_get_unit_بھرا(pid_t pid, حرف **ret_unit, حرف **ret_subgroup);
ساکن اندرونی صحیح cg_pid_get_unit(pid_t pid, حرف **ret_unit) {
        واپس cg_pid_get_unit_بھرا(pid, ret_unit, تہی);
}
صحیح cg_pidref_get_unit_بھرا(ثابت PidRef *pidref, حرف **ret_unit, حرف **ret_subgroup);
ساکن اندرونی صحیح cg_pidref_get_unit(ثابت PidRef *pidref, حرف **ret_unit) {
        واپس cg_pidref_get_unit_بھرا(pidref, ret_unit, تہی);
}
صحیح cg_pid_get_user_unit_بھرا(pid_t pid, حرف **ret_unit, حرف **ret_subgroup);
ساکن اندرونی صحیح cg_pid_get_user_unit(pid_t pid, حرف **ret_unit) {
        واپس cg_pid_get_user_unit_بھرا(pid, ret_unit, تہی);
}
صحیح cg_pidref_get_user_unit_بھرا(ثابت PidRef *pidref, حرف **ret_unit, حرف **ret_subgroup);
ساکن اندرونی صحیح cg_pidref_get_user_unit(ثابت PidRef *pidref, حرف **ret_unit) {
        واپس cg_pidref_get_user_unit_بھرا(pidref, ret_unit, تہی);
}
صحیح cg_pid_get_machine_name(pid_t pid, حرف **ret_machine);
صحیح cg_pid_get_قاش(pid_t pid, حرف **ret_قاش);
صحیح cg_pid_get_user_قاش(pid_t pid, حرف **ret_قاش);

صحیح cg_راستہ_decode_unit(ثابت حرف *cgroup, حرف **ret_unit);

bool cg_needs_escape(ثابت حرف *p) خالص;
صحیح cg_escape(ثابت حرف *p, حرف **ret);
حرف* cg_unescape(ثابت حرف *p) خالص;

صحیح cg_قاش_مقصد_راستہ(ثابت حرف *unit, حرف **ret);

صحیح cg_نقاب_supported(CGroup_نقاب *ret);
صحیح cg_نقاب_supported_subtree(ثابت حرف *جڑ, CGroup_نقاب *ret);
صحیح cg_نقاب_from_string(ثابت حرف *s, CGroup_نقاب *ret);
صحیح cg_نقاب_مقصد_string(CGroup_نقاب نقاب_3, حرف **ret);

bool cg_kill_supported(خالی);

DECLARE_STRING_جدول_LOOKUP(cgroup_controller, CGroupController);

قسمتعریف شمار ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        MANAGED_OOM_منوال_MAX,
        MANAGED_OOM_منوال_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_جدول_LOOKUP(managed_oom_منوال, ManagedOOMMode);

قسمتعریف شمار ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_جدول_LOOKUP(managed_oom_preference, ManagedOOMPreference);

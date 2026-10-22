/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

#تعریف_کن SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
تعريفنوع تعداد CGroupController {
        /* Original cgroup controllers */
        CGROUP_CONTROLLER_CPU,
        CGROUP_CONTROLLER_CPUACCT,    /* v1 only */
        CGROUP_CONTROLLER_CPUSET,     /* v2 only */
        CGROUP_CONTROLLER_IO,         /* v2 only */
        CGROUP_CONTROLLER_BLKIO,      /* v1 only */
        CGROUP_CONTROLLER_حافظه,
        CGROUP_CONTROLLER_DEVICES,    /* v1 only */
        CGROUP_CONTROLLER_PIDS,

        /* BPF-based pseudo-controllers, v2 only */
        CGROUP_CONTROLLER_BPF_FIREWALL,
        CGROUP_CONTROLLER_BPF_DEVICES,
        CGROUP_CONTROLLER_BPF_FOREIGN,
        CGROUP_CONTROLLER_BPF_SOCKET_BIND,
        CGROUP_CONTROLLER_BPF_RESTRICT_شبکه_INTERFACES,
        CGROUP_CONTROLLER_BPF_BIND_شبکه_INTERFACE,
        /* The BPF hook implementing RestrictFileSystems= is not defined here.
         * It's applied as late as possible in exec_invoke() so we don't block
         * our own unit setup code. */

        _CGROUP_CONTROLLER_MAX,
        _CGROUP_CONTROLLER_INVALID = -EINVAL,
} CGroupController;

#تعریف_کن CGROUP_CONTROLLER_مقصد_ماسک(c) (1U << (c))

/* A bit mask of well known cgroup controllers */
تعريفنوع تعداد CGroup_ماسک {
        CGROUP_ماسک_CPU = CGROUP_CONTROLLER_مقصد_ماسک(CGROUP_CONTROLLER_CPU),
        CGROUP_ماسک_CPUACCT = CGROUP_CONTROLLER_مقصد_ماسک(CGROUP_CONTROLLER_CPUACCT),
        CGROUP_ماسک_CPUSET = CGROUP_CONTROLLER_مقصد_ماسک(CGROUP_CONTROLLER_CPUSET),
        CGROUP_ماسک_IO = CGROUP_CONTROLLER_مقصد_ماسک(CGROUP_CONTROLLER_IO),
        CGROUP_ماسک_BLKIO = CGROUP_CONTROLLER_مقصد_ماسک(CGROUP_CONTROLLER_BLKIO),
        CGROUP_ماسک_حافظه = CGROUP_CONTROLLER_مقصد_ماسک(CGROUP_CONTROLLER_حافظه),
        CGROUP_ماسک_DEVICES = CGROUP_CONTROLLER_مقصد_ماسک(CGROUP_CONTROLLER_DEVICES),
        CGROUP_ماسک_PIDS = CGROUP_CONTROLLER_مقصد_ماسک(CGROUP_CONTROLLER_PIDS),
        CGROUP_ماسک_BPF_FIREWALL = CGROUP_CONTROLLER_مقصد_ماسک(CGROUP_CONTROLLER_BPF_FIREWALL),
        CGROUP_ماسک_BPF_DEVICES = CGROUP_CONTROLLER_مقصد_ماسک(CGROUP_CONTROLLER_BPF_DEVICES),
        CGROUP_ماسک_BPF_FOREIGN = CGROUP_CONTROLLER_مقصد_ماسک(CGROUP_CONTROLLER_BPF_FOREIGN),
        CGROUP_ماسک_BPF_SOCKET_BIND = CGROUP_CONTROLLER_مقصد_ماسک(CGROUP_CONTROLLER_BPF_SOCKET_BIND),
        CGROUP_ماسک_BPF_RESTRICT_شبکه_INTERFACES = CGROUP_CONTROLLER_مقصد_ماسک(CGROUP_CONTROLLER_BPF_RESTRICT_شبکه_INTERFACES),
        CGROUP_ماسک_BPF_BIND_شبکه_INTERFACE = CGROUP_CONTROLLER_مقصد_ماسک(CGROUP_CONTROLLER_BPF_BIND_شبکه_INTERFACE),

        /* All real cgroup v1 controllers */
        CGROUP_ماسک_V1 = CGROUP_ماسک_CPU|CGROUP_ماسک_CPUACCT|CGROUP_ماسک_BLKIO|CGROUP_ماسک_حافظه|CGROUP_ماسک_DEVICES|CGROUP_ماسک_PIDS,

        /* All real cgroup v2 controllers */
        CGROUP_ماسک_V2 = CGROUP_ماسک_CPU|CGROUP_ماسک_CPUSET|CGROUP_ماسک_IO|CGROUP_ماسک_حافظه|CGROUP_ماسک_PIDS,

        /* All controllers we want to delegate in case of Delegate=yes. Which are pretty much the v2 controllers only, as delegation on v1 is not safe, and bpf stuff isn't a real controller */
        CGROUP_ماسک_DELEGATE = CGROUP_ماسک_V2,

        /* All cgroup v2 BPF pseudo-controllers */
        CGROUP_ماسک_BPF = CGROUP_ماسک_BPF_FIREWALL|CGROUP_ماسک_BPF_DEVICES|CGROUP_ماسک_BPF_FOREIGN|CGROUP_ماسک_BPF_SOCKET_BIND|CGROUP_ماسک_BPF_RESTRICT_شبکه_INTERFACES|CGROUP_ماسک_BPF_BIND_شبکه_INTERFACE,

        CGROUP_ماسک_ALL = CGROUP_CONTROLLER_مقصد_ماسک(_CGROUP_CONTROLLER_MAX) - 1,
} CGroup_ماسک;

/* Special values for all weight knobs on unified hierarchy */
#تعریف_کن CGROUP_وزن_INVALID UINT64_MAX
#تعریف_کن CGROUP_وزن_IDLE UINT64_C(0)
#تعریف_کن CGROUP_وزن_MIN UINT64_C(1)
#تعریف_کن CGROUP_وزن_MAX UINT64_C(10000)
#تعریف_کن CGROUP_وزن_DEFAULT UINT64_C(100)

#تعریف_کن CGROUP_حد_MIN UINT64_C(0)
#تعریف_کن CGROUP_حد_MAX UINT64_MAX

ساكن مضمن bool CGROUP_وزن_IS_OK(uint64_t x) {
        أرجع
            x == CGROUP_وزن_INVALID ||
            (x >= CGROUP_وزن_MIN && x <= CGROUP_وزن_MAX);
}

/* IO limits on unified hierarchy */
تعريفنوع تعداد CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        CGROUP_IO_حد_TYPE_MAX,
        CGROUP_IO_حد_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

خارجي ثابت uint64_t cgroup_io_حد_defaults[CGROUP_IO_حد_TYPE_MAX];

DECLARE_STRING_جدول_LOOKUP(cgroup_io_حد_type, CGroupIOLimitType);
فراغ cgroup_io_limits_list(فراغ);

/* Special values for the io.bfq.weight attribute */
#تعریف_کن CGROUP_BFQ_وزن_INVALID UINT64_MAX
#تعریف_کن CGROUP_BFQ_وزن_MIN UINT64_C(1)
#تعریف_کن CGROUP_BFQ_وزن_MAX UINT64_C(1000)
#تعریف_کن CGROUP_BFQ_وزن_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
ساكن مضمن uint64_t BFQ_وزن(uint64_t io_وزن) {
        أرجع
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

صحيح cg_is_موجود(فراغ);

صحيح cg_مسیر_open(ثابت حرف *مسیر);
صحيح cg_cgroupid_open(صحيح cgroupfs_fd, uint64_t id);

صحيح cg_مسیر_from_cgroupid(صحيح cgroupfs_fd, uint64_t id, حرف **ret);

تعريفنوع تعداد CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

صحيح cg_enumerate_processes(ثابت حرف *مسیر, FILE **ret);
صحيح cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
صحيح cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

صحيح cg_enumerate_subgroups(ثابت حرف *مسیر, DIR **ret);
صحيح cg_read_subgroup(DIR *d, حرف **ret);

تعريفنوع صحيح (*cg_kill_log_func_t)(ثابت PidRef *pid, صحيح sig, فراغ *userdata);

صحيح cg_kill(ثابت حرف *مسیر, صحيح sig, CGroupFlags flags, مجموعه_2 *killed_pids, cg_kill_log_func_t log_kill, فراغ *userdata);
صحيح cg_kill_kernel_sigkill(ثابت حرف *مسیر, uint64_t *ret_n_pids_killed);
صحيح cg_kill_recursive(ثابت حرف *مسیر, صحيح sig, CGroupFlags flags, مجموعه_2 *killed_pids, cg_kill_log_func_t log_kill, فراغ *userdata);

صحيح cg_تقسیم_spec(ثابت حرف *spec, حرف **ret_controller, حرف **ret_مسیر);

صحيح cg_get_مسیر(ثابت حرف *مسیر, ثابت حرف *suffix, حرف **ret);

صحيح cg_pid_get_مسیر(pid_t pid, حرف **ret);
صحيح cg_pidref_get_مسیر(ثابت PidRef *pidref, حرف **ret);

صحيح cg_is_threaded(ثابت حرف *مسیر);

صحيح cg_is_delegated(ثابت حرف *مسیر);
صحيح cg_is_delegated_fd(صحيح fd);

صحيح cg_has_coredump_receive(ثابت حرف *مسیر);

صحيح cg_مجموعه_attribute(ثابت حرف *مسیر, ثابت حرف *attribute, ثابت حرف *مقدار);
صحيح cg_get_attribute(ثابت حرف *مسیر, ثابت حرف *attribute, حرف **ret);
صحيح cg_get_attribute_as_uint64(ثابت حرف *مسیر, ثابت حرف *attribute, uint64_t *ret);
صحيح cg_get_attribute_as_bool(ثابت حرف *مسیر, ثابت حرف *attribute);

صحيح cg_get_keyed_attribute(ثابت حرف *مسیر, ثابت حرف *attribute, حرف * ثابت *کلیدها, حرف **values);
صحيح cg_get_keyed_attribute_uint64(ثابت حرف *مسیر, ثابت حرف *attribute, ثابت حرف *کلید, uint64_t *ret);

صحيح cg_get_owner(ثابت حرف *مسیر, uid_t *ret_uid);

صحيح cg_مجموعه_xattr(ثابت حرف *مسیر, ثابت حرف *name, ثابت فراغ *مقدار, size_t اندازه, صحيح flags);
صحيح cg_get_xattr(ثابت حرف *مسیر, ثابت حرف *name, حرف **ret, size_t *ret_اندازه);
/* Returns negative on error, and 0 or 1 on success for the bool value */
صحيح cg_get_xattr_bool(ثابت حرف *مسیر, ثابت حرف *name);
صحيح cg_remove_xattr(ثابت حرف *مسیر, ثابت حرف *name);

صحيح cg_is_خالی(ثابت حرف *مسیر);

صحيح cg_get_ریشه_مسیر(حرف **مسیر);

صحيح cg_مسیر_get_session(ثابت حرف *مسیر, حرف **ret_session);
صحيح cg_مسیر_get_owner_uid(ثابت حرف *مسیر, uid_t *ret_uid);
صحيح cg_مسیر_get_unit_پر(ثابت حرف *مسیر, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_مسیر_get_unit(ثابت حرف *مسیر, حرف **ret_unit) {
        أرجع cg_مسیر_get_unit_پر(مسیر, ret_unit, عدم);
}
صحيح cg_مسیر_get_unit_مسیر(ثابت حرف *مسیر, حرف **ret_unit);
صحيح cg_مسیر_get_user_unit_پر(ثابت حرف *مسیر, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_مسیر_get_user_unit(ثابت حرف *مسیر, حرف **ret_unit) {
        أرجع cg_مسیر_get_user_unit_پر(مسیر, ret_unit, عدم);
}
صحيح cg_مسیر_get_machine_name(ثابت حرف *مسیر, حرف **ret_machine);
صحيح cg_مسیر_get_برش(ثابت حرف *مسیر, حرف **ret_برش);
صحيح cg_مسیر_get_user_برش(ثابت حرف *مسیر, حرف **ret_برش);

صحيح cg_لغزش_مسیر(ثابت حرف *cgroup, ثابت حرف *cached_ریشه, ثابت حرف **ret_shifted);
صحيح cg_pid_get_مسیر_shifted(pid_t pid, ثابت حرف *cached_ریشه, حرف **ret_cgroup);
صحيح cg_pidref_get_مسیر_shifted(ثابت PidRef *pidref, ثابت حرف *cached_ریشه, حرف **ret_cgroup);

صحيح cg_pid_get_session(pid_t pid, حرف **ret_session);
صحيح cg_pidref_get_session(ثابت PidRef *pidref, حرف **ret);
صحيح cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
صحيح cg_pidref_get_owner_uid(ثابت PidRef *pidref, uid_t *ret);
صحيح cg_pid_get_unit_پر(pid_t pid, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pid_get_unit(pid_t pid, حرف **ret_unit) {
        أرجع cg_pid_get_unit_پر(pid, ret_unit, عدم);
}
صحيح cg_pidref_get_unit_پر(ثابت PidRef *pidref, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pidref_get_unit(ثابت PidRef *pidref, حرف **ret_unit) {
        أرجع cg_pidref_get_unit_پر(pidref, ret_unit, عدم);
}
صحيح cg_pid_get_user_unit_پر(pid_t pid, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pid_get_user_unit(pid_t pid, حرف **ret_unit) {
        أرجع cg_pid_get_user_unit_پر(pid, ret_unit, عدم);
}
صحيح cg_pidref_get_user_unit_پر(ثابت PidRef *pidref, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pidref_get_user_unit(ثابت PidRef *pidref, حرف **ret_unit) {
        أرجع cg_pidref_get_user_unit_پر(pidref, ret_unit, عدم);
}
صحيح cg_pid_get_machine_name(pid_t pid, حرف **ret_machine);
صحيح cg_pid_get_برش(pid_t pid, حرف **ret_برش);
صحيح cg_pid_get_user_برش(pid_t pid, حرف **ret_برش);

صحيح cg_مسیر_decode_unit(ثابت حرف *cgroup, حرف **ret_unit);

bool cg_needs_escape(ثابت حرف *p) خالص;
صحيح cg_escape(ثابت حرف *p, حرف **ret);
حرف* cg_unescape(ثابت حرف *p) خالص;

صحيح cg_برش_مقصد_مسیر(ثابت حرف *unit, حرف **ret);

صحيح cg_ماسک_supported(CGroup_ماسک *ret);
صحيح cg_ماسک_supported_subtree(ثابت حرف *ریشه, CGroup_ماسک *ret);
صحيح cg_ماسک_from_string(ثابت حرف *s, CGroup_ماسک *ret);
صحيح cg_ماسک_مقصد_string(CGroup_ماسک ماسک_3, حرف **ret);

bool cg_kill_supported(فراغ);

DECLARE_STRING_جدول_LOOKUP(cgroup_controller, CGroupController);

تعريفنوع تعداد ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        MANAGED_OOM_نما_MAX,
        MANAGED_OOM_نما_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_جدول_LOOKUP(managed_oom_نما, ManagedOOMMode);

تعريفنوع تعداد ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_جدول_LOOKUP(managed_oom_preference, ManagedOOMPreference);

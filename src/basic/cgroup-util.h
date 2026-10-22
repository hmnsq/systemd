/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

#تعریف_کرو SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
تعريفنوع تعداد CGroupController {
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

#تعریف_کرو CGROUP_CONTROLLER_منزل_نقاب(c) (1U << (c))

/* A bit mask of well known cgroup controllers */
تعريفنوع تعداد CGroup_نقاب {
        CGROUP_نقاب_CPU = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_CPU),
        CGROUP_نقاب_CPUACCT = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_CPUACCT),
        CGROUP_نقاب_CPUSET = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_CPUSET),
        CGROUP_نقاب_IO = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_IO),
        CGROUP_نقاب_BLKIO = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_BLKIO),
        CGROUP_نقاب_حافظہ = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_حافظہ),
        CGROUP_نقاب_DEVICES = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_DEVICES),
        CGROUP_نقاب_PIDS = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_PIDS),
        CGROUP_نقاب_BPF_FIREWALL = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_BPF_FIREWALL),
        CGROUP_نقاب_BPF_DEVICES = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_BPF_DEVICES),
        CGROUP_نقاب_BPF_FOREIGN = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_BPF_FOREIGN),
        CGROUP_نقاب_BPF_SOCKET_BIND = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_BPF_SOCKET_BIND),
        CGROUP_نقاب_BPF_RESTRICT_جال_INTERFACES = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_BPF_RESTRICT_جال_INTERFACES),
        CGROUP_نقاب_BPF_BIND_جال_INTERFACE = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_BPF_BIND_جال_INTERFACE),

        /* All real cgroup v1 controllers */
        CGROUP_نقاب_V1 = CGROUP_نقاب_CPU|CGROUP_نقاب_CPUACCT|CGROUP_نقاب_BLKIO|CGROUP_نقاب_حافظہ|CGROUP_نقاب_DEVICES|CGROUP_نقاب_PIDS,

        /* All real cgroup v2 controllers */
        CGROUP_نقاب_V2 = CGROUP_نقاب_CPU|CGROUP_نقاب_CPUSET|CGROUP_نقاب_IO|CGROUP_نقاب_حافظہ|CGROUP_نقاب_PIDS,

        /* All controllers we want to delegate in case of Delegate=yes. Which are pretty much the v2 controllers only, as delegation on v1 is not safe, and bpf stuff isn't a real controller */
        CGROUP_نقاب_DELEGATE = CGROUP_نقاب_V2,

        /* All cgroup v2 BPF pseudo-controllers */
        CGROUP_نقاب_BPF = CGROUP_نقاب_BPF_FIREWALL|CGROUP_نقاب_BPF_DEVICES|CGROUP_نقاب_BPF_FOREIGN|CGROUP_نقاب_BPF_SOCKET_BIND|CGROUP_نقاب_BPF_RESTRICT_جال_INTERFACES|CGROUP_نقاب_BPF_BIND_جال_INTERFACE,

        CGROUP_نقاب_ALL = CGROUP_CONTROLLER_منزل_نقاب(_CGROUP_CONTROLLER_MAX) - 1,
} CGroup_نقاب;

/* Special values for all weight knobs on unified hierarchy */
#تعریف_کرو CGROUP_بھار_INVALID UINT64_MAX
#تعریف_کرو CGROUP_بھار_IDLE UINT64_C(0)
#تعریف_کرو CGROUP_بھار_MIN UINT64_C(1)
#تعریف_کرو CGROUP_بھار_MAX UINT64_C(10000)
#تعریف_کرو CGROUP_بھار_DEFAULT UINT64_C(100)

#تعریف_کرو CGROUP_حد_بندی_MIN UINT64_C(0)
#تعریف_کرو CGROUP_حد_بندی_MAX UINT64_MAX

ساكن مضمن bool CGROUP_بھار_IS_OK(uint64_t x) {
        أرجع
            x == CGROUP_بھار_INVALID ||
            (x >= CGROUP_بھار_MIN && x <= CGROUP_بھار_MAX);
}

/* IO limits on unified hierarchy */
تعريفنوع تعداد CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        CGROUP_IO_حد_بندی_TYPE_MAX,
        CGROUP_IO_حد_بندی_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

خارجي ثابت uint64_t cgroup_io_حد_بندی_defaults[CGROUP_IO_حد_بندی_TYPE_MAX];

DECLARE_STRING_جدول_LOOKUP(cgroup_io_حد_بندی_type, CGroupIOLimitType);
فراغ cgroup_io_limits_list(فراغ);

/* Special values for the io.bfq.weight attribute */
#تعریف_کرو CGROUP_BFQ_بھار_INVALID UINT64_MAX
#تعریف_کرو CGROUP_BFQ_بھار_MIN UINT64_C(1)
#تعریف_کرو CGROUP_BFQ_بھار_MAX UINT64_C(1000)
#تعریف_کرو CGROUP_BFQ_بھار_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
ساكن مضمن uint64_t BFQ_بھار(uint64_t io_بھار) {
        أرجع
            io_بھار <= CGROUP_بھار_DEFAULT ?
            CGROUP_BFQ_بھار_DEFAULT - (CGROUP_بھار_DEFAULT - io_بھار) * (CGROUP_BFQ_بھار_DEFAULT - CGROUP_BFQ_بھار_MIN) / (CGROUP_بھار_DEFAULT - CGROUP_بھار_MIN) :
            CGROUP_BFQ_بھار_DEFAULT + (io_بھار - CGROUP_بھار_DEFAULT) * (CGROUP_BFQ_بھار_MAX - CGROUP_BFQ_بھار_DEFAULT) / (CGROUP_بھار_MAX - CGROUP_بھار_DEFAULT);
}

/*
 * General rules:
 *
 * We require absolute cgroup paths. When returning, we will always
 * generate paths with multiple adjacent / removed.
 */

صحيح cg_is_دستیاب(فراغ);

صحيح cg_رستہ_open(ثابت حرف *رستہ);
صحيح cg_cgroupid_open(صحيح cgroupfs_fd, uint64_t id);

صحيح cg_رستہ_from_cgroupid(صحيح cgroupfs_fd, uint64_t id, حرف **ret);

تعريفنوع تعداد CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

صحيح cg_enumerate_processes(ثابت حرف *رستہ, FILE **ret);
صحيح cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
صحيح cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

صحيح cg_enumerate_subgroups(ثابت حرف *رستہ, DIR **ret);
صحيح cg_read_subgroup(DIR *d, حرف **ret);

تعريفنوع صحيح (*cg_kill_log_func_t)(ثابت PidRef *pid, صحيح sig, فراغ *userdata);

صحيح cg_kill(ثابت حرف *رستہ, صحيح sig, CGroupFlags flags, سیٹ_2 *killed_pids, cg_kill_log_func_t log_kill, فراغ *userdata);
صحيح cg_kill_kernel_sigkill(ثابت حرف *رستہ, uint64_t *ret_n_pids_killed);
صحيح cg_kill_recursive(ثابت حرف *رستہ, صحيح sig, CGroupFlags flags, سیٹ_2 *killed_pids, cg_kill_log_func_t log_kill, فراغ *userdata);

صحيح cg_ونڈو_spec(ثابت حرف *spec, حرف **ret_controller, حرف **ret_رستہ);

صحيح cg_get_رستہ(ثابت حرف *رستہ, ثابت حرف *suffix, حرف **ret);

صحيح cg_pid_get_رستہ(pid_t pid, حرف **ret);
صحيح cg_pidref_get_رستہ(ثابت PidRef *pidref, حرف **ret);

صحيح cg_is_threaded(ثابت حرف *رستہ);

صحيح cg_is_delegated(ثابت حرف *رستہ);
صحيح cg_is_delegated_fd(صحيح fd);

صحيح cg_has_coredump_receive(ثابت حرف *رستہ);

صحيح cg_سیٹ_attribute(ثابت حرف *رستہ, ثابت حرف *attribute, ثابت حرف *قدر);
صحيح cg_get_attribute(ثابت حرف *رستہ, ثابت حرف *attribute, حرف **ret);
صحيح cg_get_attribute_as_uint64(ثابت حرف *رستہ, ثابت حرف *attribute, uint64_t *ret);
صحيح cg_get_attribute_as_bool(ثابت حرف *رستہ, ثابت حرف *attribute);

صحيح cg_get_keyed_attribute(ثابت حرف *رستہ, ثابت حرف *attribute, حرف * ثابت *کلیدیں, حرف **values);
صحيح cg_get_keyed_attribute_uint64(ثابت حرف *رستہ, ثابت حرف *attribute, ثابت حرف *کلید, uint64_t *ret);

صحيح cg_get_owner(ثابت حرف *رستہ, uid_t *ret_uid);

صحيح cg_سیٹ_xattr(ثابت حرف *رستہ, ثابت حرف *name, ثابت فراغ *قدر, size_t ناپ, صحيح flags);
صحيح cg_get_xattr(ثابت حرف *رستہ, ثابت حرف *name, حرف **ret, size_t *ret_ناپ);
/* Returns negative on error, and 0 or 1 on success for the bool value */
صحيح cg_get_xattr_bool(ثابت حرف *رستہ, ثابت حرف *name);
صحيح cg_remove_xattr(ثابت حرف *رستہ, ثابت حرف *name);

صحيح cg_is_خالی_تھاواں(ثابت حرف *رستہ);

صحيح cg_get_جڑ_رستہ(حرف **رستہ);

صحيح cg_رستہ_get_session(ثابت حرف *رستہ, حرف **ret_session);
صحيح cg_رستہ_get_owner_uid(ثابت حرف *رستہ, uid_t *ret_uid);
صحيح cg_رستہ_get_unit_بھریا(ثابت حرف *رستہ, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_رستہ_get_unit(ثابت حرف *رستہ, حرف **ret_unit) {
        أرجع cg_رستہ_get_unit_بھریا(رستہ, ret_unit, عدم);
}
صحيح cg_رستہ_get_unit_رستہ(ثابت حرف *رستہ, حرف **ret_unit);
صحيح cg_رستہ_get_user_unit_بھریا(ثابت حرف *رستہ, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_رستہ_get_user_unit(ثابت حرف *رستہ, حرف **ret_unit) {
        أرجع cg_رستہ_get_user_unit_بھریا(رستہ, ret_unit, عدم);
}
صحيح cg_رستہ_get_machine_name(ثابت حرف *رستہ, حرف **ret_machine);
صحيح cg_رستہ_get_ٹکڑا(ثابت حرف *رستہ, حرف **ret_ٹکڑا);
صحيح cg_رستہ_get_user_ٹکڑا(ثابت حرف *رستہ, حرف **ret_ٹکڑا);

صحيح cg_کھسکاؤ_رستہ(ثابت حرف *cgroup, ثابت حرف *cached_جڑ, ثابت حرف **ret_shifted);
صحيح cg_pid_get_رستہ_shifted(pid_t pid, ثابت حرف *cached_جڑ, حرف **ret_cgroup);
صحيح cg_pidref_get_رستہ_shifted(ثابت PidRef *pidref, ثابت حرف *cached_جڑ, حرف **ret_cgroup);

صحيح cg_pid_get_session(pid_t pid, حرف **ret_session);
صحيح cg_pidref_get_session(ثابت PidRef *pidref, حرف **ret);
صحيح cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
صحيح cg_pidref_get_owner_uid(ثابت PidRef *pidref, uid_t *ret);
صحيح cg_pid_get_unit_بھریا(pid_t pid, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pid_get_unit(pid_t pid, حرف **ret_unit) {
        أرجع cg_pid_get_unit_بھریا(pid, ret_unit, عدم);
}
صحيح cg_pidref_get_unit_بھریا(ثابت PidRef *pidref, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pidref_get_unit(ثابت PidRef *pidref, حرف **ret_unit) {
        أرجع cg_pidref_get_unit_بھریا(pidref, ret_unit, عدم);
}
صحيح cg_pid_get_user_unit_بھریا(pid_t pid, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pid_get_user_unit(pid_t pid, حرف **ret_unit) {
        أرجع cg_pid_get_user_unit_بھریا(pid, ret_unit, عدم);
}
صحيح cg_pidref_get_user_unit_بھریا(ثابت PidRef *pidref, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pidref_get_user_unit(ثابت PidRef *pidref, حرف **ret_unit) {
        أرجع cg_pidref_get_user_unit_بھریا(pidref, ret_unit, عدم);
}
صحيح cg_pid_get_machine_name(pid_t pid, حرف **ret_machine);
صحيح cg_pid_get_ٹکڑا(pid_t pid, حرف **ret_ٹکڑا);
صحيح cg_pid_get_user_ٹکڑا(pid_t pid, حرف **ret_ٹکڑا);

صحيح cg_رستہ_decode_unit(ثابت حرف *cgroup, حرف **ret_unit);

bool cg_needs_escape(ثابت حرف *p) خالص;
صحيح cg_escape(ثابت حرف *p, حرف **ret);
حرف* cg_unescape(ثابت حرف *p) خالص;

صحيح cg_ٹکڑا_منزل_رستہ(ثابت حرف *unit, حرف **ret);

صحيح cg_نقاب_supported(CGroup_نقاب *ret);
صحيح cg_نقاب_supported_subtree(ثابت حرف *جڑ, CGroup_نقاب *ret);
صحيح cg_نقاب_from_string(ثابت حرف *s, CGroup_نقاب *ret);
صحيح cg_نقاب_منزل_string(CGroup_نقاب نقاب_3, حرف **ret);

bool cg_kill_supported(فراغ);

DECLARE_STRING_جدول_LOOKUP(cgroup_controller, CGroupController);

تعريفنوع تعداد ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        MANAGED_OOM_منوال_MAX,
        MANAGED_OOM_منوال_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_جدول_LOOKUP(managed_oom_منوال, ManagedOOMMode);

تعريفنوع تعداد ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_جدول_LOOKUP(managed_oom_preference, ManagedOOMPreference);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

#وصف_ڪريو SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
تعريفنوع تعداد CGroupController {
        /* Original cgroup controllers */
        CGROUP_CONTROLLER_CPU,
        CGROUP_CONTROLLER_CPUACCT,    /* v1 only */
        CGROUP_CONTROLLER_CPUSET,     /* v2 only */
        CGROUP_CONTROLLER_IO,         /* v2 only */
        CGROUP_CONTROLLER_BLKIO,      /* v1 only */
        CGROUP_CONTROLLER_يادگيري,
        CGROUP_CONTROLLER_DEVICES,    /* v1 only */
        CGROUP_CONTROLLER_PIDS,

        /* BPF-based pseudo-controllers, v2 only */
        CGROUP_CONTROLLER_BPF_FIREWALL,
        CGROUP_CONTROLLER_BPF_DEVICES,
        CGROUP_CONTROLLER_BPF_FOREIGN,
        CGROUP_CONTROLLER_BPF_SOCKET_BIND,
        CGROUP_CONTROLLER_BPF_RESTRICT_ڄار_INTERFACES,
        CGROUP_CONTROLLER_BPF_BIND_ڄار_INTERFACE,
        /* The BPF hook implementing RestrictFileSystems= is not defined here.
         * It's applied as late as possible in exec_invoke() so we don't block
         * our own unit setup code. */

        _CGROUP_CONTROLLER_MAX,
        _CGROUP_CONTROLLER_INVALID = -EINVAL,
} CGroupController;

#وصف_ڪريو CGROUP_CONTROLLER_منزل_نقاب(c) (1U << (c))

/* A bit mask of well known cgroup controllers */
تعريفنوع تعداد CGroup_نقاب {
        CGROUP_نقاب_CPU = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_CPU),
        CGROUP_نقاب_CPUACCT = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_CPUACCT),
        CGROUP_نقاب_CPUSET = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_CPUSET),
        CGROUP_نقاب_IO = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_IO),
        CGROUP_نقاب_BLKIO = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_BLKIO),
        CGROUP_نقاب_يادگيري = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_يادگيري),
        CGROUP_نقاب_DEVICES = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_DEVICES),
        CGROUP_نقاب_PIDS = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_PIDS),
        CGROUP_نقاب_BPF_FIREWALL = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_BPF_FIREWALL),
        CGROUP_نقاب_BPF_DEVICES = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_BPF_DEVICES),
        CGROUP_نقاب_BPF_FOREIGN = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_BPF_FOREIGN),
        CGROUP_نقاب_BPF_SOCKET_BIND = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_BPF_SOCKET_BIND),
        CGROUP_نقاب_BPF_RESTRICT_ڄار_INTERFACES = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_BPF_RESTRICT_ڄار_INTERFACES),
        CGROUP_نقاب_BPF_BIND_ڄار_INTERFACE = CGROUP_CONTROLLER_منزل_نقاب(CGROUP_CONTROLLER_BPF_BIND_ڄار_INTERFACE),

        /* All real cgroup v1 controllers */
        CGROUP_نقاب_V1 = CGROUP_نقاب_CPU|CGROUP_نقاب_CPUACCT|CGROUP_نقاب_BLKIO|CGROUP_نقاب_يادگيري|CGROUP_نقاب_DEVICES|CGROUP_نقاب_PIDS,

        /* All real cgroup v2 controllers */
        CGROUP_نقاب_V2 = CGROUP_نقاب_CPU|CGROUP_نقاب_CPUSET|CGROUP_نقاب_IO|CGROUP_نقاب_يادگيري|CGROUP_نقاب_PIDS,

        /* All controllers we want to delegate in case of Delegate=yes. Which are pretty much the v2 controllers only, as delegation on v1 is not safe, and bpf stuff isn't a real controller */
        CGROUP_نقاب_DELEGATE = CGROUP_نقاب_V2,

        /* All cgroup v2 BPF pseudo-controllers */
        CGROUP_نقاب_BPF = CGROUP_نقاب_BPF_FIREWALL|CGROUP_نقاب_BPF_DEVICES|CGROUP_نقاب_BPF_FOREIGN|CGROUP_نقاب_BPF_SOCKET_BIND|CGROUP_نقاب_BPF_RESTRICT_ڄار_INTERFACES|CGROUP_نقاب_BPF_BIND_ڄار_INTERFACE,

        CGROUP_نقاب_ALL = CGROUP_CONTROLLER_منزل_نقاب(_CGROUP_CONTROLLER_MAX) - 1,
} CGroup_نقاب;

/* Special values for all weight knobs on unified hierarchy */
#وصف_ڪريو CGROUP_وزن_INVALID UINT64_MAX
#وصف_ڪريو CGROUP_وزن_IDLE UINT64_C(0)
#وصف_ڪريو CGROUP_وزن_MIN UINT64_C(1)
#وصف_ڪريو CGROUP_وزن_MAX UINT64_C(10000)
#وصف_ڪريو CGROUP_وزن_DEFAULT UINT64_C(100)

#وصف_ڪريو CGROUP_حد_بندي_MIN UINT64_C(0)
#وصف_ڪريو CGROUP_حد_بندي_MAX UINT64_MAX

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

        CGROUP_IO_حد_بندي_TYPE_MAX,
        CGROUP_IO_حد_بندي_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

خارجي ثابت uint64_t cgroup_io_حد_بندي_defaults[CGROUP_IO_حد_بندي_TYPE_MAX];

DECLARE_STRING_جدول_LOOKUP(cgroup_io_حد_بندي_type, CGroupIOLimitType);
فراغ cgroup_io_limits_list(فراغ);

/* Special values for the io.bfq.weight attribute */
#وصف_ڪريو CGROUP_BFQ_وزن_INVALID UINT64_MAX
#وصف_ڪريو CGROUP_BFQ_وزن_MIN UINT64_C(1)
#وصف_ڪريو CGROUP_BFQ_وزن_MAX UINT64_C(1000)
#وصف_ڪريو CGROUP_BFQ_وزن_DEFAULT UINT64_C(100)

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

صحيح cg_رستو_open(ثابت حرف *رستو);
صحيح cg_cgroupid_open(صحيح cgroupfs_fd, uint64_t id);

صحيح cg_رستو_from_cgroupid(صحيح cgroupfs_fd, uint64_t id, حرف **ret);

تعريفنوع تعداد CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

صحيح cg_enumerate_processes(ثابت حرف *رستو, FILE **ret);
صحيح cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
صحيح cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

صحيح cg_enumerate_subgroups(ثابت حرف *رستو, DIR **ret);
صحيح cg_read_subgroup(DIR *d, حرف **ret);

تعريفنوع صحيح (*cg_kill_log_func_t)(ثابت PidRef *pid, صحيح sig, فراغ *userdata);

صحيح cg_kill(ثابت حرف *رستو, صحيح sig, CGroupFlags flags, سيٽ_2 *killed_pids, cg_kill_log_func_t log_kill, فراغ *userdata);
صحيح cg_kill_kernel_sigkill(ثابت حرف *رستو, uint64_t *ret_n_pids_killed);
صحيح cg_kill_recursive(ثابت حرف *رستو, صحيح sig, CGroupFlags flags, سيٽ_2 *killed_pids, cg_kill_log_func_t log_kill, فراغ *userdata);

صحيح cg_ورهايو_spec(ثابت حرف *spec, حرف **ret_controller, حرف **ret_رستو);

صحيح cg_get_رستو(ثابت حرف *رستو, ثابت حرف *suffix, حرف **ret);

صحيح cg_pid_get_رستو(pid_t pid, حرف **ret);
صحيح cg_pidref_get_رستو(ثابت PidRef *pidref, حرف **ret);

صحيح cg_is_threaded(ثابت حرف *رستو);

صحيح cg_is_delegated(ثابت حرف *رستو);
صحيح cg_is_delegated_fd(صحيح fd);

صحيح cg_has_coredump_receive(ثابت حرف *رستو);

صحيح cg_سيٽ_attribute(ثابت حرف *رستو, ثابت حرف *attribute, ثابت حرف *قدر);
صحيح cg_get_attribute(ثابت حرف *رستو, ثابت حرف *attribute, حرف **ret);
صحيح cg_get_attribute_as_uint64(ثابت حرف *رستو, ثابت حرف *attribute, uint64_t *ret);
صحيح cg_get_attribute_as_bool(ثابت حرف *رستو, ثابت حرف *attribute);

صحيح cg_get_keyed_attribute(ثابت حرف *رستو, ثابت حرف *attribute, حرف * ثابت *ڪنجيون, حرف **values);
صحيح cg_get_keyed_attribute_uint64(ثابت حرف *رستو, ثابت حرف *attribute, ثابت حرف *ڪنجي, uint64_t *ret);

صحيح cg_get_owner(ثابت حرف *رستو, uid_t *ret_uid);

صحيح cg_سيٽ_xattr(ثابت حرف *رستو, ثابت حرف *name, ثابت فراغ *قدر, size_t سائيز, صحيح flags);
صحيح cg_get_xattr(ثابت حرف *رستو, ثابت حرف *name, حرف **ret, size_t *ret_سائيز);
/* Returns negative on error, and 0 or 1 on success for the bool value */
صحيح cg_get_xattr_bool(ثابت حرف *رستو, ثابت حرف *name);
صحيح cg_remove_xattr(ثابت حرف *رستو, ثابت حرف *name);

صحيح cg_is_خالي_جايون(ثابت حرف *رستو);

صحيح cg_get_پاڙ_رستو(حرف **رستو);

صحيح cg_رستو_get_session(ثابت حرف *رستو, حرف **ret_session);
صحيح cg_رستو_get_owner_uid(ثابت حرف *رستو, uid_t *ret_uid);
صحيح cg_رستو_get_unit_ڀريل(ثابت حرف *رستو, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_رستو_get_unit(ثابت حرف *رستو, حرف **ret_unit) {
        أرجع cg_رستو_get_unit_ڀريل(رستو, ret_unit, عدم);
}
صحيح cg_رستو_get_unit_رستو(ثابت حرف *رستو, حرف **ret_unit);
صحيح cg_رستو_get_user_unit_ڀريل(ثابت حرف *رستو, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_رستو_get_user_unit(ثابت حرف *رستو, حرف **ret_unit) {
        أرجع cg_رستو_get_user_unit_ڀريل(رستو, ret_unit, عدم);
}
صحيح cg_رستو_get_machine_name(ثابت حرف *رستو, حرف **ret_machine);
صحيح cg_رستو_get_ٽڪرو(ثابت حرف *رستو, حرف **ret_ٽڪرو);
صحيح cg_رستو_get_user_ٽڪرو(ثابت حرف *رستو, حرف **ret_ٽڪرو);

صحيح cg_سرڪايو_رستو(ثابت حرف *cgroup, ثابت حرف *cached_پاڙ, ثابت حرف **ret_shifted);
صحيح cg_pid_get_رستو_shifted(pid_t pid, ثابت حرف *cached_پاڙ, حرف **ret_cgroup);
صحيح cg_pidref_get_رستو_shifted(ثابت PidRef *pidref, ثابت حرف *cached_پاڙ, حرف **ret_cgroup);

صحيح cg_pid_get_session(pid_t pid, حرف **ret_session);
صحيح cg_pidref_get_session(ثابت PidRef *pidref, حرف **ret);
صحيح cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
صحيح cg_pidref_get_owner_uid(ثابت PidRef *pidref, uid_t *ret);
صحيح cg_pid_get_unit_ڀريل(pid_t pid, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pid_get_unit(pid_t pid, حرف **ret_unit) {
        أرجع cg_pid_get_unit_ڀريل(pid, ret_unit, عدم);
}
صحيح cg_pidref_get_unit_ڀريل(ثابت PidRef *pidref, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pidref_get_unit(ثابت PidRef *pidref, حرف **ret_unit) {
        أرجع cg_pidref_get_unit_ڀريل(pidref, ret_unit, عدم);
}
صحيح cg_pid_get_user_unit_ڀريل(pid_t pid, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pid_get_user_unit(pid_t pid, حرف **ret_unit) {
        أرجع cg_pid_get_user_unit_ڀريل(pid, ret_unit, عدم);
}
صحيح cg_pidref_get_user_unit_ڀريل(ثابت PidRef *pidref, حرف **ret_unit, حرف **ret_subgroup);
ساكن مضمن صحيح cg_pidref_get_user_unit(ثابت PidRef *pidref, حرف **ret_unit) {
        أرجع cg_pidref_get_user_unit_ڀريل(pidref, ret_unit, عدم);
}
صحيح cg_pid_get_machine_name(pid_t pid, حرف **ret_machine);
صحيح cg_pid_get_ٽڪرو(pid_t pid, حرف **ret_ٽڪرو);
صحيح cg_pid_get_user_ٽڪرو(pid_t pid, حرف **ret_ٽڪرو);

صحيح cg_رستو_decode_unit(ثابت حرف *cgroup, حرف **ret_unit);

bool cg_needs_escape(ثابت حرف *p) خالص;
صحيح cg_escape(ثابت حرف *p, حرف **ret);
حرف* cg_unescape(ثابت حرف *p) خالص;

صحيح cg_ٽڪرو_منزل_رستو(ثابت حرف *unit, حرف **ret);

صحيح cg_نقاب_supported(CGroup_نقاب *ret);
صحيح cg_نقاب_supported_subtree(ثابت حرف *پاڙ, CGroup_نقاب *ret);
صحيح cg_نقاب_from_string(ثابت حرف *s, CGroup_نقاب *ret);
صحيح cg_نقاب_منزل_string(CGroup_نقاب نقاب_3, حرف **ret);

bool cg_kill_supported(فراغ);

DECLARE_STRING_جدول_LOOKUP(cgroup_controller, CGroupController);

تعريفنوع تعداد ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        MANAGED_OOM_گهڻائي_MAX,
        MANAGED_OOM_گهڻائي_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_جدول_LOOKUP(managed_oom_گهڻائي, ManagedOOMMode);

تعريفنوع تعداد ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_جدول_LOOKUP(managed_oom_preference, ManagedOOMPreference);

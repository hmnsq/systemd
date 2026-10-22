/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

#నిర్వచించు SYSTEMD_CGROUP_CONTROLLER "_systemd"

/* An enum of well known cgroup controllers */
రకనిర్వచనం గణన CGroupController {
        /* Original cgroup controllers */
        CGROUP_CONTROLLER_CPU,
        CGROUP_CONTROLLER_CPUACCT,    /* v1 only */
        CGROUP_CONTROLLER_CPUSET,     /* v2 only */
        CGROUP_CONTROLLER_IO,         /* v2 only */
        CGROUP_CONTROLLER_BLKIO,      /* v1 only */
        CGROUP_CONTROLLER_జ్ఞాపకం,
        CGROUP_CONTROLLER_DEVICES,    /* v1 only */
        CGROUP_CONTROLLER_PIDS,

        /* BPF-based pseudo-controllers, v2 only */
        CGROUP_CONTROLLER_BPF_FIREWALL,
        CGROUP_CONTROLLER_BPF_DEVICES,
        CGROUP_CONTROLLER_BPF_FOREIGN,
        CGROUP_CONTROLLER_BPF_SOCKET_BIND,
        CGROUP_CONTROLLER_BPF_RESTRICT_జాలం_INTERFACES,
        CGROUP_CONTROLLER_BPF_BIND_జాలం_INTERFACE,
        /* The BPF hook implementing RestrictFileSystems= is not defined here.
         * It's applied as late as possible in exec_invoke() so we don't block
         * our own unit setup code. */

        _CGROUP_CONTROLLER_MAX,
        _CGROUP_CONTROLLER_INVALID = -EINVAL,
} CGroupController;

#నిర్వచించు CGROUP_CONTROLLER_గమ్యం_ముసుగు(c) (1U << (c))

/* A bit mask of well known cgroup controllers */
రకనిర్వచనం గణన CGroup_ముసుగు {
        CGROUP_ముసుగు_CPU = CGROUP_CONTROLLER_గమ్యం_ముసుగు(CGROUP_CONTROLLER_CPU),
        CGROUP_ముసుగు_CPUACCT = CGROUP_CONTROLLER_గమ్యం_ముసుగు(CGROUP_CONTROLLER_CPUACCT),
        CGROUP_ముసుగు_CPUSET = CGROUP_CONTROLLER_గమ్యం_ముసుగు(CGROUP_CONTROLLER_CPUSET),
        CGROUP_ముసుగు_IO = CGROUP_CONTROLLER_గమ్యం_ముసుగు(CGROUP_CONTROLLER_IO),
        CGROUP_ముసుగు_BLKIO = CGROUP_CONTROLLER_గమ్యం_ముసుగు(CGROUP_CONTROLLER_BLKIO),
        CGROUP_ముసుగు_జ్ఞాపకం = CGROUP_CONTROLLER_గమ్యం_ముసుగు(CGROUP_CONTROLLER_జ్ఞాపకం),
        CGROUP_ముసుగు_DEVICES = CGROUP_CONTROLLER_గమ్యం_ముసుగు(CGROUP_CONTROLLER_DEVICES),
        CGROUP_ముసుగు_PIDS = CGROUP_CONTROLLER_గమ్యం_ముసుగు(CGROUP_CONTROLLER_PIDS),
        CGROUP_ముసుగు_BPF_FIREWALL = CGROUP_CONTROLLER_గమ్యం_ముసుగు(CGROUP_CONTROLLER_BPF_FIREWALL),
        CGROUP_ముసుగు_BPF_DEVICES = CGROUP_CONTROLLER_గమ్యం_ముసుగు(CGROUP_CONTROLLER_BPF_DEVICES),
        CGROUP_ముసుగు_BPF_FOREIGN = CGROUP_CONTROLLER_గమ్యం_ముసుగు(CGROUP_CONTROLLER_BPF_FOREIGN),
        CGROUP_ముసుగు_BPF_SOCKET_BIND = CGROUP_CONTROLLER_గమ్యం_ముసుగు(CGROUP_CONTROLLER_BPF_SOCKET_BIND),
        CGROUP_ముసుగు_BPF_RESTRICT_జాలం_INTERFACES = CGROUP_CONTROLLER_గమ్యం_ముసుగు(CGROUP_CONTROLLER_BPF_RESTRICT_జాలం_INTERFACES),
        CGROUP_ముసుగు_BPF_BIND_జాలం_INTERFACE = CGROUP_CONTROLLER_గమ్యం_ముసుగు(CGROUP_CONTROLLER_BPF_BIND_జాలం_INTERFACE),

        /* All real cgroup v1 controllers */
        CGROUP_ముసుగు_V1 = CGROUP_ముసుగు_CPU|CGROUP_ముసుగు_CPUACCT|CGROUP_ముసుగు_BLKIO|CGROUP_ముసుగు_జ్ఞాపకం|CGROUP_ముసుగు_DEVICES|CGROUP_ముసుగు_PIDS,

        /* All real cgroup v2 controllers */
        CGROUP_ముసుగు_V2 = CGROUP_ముసుగు_CPU|CGROUP_ముసుగు_CPUSET|CGROUP_ముసుగు_IO|CGROUP_ముసుగు_జ్ఞాపకం|CGROUP_ముసుగు_PIDS,

        /* All controllers we want to delegate in case of Delegate=yes. Which are pretty much the v2 controllers only, as delegation on v1 is not safe, and bpf stuff isn't a real controller */
        CGROUP_ముసుగు_DELEGATE = CGROUP_ముసుగు_V2,

        /* All cgroup v2 BPF pseudo-controllers */
        CGROUP_ముసుగు_BPF = CGROUP_ముసుగు_BPF_FIREWALL|CGROUP_ముసుగు_BPF_DEVICES|CGROUP_ముసుగు_BPF_FOREIGN|CGROUP_ముసుగు_BPF_SOCKET_BIND|CGROUP_ముసుగు_BPF_RESTRICT_జాలం_INTERFACES|CGROUP_ముసుగు_BPF_BIND_జాలం_INTERFACE,

        CGROUP_ముసుగు_ALL = CGROUP_CONTROLLER_గమ్యం_ముసుగు(_CGROUP_CONTROLLER_MAX) - 1,
} CGroup_ముసుగు;

/* Special values for all weight knobs on unified hierarchy */
#నిర్వచించు CGROUP_బరువు_INVALID UINT64_MAX
#నిర్వచించు CGROUP_బరువు_IDLE UINT64_C(0)
#నిర్వచించు CGROUP_బరువు_MIN UINT64_C(1)
#నిర్వచించు CGROUP_బరువు_MAX UINT64_C(10000)
#నిర్వచించు CGROUP_బరువు_DEFAULT UINT64_C(100)

#నిర్వచించు CGROUP_హద్దు_MIN UINT64_C(0)
#నిర్వచించు CGROUP_హద్దు_MAX UINT64_MAX

స్థిర అంతఃవరుస bool CGROUP_బరువు_IS_OK(uint64_t x) {
        తిరిగి
            x == CGROUP_బరువు_INVALID ||
            (x >= CGROUP_బరువు_MIN && x <= CGROUP_బరువు_MAX);
}

/* IO limits on unified hierarchy */
రకనిర్వచనం గణన CGroupIOLimitType {
        CGROUP_IO_RBPS_MAX,
        CGROUP_IO_WBPS_MAX,
        CGROUP_IO_RIOPS_MAX,
        CGROUP_IO_WIOPS_MAX,

        CGROUP_IO_హద్దు_TYPE_MAX,
        CGROUP_IO_హద్దు_TYPE_INVALID = -EINVAL,
} CGroupIOLimitType;

బాహ్య స్థిరరాశి uint64_t cgroup_io_హద్దు_defaults[CGROUP_IO_హద్దు_TYPE_MAX];

DECLARE_STRING_పట్టిక_LOOKUP(cgroup_io_హద్దు_type, CGroupIOLimitType);
శూన్య cgroup_io_limits_list(శూన్య);

/* Special values for the io.bfq.weight attribute */
#నిర్వచించు CGROUP_BFQ_బరువు_INVALID UINT64_MAX
#నిర్వచించు CGROUP_BFQ_బరువు_MIN UINT64_C(1)
#నిర్వచించు CGROUP_BFQ_బరువు_MAX UINT64_C(1000)
#నిర్వచించు CGROUP_BFQ_బరువు_DEFAULT UINT64_C(100)

/* Convert the normal io.weight value to io.bfq.weight */
స్థిర అంతఃవరుస uint64_t BFQ_బరువు(uint64_t io_బరువు) {
        తిరిగి
            io_బరువు <= CGROUP_బరువు_DEFAULT ?
            CGROUP_BFQ_బరువు_DEFAULT - (CGROUP_బరువు_DEFAULT - io_బరువు) * (CGROUP_BFQ_బరువు_DEFAULT - CGROUP_BFQ_బరువు_MIN) / (CGROUP_బరువు_DEFAULT - CGROUP_బరువు_MIN) :
            CGROUP_BFQ_బరువు_DEFAULT + (io_బరువు - CGROUP_బరువు_DEFAULT) * (CGROUP_BFQ_బరువు_MAX - CGROUP_BFQ_బరువు_DEFAULT) / (CGROUP_బరువు_MAX - CGROUP_బరువు_DEFAULT);
}

/*
 * General rules:
 *
 * We require absolute cgroup paths. When returning, we will always
 * generate paths with multiple adjacent / removed.
 */

పూర్ణాంకం cg_is_అందుబాటు(శూన్య);

పూర్ణాంకం cg_మార్గం_open(స్థిరరాశి అక్షరం *మార్గం);
పూర్ణాంకం cg_cgroupid_open(పూర్ణాంకం cgroupfs_fd, uint64_t id);

పూర్ణాంకం cg_మార్గం_from_cgroupid(పూర్ణాంకం cgroupfs_fd, uint64_t id, అక్షరం **ret);

రకనిర్వచనం గణన CGroupFlags {
        CGROUP_SIGCONT            = 1 << 0,
        CGROUP_IGNORE_SELF        = 1 << 1,
        CGROUP_DONT_SKIP_UNMAPPED = 1 << 2,
} CGroupFlags;

పూర్ణాంకం cg_enumerate_processes(స్థిరరాశి అక్షరం *మార్గం, FILE **ret);
పూర్ణాంకం cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags);
పూర్ణాంకం cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags);

పూర్ణాంకం cg_enumerate_subgroups(స్థిరరాశి అక్షరం *మార్గం, DIR **ret);
పూర్ణాంకం cg_read_subgroup(DIR *d, అక్షరం **ret);

రకనిర్వచనం పూర్ణాంకం (*cg_kill_log_func_t)(స్థిరరాశి PidRef *pid, పూర్ణాంకం sig, శూన్య *userdata);

పూర్ణాంకం cg_kill(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం sig, CGroupFlags flags, సమితి_2 *killed_pids, cg_kill_log_func_t log_kill, శూన్య *userdata);
పూర్ణాంకం cg_kill_kernel_sigkill(స్థిరరాశి అక్షరం *మార్గం, uint64_t *ret_n_pids_killed);
పూర్ణాంకం cg_kill_recursive(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం sig, CGroupFlags flags, సమితి_2 *killed_pids, cg_kill_log_func_t log_kill, శూన్య *userdata);

పూర్ణాంకం cg_విభజించు_spec(స్థిరరాశి అక్షరం *spec, అక్షరం **ret_controller, అక్షరం **ret_మార్గం);

పూర్ణాంకం cg_get_మార్గం(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *suffix, అక్షరం **ret);

పూర్ణాంకం cg_pid_get_మార్గం(pid_t pid, అక్షరం **ret);
పూర్ణాంకం cg_pidref_get_మార్గం(స్థిరరాశి PidRef *pidref, అక్షరం **ret);

పూర్ణాంకం cg_is_threaded(స్థిరరాశి అక్షరం *మార్గం);

పూర్ణాంకం cg_is_delegated(స్థిరరాశి అక్షరం *మార్గం);
పూర్ణాంకం cg_is_delegated_fd(పూర్ణాంకం fd);

పూర్ణాంకం cg_has_coredump_receive(స్థిరరాశి అక్షరం *మార్గం);

పూర్ణాంకం cg_సమితి_attribute(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *attribute, స్థిరరాశి అక్షరం *విలువ);
పూర్ణాంకం cg_get_attribute(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *attribute, అక్షరం **ret);
పూర్ణాంకం cg_get_attribute_as_uint64(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *attribute, uint64_t *ret);
పూర్ణాంకం cg_get_attribute_as_bool(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *attribute);

పూర్ణాంకం cg_get_keyed_attribute(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *attribute, అక్షరం * స్థిరరాశి *కీలకాలు, అక్షరం **values);
పూర్ణాంకం cg_get_keyed_attribute_uint64(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *attribute, స్థిరరాశి అక్షరం *కీలకం, uint64_t *ret);

పూర్ణాంకం cg_get_owner(స్థిరరాశి అక్షరం *మార్గం, uid_t *ret_uid);

పూర్ణాంకం cg_సమితి_xattr(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name, స్థిరరాశి శూన్య *విలువ, size_t కొలత, పూర్ణాంకం flags);
పూర్ణాంకం cg_get_xattr(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name, అక్షరం **ret, size_t *ret_కొలత);
/* Returns negative on error, and 0 or 1 on success for the bool value */
పూర్ణాంకం cg_get_xattr_bool(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name);
పూర్ణాంకం cg_remove_xattr(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name);

పూర్ణాంకం cg_is_ఖాళీలు(స్థిరరాశి అక్షరం *మార్గం);

పూర్ణాంకం cg_get_మూలం_మార్గం(అక్షరం **మార్గం);

పూర్ణాంకం cg_మార్గం_get_session(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_session);
పూర్ణాంకం cg_మార్గం_get_owner_uid(స్థిరరాశి అక్షరం *మార్గం, uid_t *ret_uid);
పూర్ణాంకం cg_మార్గం_get_unit_నిండిన(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_unit, అక్షరం **ret_subgroup);
స్థిర అంతఃవరుస పూర్ణాంకం cg_మార్గం_get_unit(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_unit) {
        తిరిగి cg_మార్గం_get_unit_నిండిన(మార్గం, ret_unit, ఖాళీ);
}
పూర్ణాంకం cg_మార్గం_get_unit_మార్గం(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_unit);
పూర్ణాంకం cg_మార్గం_get_user_unit_నిండిన(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_unit, అక్షరం **ret_subgroup);
స్థిర అంతఃవరుస పూర్ణాంకం cg_మార్గం_get_user_unit(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_unit) {
        తిరిగి cg_మార్గం_get_user_unit_నిండిన(మార్గం, ret_unit, ఖాళీ);
}
పూర్ణాంకం cg_మార్గం_get_machine_name(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_machine);
పూర్ణాంకం cg_మార్గం_get_ముక్క(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_ముక్క);
పూర్ణాంకం cg_మార్గం_get_user_ముక్క(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_ముక్క);

పూర్ణాంకం cg_జరుపు_మార్గం(స్థిరరాశి అక్షరం *cgroup, స్థిరరాశి అక్షరం *cached_మూలం, స్థిరరాశి అక్షరం **ret_shifted);
పూర్ణాంకం cg_pid_get_మార్గం_shifted(pid_t pid, స్థిరరాశి అక్షరం *cached_మూలం, అక్షరం **ret_cgroup);
పూర్ణాంకం cg_pidref_get_మార్గం_shifted(స్థిరరాశి PidRef *pidref, స్థిరరాశి అక్షరం *cached_మూలం, అక్షరం **ret_cgroup);

పూర్ణాంకం cg_pid_get_session(pid_t pid, అక్షరం **ret_session);
పూర్ణాంకం cg_pidref_get_session(స్థిరరాశి PidRef *pidref, అక్షరం **ret);
పూర్ణాంకం cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid);
పూర్ణాంకం cg_pidref_get_owner_uid(స్థిరరాశి PidRef *pidref, uid_t *ret);
పూర్ణాంకం cg_pid_get_unit_నిండిన(pid_t pid, అక్షరం **ret_unit, అక్షరం **ret_subgroup);
స్థిర అంతఃవరుస పూర్ణాంకం cg_pid_get_unit(pid_t pid, అక్షరం **ret_unit) {
        తిరిగి cg_pid_get_unit_నిండిన(pid, ret_unit, ఖాళీ);
}
పూర్ణాంకం cg_pidref_get_unit_నిండిన(స్థిరరాశి PidRef *pidref, అక్షరం **ret_unit, అక్షరం **ret_subgroup);
స్థిర అంతఃవరుస పూర్ణాంకం cg_pidref_get_unit(స్థిరరాశి PidRef *pidref, అక్షరం **ret_unit) {
        తిరిగి cg_pidref_get_unit_నిండిన(pidref, ret_unit, ఖాళీ);
}
పూర్ణాంకం cg_pid_get_user_unit_నిండిన(pid_t pid, అక్షరం **ret_unit, అక్షరం **ret_subgroup);
స్థిర అంతఃవరుస పూర్ణాంకం cg_pid_get_user_unit(pid_t pid, అక్షరం **ret_unit) {
        తిరిగి cg_pid_get_user_unit_నిండిన(pid, ret_unit, ఖాళీ);
}
పూర్ణాంకం cg_pidref_get_user_unit_నిండిన(స్థిరరాశి PidRef *pidref, అక్షరం **ret_unit, అక్షరం **ret_subgroup);
స్థిర అంతఃవరుస పూర్ణాంకం cg_pidref_get_user_unit(స్థిరరాశి PidRef *pidref, అక్షరం **ret_unit) {
        తిరిగి cg_pidref_get_user_unit_నిండిన(pidref, ret_unit, ఖాళీ);
}
పూర్ణాంకం cg_pid_get_machine_name(pid_t pid, అక్షరం **ret_machine);
పూర్ణాంకం cg_pid_get_ముక్క(pid_t pid, అక్షరం **ret_ముక్క);
పూర్ణాంకం cg_pid_get_user_ముక్క(pid_t pid, అక్షరం **ret_ముక్క);

పూర్ణాంకం cg_మార్గం_decode_unit(స్థిరరాశి అక్షరం *cgroup, అక్షరం **ret_unit);

bool cg_needs_escape(స్థిరరాశి అక్షరం *p) శుద్ధ;
పూర్ణాంకం cg_escape(స్థిరరాశి అక్షరం *p, అక్షరం **ret);
అక్షరం* cg_unescape(స్థిరరాశి అక్షరం *p) శుద్ధ;

పూర్ణాంకం cg_ముక్క_గమ్యం_మార్గం(స్థిరరాశి అక్షరం *unit, అక్షరం **ret);

పూర్ణాంకం cg_ముసుగు_supported(CGroup_ముసుగు *ret);
పూర్ణాంకం cg_ముసుగు_supported_subtree(స్థిరరాశి అక్షరం *మూలం, CGroup_ముసుగు *ret);
పూర్ణాంకం cg_ముసుగు_from_string(స్థిరరాశి అక్షరం *s, CGroup_ముసుగు *ret);
పూర్ణాంకం cg_ముసుగు_గమ్యం_string(CGroup_ముసుగు ముసుగు_3, అక్షరం **ret);

bool cg_kill_supported(శూన్య);

DECLARE_STRING_పట్టిక_LOOKUP(cgroup_controller, CGroupController);

రకనిర్వచనం గణన ManagedOOMMode {
        MANAGED_OOM_AUTO,
        MANAGED_OOM_KILL,
        MANAGED_OOM_బహుళకం_MAX,
        MANAGED_OOM_బహుళకం_INVALID = -EINVAL,
} ManagedOOMMode;

DECLARE_STRING_పట్టిక_LOOKUP(managed_oom_బహుళకం, ManagedOOMMode);

రకనిర్వచనం గణన ManagedOOMPreference {
        MANAGED_OOM_PREFERENCE_NONE = 0,
        MANAGED_OOM_PREFERENCE_AVOID = 1,
        MANAGED_OOM_PREFERENCE_OMIT = 2,
        _MANAGED_OOM_PREFERENCE_MAX,
        _MANAGED_OOM_PREFERENCE_INVALID = -EINVAL,
} ManagedOOMPreference;

DECLARE_STRING_పట్టిక_LOOKUP(managed_oom_preference, ManagedOOMPreference);

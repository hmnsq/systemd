/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdatomic.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <sys/prctl.h> /* IWYU pragma: keep */
#అన్తర్భావ <sys/syscall.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "bitfield.h"
#అన్తర్భావ "capability-list.h"
#అన్తర్భావ "capability-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "pidref.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "user-util.h"

పూర్ణాంకం capability_get(CapabilityQuintet *ret) {
        ప్రతిజ్ఞా(ret);

        నిర్మాణం __user_cap_header_struct hdr = {
                .సంస్కరణ = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        assert_cc(_LINUX_CAPABILITY_U32S_3 == 2);
        నిర్మాణం __user_cap_data_struct దత్తాంశం[_LINUX_CAPABILITY_U32S_3];
        ఒకవేళ (syscall(SYS_capget, &hdr, దత్తాంశం) < 0)
                తిరిగి -errno;

        *ret = (CapabilityQuintet) {
                .effective = (uint64_t) దత్తాంశం[0].effective | ((uint64_t) దత్తాంశం[1].effective << 32),
                .bounding = UINT64_MAX,
                .inheritable = (uint64_t) దత్తాంశం[0].inheritable | ((uint64_t) దత్తాంశం[1].inheritable << 32),
                .permitted = (uint64_t) దత్తాంశం[0].permitted | ((uint64_t) దత్తాంశం[1].permitted << 32),
                .ambient = UINT64_MAX,
        };
        తిరిగి 0;
}

స్థిర పూర్ణాంకం capability_apply(స్థిరరాశి CapabilityQuintet *q) {
        ప్రతిజ్ఞా(q);

        నిర్మాణం __user_cap_header_struct hdr = {
                .సంస్కరణ = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        నిర్మాణం __user_cap_data_struct దత్తాంశం[_LINUX_CAPABILITY_U32S_3] = {
                {
                        .effective = (uint32_t) (q->effective & UINT32_MAX),
                        .inheritable = (uint32_t) (q->inheritable & UINT32_MAX),
                        .permitted = (uint32_t) (q->permitted & UINT32_MAX),
                },
                {
                        .effective = (uint32_t) (q->effective >> 32),
                        .inheritable = (uint32_t) (q->inheritable >> 32),
                        .permitted = (uint32_t) (q->permitted >> 32),
                },
        };
        తిరిగి RET_NERRNO(syscall(SYS_capset, &hdr, దత్తాంశం));
}

గుర్తులేని cap_చివరి_cap(శూన్య) {
        స్థిర atomic_int saved = గరిష్ఠ_పూర్ణాంకం;
        పూర్ణాంకం r, c;

        c = saved;
        ఒకవేళ (c != గరిష్ఠ_పూర్ణాంకం)
                తిరిగి c;

        /* Available since linux-3.2 */
        _cleanup_free_ అక్షరం *content = ఖాళీ;
        r = read_one_పంక్తి_file("/proc/sys/kernel/cap_last_cap", &content);
        ఒకవేళ (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/cap_last_cap, ignoring: %m");
        లేకపోతే {
                r = సురక్షిత_atoi(content, &c);
                ఒకవేళ (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/kernel/cap_last_cap, ignoring: %m");
                లేకపోతే {
                        ఒకవేళ (c > CAP_హద్దు) /* Safety for the future: if one day the kernel learns more than
                                            * 64 caps, then we are in trouble (since we, as much userspace
                                            * and kernel space store capability masks in uint64_t types). We
                                            * also want to use UINT64_MAX as marker for "unset". Hence let's
                                            * hence protect ourselves against that and always cap at 62 for
                                            * now. */
                                c = CAP_హద్దు;

                        saved = c;
                        తిరిగి c;
                }
        }

        /* Fall back to syscall-probing for pre linux-3.2, or where /proc/ is not mounted */
        గుర్తులేని దీర్ఘ p = (గుర్తులేని దీర్ఘ) MIN(CAP_LAST_CAP, CAP_హద్దు);

        ఒకవేళ (prctl_సురక్షిత(PR_CAPBSET_READ, p, 0, 0, 0) < 0) {

                /* Hmm, look downwards, until we find one that works */
                కొరకు (p--; p > 0; p--)
                        ఒకవేళ (prctl_సురక్షిత(PR_CAPBSET_READ, p, 0, 0, 0) >= 0)
                                ఆపు;

        } లేకపోతే {

                /* Hmm, look upwards, until we find one that doesn't work */
                కొరకు (; p < CAP_హద్దు; p++)
                        ఒకవేళ (prctl_సురక్షిత(PR_CAPBSET_READ, p+1, 0, 0, 0) < 0)
                                ఆపు;
        }

        c = (పూర్ణాంకం) p;
        saved = c;
        తిరిగి c;
}

పూర్ణాంకం have_effective_cap(గుర్తులేని cap) {
        CapabilityQuintet q;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(cap <= CAP_హద్దు);

        r = capability_get(&q);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి బిట్టు_సమితి(q.effective, cap);
}

పూర్ణాంకం have_inheritable_cap(గుర్తులేని cap) {
        CapabilityQuintet q;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(cap <= CAP_హద్దు);

        r = capability_get(&q);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి బిట్టు_సమితి(q.inheritable, cap);
}

పూర్ణాంకం capability_ambient_సమితి_apply(uint64_t సమితి, bool also_inherit) {
        పూర్ణాంకం r;

        /* Remove capabilities requested in ambient set, but not in the bounding set */
        కొరకు (గుర్తులేని i = 0; i <= cap_చివరి_cap(); i++) {
                ఒకవేళ (!బిట్టు_సమితి(సమితి, i))
                        కొనసాగు;

                r = prctl_సురక్షిత(PR_CAPBSET_READ, i, 0, 0, 0);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r != 1) {
                        log_debug("Ambient capability %s requested but missing from bounding set, suppressing automatically.",
                                  capability_గమ్యం_name(i));
                        CLEAR_బిట్టు(సమితి, i);
                }
        }

        /* Add the capabilities to the ambient set (an possibly also the inheritable set) */

        ఒకవేళ (also_inherit) {
                CapabilityQuintet q;

                r = capability_get(&q);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                q.inheritable = సమితి;

                r = capability_apply(&q);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        కొరకు (గుర్తులేని i = 0; i <= cap_చివరి_cap(); i++)
                ఒకవేళ (బిట్టు_సమితి(సమితి, i)) {
                        /* Add the capability to the ambient set. */
                        r = prctl_సురక్షిత(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, i, 0, 0);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                } లేకపోతే {
                        /* Drop the capability so we don't inherit capabilities we didn't ask for. */
                        r = prctl_సురక్షిత(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r > 0) {
                                r = prctl_సురక్షిత(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, i, 0, 0);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;
                        }
                }

        తిరిగి 0;
}

పూర్ణాంకం capability_gain_cap_setpcap(శూన్య) {
        CapabilityQuintet q;
        పూర్ణాంకం r;

        r = capability_get(&q);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (బిట్టు_సమితి(q.effective, CAP_SETPCAP))
                తిరిగి 1; /* We already have capability. */

        సమితి_బిట్టు(q.effective, CAP_SETPCAP);

        r = capability_apply(&q);
        ఒకవేళ (r < 0) {
                /* If we didn't manage to acquire the CAP_SETPCAP bit, we continue anyway, after all this
                 * just means we'll fail later, when we actually intend to drop some capabilities or try to
                 * set securebits. */
                log_debug_errno(r, "Can't acquire effective CAP_SETPCAP bit, ignoring: %m");
                తిరిగి 0;
        }

        తిరిగి 1; /* acquired */
}

పూర్ణాంకం capability_bounding_సమితి_drop(uint64_t keep, bool కుడి_now) {
        పూర్ణాంకం k, r;

        /* If we are run as PID 1 we will lack CAP_SETPCAP by default in the effective set (yes, the kernel
         * drops that when executing init!), so get it back temporarily so that we can call PR_CAPBSET_DROP. */

        CapabilityQuintet q;
        r = capability_get(&q);
        ఒకవేళ (r < 0)
                తిరిగి r;
        CapabilityQuintet saved = q;

        r = capability_gain_cap_setpcap();
        ఒకవేళ (r < 0)
                తిరిగి r;

        కొరకు (గుర్తులేని i = 0; i <= cap_చివరి_cap(); i++) {
                ఒకవేళ (బిట్టు_సమితి(keep, i))
                        కొనసాగు;

                /* Drop it from the bounding set */
                r = prctl_సురక్షిత(PR_CAPBSET_DROP, i, 0, 0, 0);
                ఒకవేళ (r < 0) {
                        /* If dropping the capability failed, let's see if we didn't have it in the first
                         * place. If so, continue anyway, as dropping a capability we didn't have in the
                         * first place doesn't really matter anyway. */
                        ఒకవేళ (prctl_సురక్షిత(PR_CAPBSET_READ, i, 0, 0, 0) != 0)
                                వెళ్ళు ముగింపు;
                }

                /* Also drop it from the inheritable set, so that anything we exec() loses the capability for
                 * good. */
                CLEAR_బిట్టు(q.inheritable, i);

                /* If we shall apply this right now drop it also from our own capability sets. */
                ఒకవేళ (కుడి_now) {
                        CLEAR_బిట్టు(q.effective, i);
                        CLEAR_బిట్టు(q.permitted, i);
                }
        }

        r = 0;

ముగింపు:
        k = capability_apply(&q);
        ఒకవేళ (k < 0)
                /* If there are no actual changes anyway then let's ignore this error. */
                ఒకవేళ (!capability_quintet_equal(&q, &saved))
                        తిరిగి k;

        తిరిగి r;
}

స్థిర పూర్ణాంకం drop_from_file(స్థిరరాశి అక్షరం *fn, uint64_t keep) {
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        uint64_t current, after;
        uint32_t hi, lo;
        పూర్ణాంకం r, k;

        r = read_one_పంక్తి_file(fn, &p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        k = అక్షరశ్రేణి_చదువు(p, "%" PRIu32 " %" PRIu32, &lo, &hi);
        ఒకవేళ (k != 2)
                తిరిగి -EIO;

        current = (uint64_t) lo | ((uint64_t) hi << 32);
        after = current & keep;

        ఒకవేళ (current == after)
                తిరిగి 0;

        lo = after & UINT32_MAX;
        hi = (after >> 32) & UINT32_MAX;

        తిరిగి write_string_filef(fn, 0, "%" PRIu32 " %" PRIu32, lo, hi);
}

పూర్ణాంకం capability_bounding_సమితి_drop_usermode(uint64_t keep) {
        పూర్ణాంకం r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/inheritable", keep);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/bset", keep);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి r;
}

పూర్ణాంకం drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities) {
        పూర్ణాంకం r;

        /* Unfortunately we cannot leave privilege dropping to PID 1 here, since we want to run as user but
         * want to keep some capabilities. Since file capabilities have been introduced this cannot be done
         * across exec() anymore, unless our binary has the capability configured in the file system, which
         * we want to avoid. */

        ఒకవేళ (setresgid(gid, gid, gid) < 0)
                తిరిగి log_error_errno(errno, "Failed to change group ID: %m");

        r = maybe_setgroups(/* size= */ 0, /* list= */ ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి log_error_errno(r, "Failed to drop auxiliary groups list: %m");

        /* Ensure we keep the permitted caps across the setresuid(). Note that we do this even if we actually
         * don't want to keep any capabilities, since we want to be able to drop them from the bounding set
         * too, and we can only do that if we have capabilities. */
        r = prctl_సురక్షిత(PR_SET_KEEPCAPS, 1, 0, 0, 0);
        ఒకవేళ (r < 0)
                తిరిగి log_error_errno(r, "Failed to enable keep capabilities flag: %m");

        ఒకవేళ (setresuid(uid, uid, uid) < 0)
                తిరిగి log_error_errno(errno, "Failed to change user ID: %m");

        r = prctl_సురక్షిత(PR_SET_KEEPCAPS, 0, 0, 0, 0);
        ఒకవేళ (r < 0)
                తిరిగి log_error_errno(r, "Failed to disable keep capabilities flag: %m");

        /* Drop all caps from the bounding set (as well as the inheritable/permitted/effective sets), except
         * the ones we want to keep */
        r = capability_bounding_సమితి_drop(keep_capabilities, /* right_now= */ true);
        ఒకవేళ (r < 0)
                తిరిగి log_error_errno(r, "Failed to drop capabilities: %m");

        /* Now upgrade the permitted caps we still kept to effective caps */
        ఒకవేళ (keep_capabilities != 0) {
                CapabilityQuintet q = {
                        .effective = keep_capabilities,
                        .permitted = keep_capabilities,
                };

                r = capability_apply(&q);
                ఒకవేళ (r < 0)
                        తిరిగి log_error_errno(r, "Failed to increase capabilities: %m");
        }

        తిరిగి 0;
}

స్థిర పూర్ణాంకం change_capability(గుర్తులేని cap, bool b) {
        CapabilityQuintet q;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(cap <= CAP_హద్దు);

        r = capability_get(&q);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (b) {
                సమితి_బిట్టు(q.effective, cap);
                సమితి_బిట్టు(q.permitted, cap);
                సమితి_బిట్టు(q.inheritable, cap);
        } లేకపోతే {
                CLEAR_బిట్టు(q.effective, cap);
                CLEAR_బిట్టు(q.permitted, cap);
                CLEAR_బిట్టు(q.inheritable, cap);
        }

        తిరిగి capability_apply(&q);
}

పూర్ణాంకం drop_capability(గుర్తులేని cap) {
        తిరిగి change_capability(cap, false);
}

పూర్ణాంకం keep_capability(గుర్తులేని cap) {
        తిరిగి change_capability(cap, true);
}

bool capability_quintet_mangle(CapabilityQuintet *q) {
        uint64_t combined, drop = 0;

        ప్రతిజ్ఞా(q);

        combined = q->effective | q->bounding | q->inheritable | q->permitted | q->ambient;

        కొరకు (గుర్తులేని i = 0; i <= cap_చివరి_cap(); i++) {
                ఒకవేళ (!బిట్టు_సమితి(combined, i))
                        కొనసాగు;

                ఒకవేళ (prctl_సురక్షిత(PR_CAPBSET_READ, i, 0, 0, 0) > 0)
                        కొనసాగు;

                సమితి_బిట్టు(drop, i);

                log_debug("Dropping capability not in the current bounding set: %s", capability_గమ్యం_name(i));
        }

        q->effective &= ~drop;
        q->bounding &= ~drop;
        q->inheritable &= ~drop;
        q->permitted &= ~drop;
        q->ambient &= ~drop;

        తిరిగి drop != 0; /* Let the caller know we changed something */
}

పూర్ణాంకం capability_quintet_enforce(స్థిరరాశి CapabilityQuintet *q) {
        CapabilityQuintet c;
        bool modified = false;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(q);

        ఒకవేళ (q->ambient != CAP_ముసుగు_UNSET ||
            q->inheritable != CAP_ముసుగు_UNSET ||
            q->permitted != CAP_ముసుగు_UNSET ||
            q->effective != CAP_ముసుగు_UNSET) {
                r = capability_get(&c);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (q->ambient != CAP_ముసుగు_UNSET) {
                /* In order to raise the ambient caps set we first need to raise the matching
                 * inheritable + permitted cap */
                ఒకవేళ (!FLAGS_సమితి(c.permitted, q->ambient) ||
                    !FLAGS_సమితి(c.inheritable, q->ambient)) {

                        c.permitted |= q->ambient;
                        c.inheritable |= q->ambient;

                        r = capability_apply(&c);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                r = capability_ambient_సమితి_apply(q->ambient, /* also_inherit= */ false);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (q->inheritable != CAP_ముసుగు_UNSET || q->permitted != CAP_ముసుగు_UNSET || q->effective != CAP_ముసుగు_UNSET) {
                ఒకవేళ (!FLAGS_సమితి(c.effective, q->effective) ||
                    !FLAGS_సమితి(c.permitted, q->permitted) ||
                    !FLAGS_సమితి(c.inheritable, q->inheritable)) {

                        c.effective |= q->effective;
                        c.permitted |= q->permitted;
                        c.inheritable |= q->inheritable;

                        /* Now, let's enforce the caps for the first time. Note that this is where we acquire
                         * caps in any of the sets we currently don't have. We have to do this before
                         * dropping the bounding caps below, since at that point we can never acquire new
                         * caps in inherited/permitted/effective anymore, but only lose them.
                         *
                         * In order to change the bounding caps, we need to keep CAP_SETPCAP for a bit
                         * longer. Let's add it to our list hence for now. */
                        ఒకవేళ (q->bounding != CAP_ముసుగు_UNSET &&
                            (!బిట్టు_సమితి(c.effective, CAP_SETPCAP) || !బిట్టు_సమితి(c.permitted, CAP_SETPCAP))) {
                                CapabilityQuintet tmp = c;

                                సమితి_బిట్టు(c.effective, CAP_SETPCAP);
                                సమితి_బిట్టు(c.permitted, CAP_SETPCAP);

                                modified = true;

                                r = capability_apply(&tmp);
                        } లేకపోతే
                                r = capability_apply(&c);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }
        }

        ఒకవేళ (q->bounding != CAP_ముసుగు_UNSET) {
                r = capability_bounding_సమితి_drop(q->bounding, /* right_now= */ false);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        /* If needed, let's now set the caps again, this time in the final version, which differs from what
         * we have already set only in the CAP_SETPCAP bit, which we needed for dropping the bounding
         * bits. This call only undoes bits and doesn't acquire any which means the bounding caps don't
         * matter. */
        ఒకవేళ (modified) {
                r = capability_apply(&c);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి 0;
}

పూర్ణాంకం capability_get_ambient(uint64_t *ret) {
        uint64_t a = 0;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        కొరకు (గుర్తులేని i = 0; i <= cap_చివరి_cap(); i++) {
                r = prctl_సురక్షిత(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r > 0)
                        సమితి_బిట్టు(a, i);
        }

        *ret = a;
        తిరిగి 1;
}

పూర్ణాంకం pidref_get_capability(స్థిరరాశి PidRef *pidref, CapabilityQuintet *ret) {
        పూర్ణాంకం r;

        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి -ESRCH;
        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        స్థిరరాశి అక్షరం *మార్గం = procfs_file_alloca(pidref->pid, "status");
        _cleanup_fclose_ FILE *f = fopen(మార్గం, "re");
        ఒకవేళ (!f) {
                ఒకవేళ (errno == ENOENT && proc_mounted() == 0)
                        తిరిగి -ENOSYS;

                తిరిగి -errno;
        }

        CapabilityQuintet q = CAPABILITY_QUINTET_NULL;
        కొరకు (;;) {
                _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;

                r = read_పంక్తి(f, LONG_పంక్తి_MAX, &పంక్తి);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                స్థిర స్థిరరాశి నిర్మాణం {
                        స్థిరరాశి అక్షరం *క్షేత్రం;
                        size_t స్థానభ్రంశం;
                } fields[] = {
                        { "CapBnd:", offsetof(CapabilityQuintet, bounding)    },
                        { "CapInh:", offsetof(CapabilityQuintet, inheritable) },
                        { "CapPrm:", offsetof(CapabilityQuintet, permitted)   },
                        { "CapEff:", offsetof(CapabilityQuintet, effective)   },
                        { "CapAmb:", offsetof(CapabilityQuintet, ambient)     },
                };

                FOREACH_ELEMENT(i, fields) {

                        స్థిరరాశి అక్షరం *p = మొదటి_మాట(పంక్తి, i->క్షేత్రం);
                        ఒకవేళ (!p)
                                కొనసాగు;

                        uint64_t *v = (uint64_t*) ((uint8_t*) &q + i->స్థానభ్రంశం);

                        ఒకవేళ (*v != CAP_ముసుగు_UNSET)
                                తిరిగి -EBADMSG;

                        r = సురక్షిత_atoux64(p, v);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        ఒకవేళ (*v == CAP_ముసుగు_UNSET)
                                తిరిగి -EBADMSG;
                }
        }

        ఒకవేళ (!capability_quintet_is_fully_సమితి(&q))
                తిరిగి -EBADMSG;

        r = pidref_verify(pidref);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret)
                *ret = q;

        తిరిగి 0;
}

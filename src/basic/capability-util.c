/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdatomic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/prctl.h> /* IWYU pragma: keep */
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bitfield.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "capability-list.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "capability-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user-util.h"

ਪੂਰਨ capability_get(CapabilityQuintet *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਬਣਤਰ __user_cap_header_struct hdr = {
                .ਸੰਸਕਰਣ = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        assert_cc(_LINUX_CAPABILITY_U32S_3 == 2);
        ਬਣਤਰ __user_cap_data_struct ਅੰਕੜੇ[_LINUX_CAPABILITY_U32S_3];
        ਜੇ (syscall(SYS_capget, &hdr, ਅੰਕੜੇ) < 0)
                ਵਾਪਸ -errno;

        *ret = (CapabilityQuintet) {
                .effective = (uint64_t) ਅੰਕੜੇ[0].effective | ((uint64_t) ਅੰਕੜੇ[1].effective << 32),
                .bounding = UINT64_MAX,
                .inheritable = (uint64_t) ਅੰਕੜੇ[0].inheritable | ((uint64_t) ਅੰਕੜੇ[1].inheritable << 32),
                .permitted = (uint64_t) ਅੰਕੜੇ[0].permitted | ((uint64_t) ਅੰਕੜੇ[1].permitted << 32),
                .ambient = UINT64_MAX,
        };
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ capability_apply(ਅਚਲ CapabilityQuintet *q) {
        ਪ੍ਰਤਿਜ੍ਞਾ(q);

        ਬਣਤਰ __user_cap_header_struct hdr = {
                .ਸੰਸਕਰਣ = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        ਬਣਤਰ __user_cap_data_struct ਅੰਕੜੇ[_LINUX_CAPABILITY_U32S_3] = {
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
        ਵਾਪਸ RET_NERRNO(syscall(SYS_capset, &hdr, ਅੰਕੜੇ));
}

ਬਿਨਾਨਿਸ਼ਾਨੀ cap_ਆਖਰੀ_cap(ਖਾਲੀ) {
        ਸਥਿਰ atomic_int saved = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
        ਪੂਰਨ r, c;

        c = saved;
        ਜੇ (c != ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
                ਵਾਪਸ c;

        /* Available since linux-3.2 */
        _cleanup_free_ ਅੱਖਰ *content = ਨਿਰਾ;
        r = read_one_ਸਤਰ_file("/proc/sys/kernel/cap_last_cap", &content);
        ਜੇ (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/cap_last_cap, ignoring: %m");
        ਨਹੀਂਤਾਂ {
                r = ਸੁਰੱਖਿਅਤ_atoi(content, &c);
                ਜੇ (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/kernel/cap_last_cap, ignoring: %m");
                ਨਹੀਂਤਾਂ {
                        ਜੇ (c > CAP_ਹੱਦਬੰਦੀ) /* Safety for the future: if one day the kernel learns more than
                                            * 64 caps, then we are in trouble (since we, as much userspace
                                            * and kernel space store capability masks in uint64_t types). We
                                            * also want to use UINT64_MAX as marker for "unset". Hence let's
                                            * hence protect ourselves against that and always cap at 62 for
                                            * now. */
                                c = CAP_ਹੱਦਬੰਦੀ;

                        saved = c;
                        ਵਾਪਸ c;
                }
        }

        /* Fall back to syscall-probing for pre linux-3.2, or where /proc/ is not mounted */
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ p = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) MIN(CAP_LAST_CAP, CAP_ਹੱਦਬੰਦੀ);

        ਜੇ (prctl_ਸੁਰੱਖਿਅਤ(PR_CAPBSET_READ, p, 0, 0, 0) < 0) {

                /* Hmm, look downwards, until we find one that works */
                ਲਈ (p--; p > 0; p--)
                        ਜੇ (prctl_ਸੁਰੱਖਿਅਤ(PR_CAPBSET_READ, p, 0, 0, 0) >= 0)
                                ਰੋਕੋ;

        } ਨਹੀਂਤਾਂ {

                /* Hmm, look upwards, until we find one that doesn't work */
                ਲਈ (; p < CAP_ਹੱਦਬੰਦੀ; p++)
                        ਜੇ (prctl_ਸੁਰੱਖਿਅਤ(PR_CAPBSET_READ, p+1, 0, 0, 0) < 0)
                                ਰੋਕੋ;
        }

        c = (ਪੂਰਨ) p;
        saved = c;
        ਵਾਪਸ c;
}

ਪੂਰਨ have_effective_cap(ਬਿਨਾਨਿਸ਼ਾਨੀ cap) {
        CapabilityQuintet q;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(cap <= CAP_ਹੱਦਬੰਦੀ);

        r = capability_get(&q);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(q.effective, cap);
}

ਪੂਰਨ have_inheritable_cap(ਬਿਨਾਨਿਸ਼ਾਨੀ cap) {
        CapabilityQuintet q;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(cap <= CAP_ਹੱਦਬੰਦੀ);

        r = capability_get(&q);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(q.inheritable, cap);
}

ਪੂਰਨ capability_ambient_ਸੈੱਟ_apply(uint64_t ਸੈੱਟ, bool also_inherit) {
        ਪੂਰਨ r;

        /* Remove capabilities requested in ambient set, but not in the bounding set */
        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i <= cap_ਆਖਰੀ_cap(); i++) {
                ਜੇ (!ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(ਸੈੱਟ, i))
                        ਜਾਰੀ;

                r = prctl_ਸੁਰੱਖਿਅਤ(PR_CAPBSET_READ, i, 0, 0, 0);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r != 1) {
                        log_debug("Ambient capability %s requested but missing from bounding set, suppressing automatically.",
                                  capability_ਮੰਜ਼ਿਲ_name(i));
                        CLEAR_ਦੋਆਧਾਰੀ_ਅੰਕ(ਸੈੱਟ, i);
                }
        }

        /* Add the capabilities to the ambient set (an possibly also the inheritable set) */

        ਜੇ (also_inherit) {
                CapabilityQuintet q;

                r = capability_get(&q);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                q.inheritable = ਸੈੱਟ;

                r = capability_apply(&q);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i <= cap_ਆਖਰੀ_cap(); i++)
                ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(ਸੈੱਟ, i)) {
                        /* Add the capability to the ambient set. */
                        r = prctl_ਸੁਰੱਖਿਅਤ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, i, 0, 0);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                } ਨਹੀਂਤਾਂ {
                        /* Drop the capability so we don't inherit capabilities we didn't ask for. */
                        r = prctl_ਸੁਰੱਖਿਅਤ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r > 0) {
                                r = prctl_ਸੁਰੱਖਿਅਤ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, i, 0, 0);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;
                        }
                }

        ਵਾਪਸ 0;
}

ਪੂਰਨ capability_gain_cap_setpcap(ਖਾਲੀ) {
        CapabilityQuintet q;
        ਪੂਰਨ r;

        r = capability_get(&q);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(q.effective, CAP_SETPCAP))
                ਵਾਪਸ 1; /* We already have capability. */

        ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(q.effective, CAP_SETPCAP);

        r = capability_apply(&q);
        ਜੇ (r < 0) {
                /* If we didn't manage to acquire the CAP_SETPCAP bit, we continue anyway, after all this
                 * just means we'll fail later, when we actually intend to drop some capabilities or try to
                 * set securebits. */
                log_debug_errno(r, "Can't acquire effective CAP_SETPCAP bit, ignoring: %m");
                ਵਾਪਸ 0;
        }

        ਵਾਪਸ 1; /* acquired */
}

ਪੂਰਨ capability_bounding_ਸੈੱਟ_drop(uint64_t keep, bool ਸੱਜਾ_now) {
        ਪੂਰਨ k, r;

        /* If we are run as PID 1 we will lack CAP_SETPCAP by default in the effective set (yes, the kernel
         * drops that when executing init!), so get it back temporarily so that we can call PR_CAPBSET_DROP. */

        CapabilityQuintet q;
        r = capability_get(&q);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        CapabilityQuintet saved = q;

        r = capability_gain_cap_setpcap();
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i <= cap_ਆਖਰੀ_cap(); i++) {
                ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(keep, i))
                        ਜਾਰੀ;

                /* Drop it from the bounding set */
                r = prctl_ਸੁਰੱਖਿਅਤ(PR_CAPBSET_DROP, i, 0, 0, 0);
                ਜੇ (r < 0) {
                        /* If dropping the capability failed, let's see if we didn't have it in the first
                         * place. If so, continue anyway, as dropping a capability we didn't have in the
                         * first place doesn't really matter anyway. */
                        ਜੇ (prctl_ਸੁਰੱਖਿਅਤ(PR_CAPBSET_READ, i, 0, 0, 0) != 0)
                                ਜਾਓ ਅੰਤ;
                }

                /* Also drop it from the inheritable set, so that anything we exec() loses the capability for
                 * good. */
                CLEAR_ਦੋਆਧਾਰੀ_ਅੰਕ(q.inheritable, i);

                /* If we shall apply this right now drop it also from our own capability sets. */
                ਜੇ (ਸੱਜਾ_now) {
                        CLEAR_ਦੋਆਧਾਰੀ_ਅੰਕ(q.effective, i);
                        CLEAR_ਦੋਆਧਾਰੀ_ਅੰਕ(q.permitted, i);
                }
        }

        r = 0;

ਅੰਤ:
        k = capability_apply(&q);
        ਜੇ (k < 0)
                /* If there are no actual changes anyway then let's ignore this error. */
                ਜੇ (!capability_quintet_equal(&q, &saved))
                        ਵਾਪਸ k;

        ਵਾਪਸ r;
}

ਸਥਿਰ ਪੂਰਨ drop_from_file(ਅਚਲ ਅੱਖਰ *fn, uint64_t keep) {
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        uint64_t current, after;
        uint32_t hi, lo;
        ਪੂਰਨ r, k;

        r = read_one_ਸਤਰ_file(fn, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        k = ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(p, "%" PRIu32 " %" PRIu32, &lo, &hi);
        ਜੇ (k != 2)
                ਵਾਪਸ -EIO;

        current = (uint64_t) lo | ((uint64_t) hi << 32);
        after = current & keep;

        ਜੇ (current == after)
                ਵਾਪਸ 0;

        lo = after & UINT32_MAX;
        hi = (after >> 32) & UINT32_MAX;

        ਵਾਪਸ write_string_filef(fn, 0, "%" PRIu32 " %" PRIu32, lo, hi);
}

ਪੂਰਨ capability_bounding_ਸੈੱਟ_drop_usermode(uint64_t keep) {
        ਪੂਰਨ r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/inheritable", keep);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/bset", keep);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ r;
}

ਪੂਰਨ drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities) {
        ਪੂਰਨ r;

        /* Unfortunately we cannot leave privilege dropping to PID 1 here, since we want to run as user but
         * want to keep some capabilities. Since file capabilities have been introduced this cannot be done
         * across exec() anymore, unless our binary has the capability configured in the file system, which
         * we want to avoid. */

        ਜੇ (setresgid(gid, gid, gid) < 0)
                ਵਾਪਸ log_error_errno(errno, "Failed to change group ID: %m");

        r = maybe_setgroups(/* size= */ 0, /* list= */ ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ log_error_errno(r, "Failed to drop auxiliary groups list: %m");

        /* Ensure we keep the permitted caps across the setresuid(). Note that we do this even if we actually
         * don't want to keep any capabilities, since we want to be able to drop them from the bounding set
         * too, and we can only do that if we have capabilities. */
        r = prctl_ਸੁਰੱਖਿਅਤ(PR_SET_KEEPCAPS, 1, 0, 0, 0);
        ਜੇ (r < 0)
                ਵਾਪਸ log_error_errno(r, "Failed to enable keep capabilities flag: %m");

        ਜੇ (setresuid(uid, uid, uid) < 0)
                ਵਾਪਸ log_error_errno(errno, "Failed to change user ID: %m");

        r = prctl_ਸੁਰੱਖਿਅਤ(PR_SET_KEEPCAPS, 0, 0, 0, 0);
        ਜੇ (r < 0)
                ਵਾਪਸ log_error_errno(r, "Failed to disable keep capabilities flag: %m");

        /* Drop all caps from the bounding set (as well as the inheritable/permitted/effective sets), except
         * the ones we want to keep */
        r = capability_bounding_ਸੈੱਟ_drop(keep_capabilities, /* right_now= */ true);
        ਜੇ (r < 0)
                ਵਾਪਸ log_error_errno(r, "Failed to drop capabilities: %m");

        /* Now upgrade the permitted caps we still kept to effective caps */
        ਜੇ (keep_capabilities != 0) {
                CapabilityQuintet q = {
                        .effective = keep_capabilities,
                        .permitted = keep_capabilities,
                };

                r = capability_apply(&q);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_error_errno(r, "Failed to increase capabilities: %m");
        }

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ change_capability(ਬਿਨਾਨਿਸ਼ਾਨੀ cap, bool b) {
        CapabilityQuintet q;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(cap <= CAP_ਹੱਦਬੰਦੀ);

        r = capability_get(&q);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (b) {
                ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(q.effective, cap);
                ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(q.permitted, cap);
                ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(q.inheritable, cap);
        } ਨਹੀਂਤਾਂ {
                CLEAR_ਦੋਆਧਾਰੀ_ਅੰਕ(q.effective, cap);
                CLEAR_ਦੋਆਧਾਰੀ_ਅੰਕ(q.permitted, cap);
                CLEAR_ਦੋਆਧਾਰੀ_ਅੰਕ(q.inheritable, cap);
        }

        ਵਾਪਸ capability_apply(&q);
}

ਪੂਰਨ drop_capability(ਬਿਨਾਨਿਸ਼ਾਨੀ cap) {
        ਵਾਪਸ change_capability(cap, false);
}

ਪੂਰਨ keep_capability(ਬਿਨਾਨਿਸ਼ਾਨੀ cap) {
        ਵਾਪਸ change_capability(cap, true);
}

bool capability_quintet_mangle(CapabilityQuintet *q) {
        uint64_t combined, drop = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(q);

        combined = q->effective | q->bounding | q->inheritable | q->permitted | q->ambient;

        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i <= cap_ਆਖਰੀ_cap(); i++) {
                ਜੇ (!ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(combined, i))
                        ਜਾਰੀ;

                ਜੇ (prctl_ਸੁਰੱਖਿਅਤ(PR_CAPBSET_READ, i, 0, 0, 0) > 0)
                        ਜਾਰੀ;

                ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(drop, i);

                log_debug("Dropping capability not in the current bounding set: %s", capability_ਮੰਜ਼ਿਲ_name(i));
        }

        q->effective &= ~drop;
        q->bounding &= ~drop;
        q->inheritable &= ~drop;
        q->permitted &= ~drop;
        q->ambient &= ~drop;

        ਵਾਪਸ drop != 0; /* Let the caller know we changed something */
}

ਪੂਰਨ capability_quintet_enforce(ਅਚਲ CapabilityQuintet *q) {
        CapabilityQuintet c;
        bool modified = false;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(q);

        ਜੇ (q->ambient != CAP_ਨਕਾਬ_UNSET ||
            q->inheritable != CAP_ਨਕਾਬ_UNSET ||
            q->permitted != CAP_ਨਕਾਬ_UNSET ||
            q->effective != CAP_ਨਕਾਬ_UNSET) {
                r = capability_get(&c);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (q->ambient != CAP_ਨਕਾਬ_UNSET) {
                /* In order to raise the ambient caps set we first need to raise the matching
                 * inheritable + permitted cap */
                ਜੇ (!FLAGS_ਸੈੱਟ(c.permitted, q->ambient) ||
                    !FLAGS_ਸੈੱਟ(c.inheritable, q->ambient)) {

                        c.permitted |= q->ambient;
                        c.inheritable |= q->ambient;

                        r = capability_apply(&c);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                r = capability_ambient_ਸੈੱਟ_apply(q->ambient, /* also_inherit= */ false);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (q->inheritable != CAP_ਨਕਾਬ_UNSET || q->permitted != CAP_ਨਕਾਬ_UNSET || q->effective != CAP_ਨਕਾਬ_UNSET) {
                ਜੇ (!FLAGS_ਸੈੱਟ(c.effective, q->effective) ||
                    !FLAGS_ਸੈੱਟ(c.permitted, q->permitted) ||
                    !FLAGS_ਸੈੱਟ(c.inheritable, q->inheritable)) {

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
                        ਜੇ (q->bounding != CAP_ਨਕਾਬ_UNSET &&
                            (!ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(c.effective, CAP_SETPCAP) || !ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(c.permitted, CAP_SETPCAP))) {
                                CapabilityQuintet tmp = c;

                                ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(c.effective, CAP_SETPCAP);
                                ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(c.permitted, CAP_SETPCAP);

                                modified = true;

                                r = capability_apply(&tmp);
                        } ਨਹੀਂਤਾਂ
                                r = capability_apply(&c);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }
        }

        ਜੇ (q->bounding != CAP_ਨਕਾਬ_UNSET) {
                r = capability_bounding_ਸੈੱਟ_drop(q->bounding, /* right_now= */ false);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        /* If needed, let's now set the caps again, this time in the final version, which differs from what
         * we have already set only in the CAP_SETPCAP bit, which we needed for dropping the bounding
         * bits. This call only undoes bits and doesn't acquire any which means the bounding caps don't
         * matter. */
        ਜੇ (modified) {
                r = capability_apply(&c);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ capability_get_ambient(uint64_t *ret) {
        uint64_t a = 0;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i <= cap_ਆਖਰੀ_cap(); i++) {
                r = prctl_ਸੁਰੱਖਿਅਤ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r > 0)
                        ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(a, i);
        }

        *ret = a;
        ਵਾਪਸ 1;
}

ਪੂਰਨ pidref_get_capability(ਅਚਲ PidRef *pidref, CapabilityQuintet *ret) {
        ਪੂਰਨ r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ -ESRCH;
        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        ਅਚਲ ਅੱਖਰ *ਰਸਤਾ = procfs_file_alloca(pidref->pid, "status");
        _cleanup_fclose_ FILE *f = fopen(ਰਸਤਾ, "re");
        ਜੇ (!f) {
                ਜੇ (errno == ENOENT && proc_mounted() == 0)
                        ਵਾਪਸ -ENOSYS;

                ਵਾਪਸ -errno;
        }

        CapabilityQuintet q = CAPABILITY_QUINTET_NULL;
        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;

                r = read_ਸਤਰ(f, LONG_ਸਤਰ_MAX, &ਸਤਰ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                ਸਥਿਰ ਅਚਲ ਬਣਤਰ {
                        ਅਚਲ ਅੱਖਰ *ਖੇਤਰ;
                        size_t ਵਿੱਥ;
                } fields[] = {
                        { "CapBnd:", offsetof(CapabilityQuintet, bounding)    },
                        { "CapInh:", offsetof(CapabilityQuintet, inheritable) },
                        { "CapPrm:", offsetof(CapabilityQuintet, permitted)   },
                        { "CapEff:", offsetof(CapabilityQuintet, effective)   },
                        { "CapAmb:", offsetof(CapabilityQuintet, ambient)     },
                };

                FOREACH_ELEMENT(i, fields) {

                        ਅਚਲ ਅੱਖਰ *p = ਪਹਿਲਾ_ਸ਼ਬਦ(ਸਤਰ, i->ਖੇਤਰ);
                        ਜੇ (!p)
                                ਜਾਰੀ;

                        uint64_t *v = (uint64_t*) ((uint8_t*) &q + i->ਵਿੱਥ);

                        ਜੇ (*v != CAP_ਨਕਾਬ_UNSET)
                                ਵਾਪਸ -EBADMSG;

                        r = ਸੁਰੱਖਿਅਤ_atoux64(p, v);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        ਜੇ (*v == CAP_ਨਕਾਬ_UNSET)
                                ਵਾਪਸ -EBADMSG;
                }
        }

        ਜੇ (!capability_quintet_is_fully_ਸੈੱਟ(&q))
                ਵਾਪਸ -EBADMSG;

        r = pidref_verify(pidref);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret)
                *ret = q;

        ਵਾਪਸ 0;
}

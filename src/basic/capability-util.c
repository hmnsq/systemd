/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdatomic.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <sys/prctl.h> /* IWYU pragma: keep */
#ಅನ್ತರ್ಭಾವ <sys/syscall.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "bitfield.h"
#ಅನ್ತರ್ಭಾವ "capability-list.h"
#ಅನ್ತರ್ಭಾವ "capability-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "pidref.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"

ಪೂರ್ಣಾಂಕ capability_get(CapabilityQuintet *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ರಚನೆ __user_cap_header_struct hdr = {
                .ಆವೃತ್ತಿ = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        assert_cc(_LINUX_CAPABILITY_U32S_3 == 2);
        ರಚನೆ __user_cap_data_struct ದತ್ತಾಂಶ[_LINUX_CAPABILITY_U32S_3];
        ಆದರೆ (syscall(SYS_capget, &hdr, ದತ್ತಾಂಶ) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        *ret = (CapabilityQuintet) {
                .effective = (uint64_t) ದತ್ತಾಂಶ[0].effective | ((uint64_t) ದತ್ತಾಂಶ[1].effective << 32),
                .bounding = UINT64_MAX,
                .inheritable = (uint64_t) ದತ್ತಾಂಶ[0].inheritable | ((uint64_t) ದತ್ತಾಂಶ[1].inheritable << 32),
                .permitted = (uint64_t) ದತ್ತಾಂಶ[0].permitted | ((uint64_t) ದತ್ತಾಂಶ[1].permitted << 32),
                .ambient = UINT64_MAX,
        };
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ capability_apply(ಸ್ಥಿರಾಂಕ CapabilityQuintet *q) {
        ಪ್ರತಿಜ್ಞಾ(q);

        ರಚನೆ __user_cap_header_struct hdr = {
                .ಆವೃತ್ತಿ = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        ರಚನೆ __user_cap_data_struct ದತ್ತಾಂಶ[_LINUX_CAPABILITY_U32S_3] = {
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
        ಹಿಂತಿರುಗಿ RET_NERRNO(syscall(SYS_capset, &hdr, ದತ್ತಾಂಶ));
}

ಚಿಹ್ನೆರಹಿತ cap_ಕೊನೆಯ_cap(ಶೂನ್ಯ) {
        ಸ್ಥಿರ atomic_int saved = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
        ಪೂರ್ಣಾಂಕ r, c;

        c = saved;
        ಆದರೆ (c != ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
                ಹಿಂತಿರುಗಿ c;

        /* Available since linux-3.2 */
        _cleanup_free_ ಅಕ್ಷರ *content = ಖಾಲಿ;
        r = read_one_ಸಾಲು_file("/proc/sys/kernel/cap_last_cap", &content);
        ಆದರೆ (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/cap_last_cap, ignoring: %m");
        ಇಲ್ಲದಿದ್ದರೆ {
                r = ಸುರಕ್ಷಿತ_atoi(content, &c);
                ಆದರೆ (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/kernel/cap_last_cap, ignoring: %m");
                ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ (c > CAP_ಎಲ್ಲೆ) /* Safety for the future: if one day the kernel learns more than
                                            * 64 caps, then we are in trouble (since we, as much userspace
                                            * and kernel space store capability masks in uint64_t types). We
                                            * also want to use UINT64_MAX as marker for "unset". Hence let's
                                            * hence protect ourselves against that and always cap at 62 for
                                            * now. */
                                c = CAP_ಎಲ್ಲೆ;

                        saved = c;
                        ಹಿಂತಿರುಗಿ c;
                }
        }

        /* Fall back to syscall-probing for pre linux-3.2, or where /proc/ is not mounted */
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ p = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) MIN(CAP_LAST_CAP, CAP_ಎಲ್ಲೆ);

        ಆದರೆ (prctl_ಸುರಕ್ಷಿತ(PR_CAPBSET_READ, p, 0, 0, 0) < 0) {

                /* Hmm, look downwards, until we find one that works */
                ಗಾಗಿ (p--; p > 0; p--)
                        ಆದರೆ (prctl_ಸುರಕ್ಷಿತ(PR_CAPBSET_READ, p, 0, 0, 0) >= 0)
                                ನಿಲ್ಲಿಸು;

        } ಇಲ್ಲದಿದ್ದರೆ {

                /* Hmm, look upwards, until we find one that doesn't work */
                ಗಾಗಿ (; p < CAP_ಎಲ್ಲೆ; p++)
                        ಆದರೆ (prctl_ಸುರಕ್ಷಿತ(PR_CAPBSET_READ, p+1, 0, 0, 0) < 0)
                                ನಿಲ್ಲಿಸು;
        }

        c = (ಪೂರ್ಣಾಂಕ) p;
        saved = c;
        ಹಿಂತಿರುಗಿ c;
}

ಪೂರ್ಣಾಂಕ have_effective_cap(ಚಿಹ್ನೆರಹಿತ cap) {
        CapabilityQuintet q;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(cap <= CAP_ಎಲ್ಲೆ);

        r = capability_get(&q);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ಬಿಟ್_ಗಣ(q.effective, cap);
}

ಪೂರ್ಣಾಂಕ have_inheritable_cap(ಚಿಹ್ನೆರಹಿತ cap) {
        CapabilityQuintet q;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(cap <= CAP_ಎಲ್ಲೆ);

        r = capability_get(&q);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ಬಿಟ್_ಗಣ(q.inheritable, cap);
}

ಪೂರ್ಣಾಂಕ capability_ambient_ಗಣ_apply(uint64_t ಗಣ, bool also_inherit) {
        ಪೂರ್ಣಾಂಕ r;

        /* Remove capabilities requested in ambient set, but not in the bounding set */
        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i <= cap_ಕೊನೆಯ_cap(); i++) {
                ಆದರೆ (!ಬಿಟ್_ಗಣ(ಗಣ, i))
                        ಮುಂದುವರಿ;

                r = prctl_ಸುರಕ್ಷಿತ(PR_CAPBSET_READ, i, 0, 0, 0);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r != 1) {
                        log_debug("Ambient capability %s requested but missing from bounding set, suppressing automatically.",
                                  capability_ಗಮ್ಯ_name(i));
                        CLEAR_ಬಿಟ್(ಗಣ, i);
                }
        }

        /* Add the capabilities to the ambient set (an possibly also the inheritable set) */

        ಆದರೆ (also_inherit) {
                CapabilityQuintet q;

                r = capability_get(&q);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                q.inheritable = ಗಣ;

                r = capability_apply(&q);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i <= cap_ಕೊನೆಯ_cap(); i++)
                ಆದರೆ (ಬಿಟ್_ಗಣ(ಗಣ, i)) {
                        /* Add the capability to the ambient set. */
                        r = prctl_ಸುರಕ್ಷಿತ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, i, 0, 0);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                } ಇಲ್ಲದಿದ್ದರೆ {
                        /* Drop the capability so we don't inherit capabilities we didn't ask for. */
                        r = prctl_ಸುರಕ್ಷಿತ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r > 0) {
                                r = prctl_ಸುರಕ್ಷಿತ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, i, 0, 0);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;
                        }
                }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ capability_gain_cap_setpcap(ಶೂನ್ಯ) {
        CapabilityQuintet q;
        ಪೂರ್ಣಾಂಕ r;

        r = capability_get(&q);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ಬಿಟ್_ಗಣ(q.effective, CAP_SETPCAP))
                ಹಿಂತಿರುಗಿ 1; /* We already have capability. */

        ಗಣ_ಬಿಟ್(q.effective, CAP_SETPCAP);

        r = capability_apply(&q);
        ಆದರೆ (r < 0) {
                /* If we didn't manage to acquire the CAP_SETPCAP bit, we continue anyway, after all this
                 * just means we'll fail later, when we actually intend to drop some capabilities or try to
                 * set securebits. */
                log_debug_errno(r, "Can't acquire effective CAP_SETPCAP bit, ignoring: %m");
                ಹಿಂತಿರುಗಿ 0;
        }

        ಹಿಂತಿರುಗಿ 1; /* acquired */
}

ಪೂರ್ಣಾಂಕ capability_bounding_ಗಣ_drop(uint64_t keep, bool ಬಲ_now) {
        ಪೂರ್ಣಾಂಕ k, r;

        /* If we are run as PID 1 we will lack CAP_SETPCAP by default in the effective set (yes, the kernel
         * drops that when executing init!), so get it back temporarily so that we can call PR_CAPBSET_DROP. */

        CapabilityQuintet q;
        r = capability_get(&q);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        CapabilityQuintet saved = q;

        r = capability_gain_cap_setpcap();
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i <= cap_ಕೊನೆಯ_cap(); i++) {
                ಆದರೆ (ಬಿಟ್_ಗಣ(keep, i))
                        ಮುಂದುವರಿ;

                /* Drop it from the bounding set */
                r = prctl_ಸುರಕ್ಷಿತ(PR_CAPBSET_DROP, i, 0, 0, 0);
                ಆದರೆ (r < 0) {
                        /* If dropping the capability failed, let's see if we didn't have it in the first
                         * place. If so, continue anyway, as dropping a capability we didn't have in the
                         * first place doesn't really matter anyway. */
                        ಆದರೆ (prctl_ಸುರಕ್ಷಿತ(PR_CAPBSET_READ, i, 0, 0, 0) != 0)
                                ಹೋಗು ಮುಕ್ತಾಯ;
                }

                /* Also drop it from the inheritable set, so that anything we exec() loses the capability for
                 * good. */
                CLEAR_ಬಿಟ್(q.inheritable, i);

                /* If we shall apply this right now drop it also from our own capability sets. */
                ಆದರೆ (ಬಲ_now) {
                        CLEAR_ಬಿಟ್(q.effective, i);
                        CLEAR_ಬಿಟ್(q.permitted, i);
                }
        }

        r = 0;

ಮುಕ್ತಾಯ:
        k = capability_apply(&q);
        ಆದರೆ (k < 0)
                /* If there are no actual changes anyway then let's ignore this error. */
                ಆದರೆ (!capability_quintet_equal(&q, &saved))
                        ಹಿಂತಿರುಗಿ k;

        ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ drop_from_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, uint64_t keep) {
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        uint64_t current, after;
        uint32_t hi, lo;
        ಪೂರ್ಣಾಂಕ r, k;

        r = read_one_ಸಾಲು_file(fn, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        k = ಅಕ್ಷರಸರಣಿ_ಓದು(p, "%" PRIu32 " %" PRIu32, &lo, &hi);
        ಆದರೆ (k != 2)
                ಹಿಂತಿರುಗಿ -EIO;

        current = (uint64_t) lo | ((uint64_t) hi << 32);
        after = current & keep;

        ಆದರೆ (current == after)
                ಹಿಂತಿರುಗಿ 0;

        lo = after & UINT32_MAX;
        hi = (after >> 32) & UINT32_MAX;

        ಹಿಂತಿರುಗಿ write_string_filef(fn, 0, "%" PRIu32 " %" PRIu32, lo, hi);
}

ಪೂರ್ಣಾಂಕ capability_bounding_ಗಣ_drop_usermode(uint64_t keep) {
        ಪೂರ್ಣಾಂಕ r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/inheritable", keep);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/bset", keep);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities) {
        ಪೂರ್ಣಾಂಕ r;

        /* Unfortunately we cannot leave privilege dropping to PID 1 here, since we want to run as user but
         * want to keep some capabilities. Since file capabilities have been introduced this cannot be done
         * across exec() anymore, unless our binary has the capability configured in the file system, which
         * we want to avoid. */

        ಆದರೆ (setresgid(gid, gid, gid) < 0)
                ಹಿಂತಿರುಗಿ log_error_errno(errno, "Failed to change group ID: %m");

        r = maybe_setgroups(/* size= */ 0, /* list= */ ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_error_errno(r, "Failed to drop auxiliary groups list: %m");

        /* Ensure we keep the permitted caps across the setresuid(). Note that we do this even if we actually
         * don't want to keep any capabilities, since we want to be able to drop them from the bounding set
         * too, and we can only do that if we have capabilities. */
        r = prctl_ಸುರಕ್ಷಿತ(PR_SET_KEEPCAPS, 1, 0, 0, 0);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_error_errno(r, "Failed to enable keep capabilities flag: %m");

        ಆದರೆ (setresuid(uid, uid, uid) < 0)
                ಹಿಂತಿರುಗಿ log_error_errno(errno, "Failed to change user ID: %m");

        r = prctl_ಸುರಕ್ಷಿತ(PR_SET_KEEPCAPS, 0, 0, 0, 0);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_error_errno(r, "Failed to disable keep capabilities flag: %m");

        /* Drop all caps from the bounding set (as well as the inheritable/permitted/effective sets), except
         * the ones we want to keep */
        r = capability_bounding_ಗಣ_drop(keep_capabilities, /* right_now= */ true);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_error_errno(r, "Failed to drop capabilities: %m");

        /* Now upgrade the permitted caps we still kept to effective caps */
        ಆದರೆ (keep_capabilities != 0) {
                CapabilityQuintet q = {
                        .effective = keep_capabilities,
                        .permitted = keep_capabilities,
                };

                r = capability_apply(&q);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_error_errno(r, "Failed to increase capabilities: %m");
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ change_capability(ಚಿಹ್ನೆರಹಿತ cap, bool b) {
        CapabilityQuintet q;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(cap <= CAP_ಎಲ್ಲೆ);

        r = capability_get(&q);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (b) {
                ಗಣ_ಬಿಟ್(q.effective, cap);
                ಗಣ_ಬಿಟ್(q.permitted, cap);
                ಗಣ_ಬಿಟ್(q.inheritable, cap);
        } ಇಲ್ಲದಿದ್ದರೆ {
                CLEAR_ಬಿಟ್(q.effective, cap);
                CLEAR_ಬಿಟ್(q.permitted, cap);
                CLEAR_ಬಿಟ್(q.inheritable, cap);
        }

        ಹಿಂತಿರುಗಿ capability_apply(&q);
}

ಪೂರ್ಣಾಂಕ drop_capability(ಚಿಹ್ನೆರಹಿತ cap) {
        ಹಿಂತಿರುಗಿ change_capability(cap, false);
}

ಪೂರ್ಣಾಂಕ keep_capability(ಚಿಹ್ನೆರಹಿತ cap) {
        ಹಿಂತಿರುಗಿ change_capability(cap, true);
}

bool capability_quintet_mangle(CapabilityQuintet *q) {
        uint64_t combined, drop = 0;

        ಪ್ರತಿಜ್ಞಾ(q);

        combined = q->effective | q->bounding | q->inheritable | q->permitted | q->ambient;

        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i <= cap_ಕೊನೆಯ_cap(); i++) {
                ಆದರೆ (!ಬಿಟ್_ಗಣ(combined, i))
                        ಮುಂದುವರಿ;

                ಆದರೆ (prctl_ಸುರಕ್ಷಿತ(PR_CAPBSET_READ, i, 0, 0, 0) > 0)
                        ಮುಂದುವರಿ;

                ಗಣ_ಬಿಟ್(drop, i);

                log_debug("Dropping capability not in the current bounding set: %s", capability_ಗಮ್ಯ_name(i));
        }

        q->effective &= ~drop;
        q->bounding &= ~drop;
        q->inheritable &= ~drop;
        q->permitted &= ~drop;
        q->ambient &= ~drop;

        ಹಿಂತಿರುಗಿ drop != 0; /* Let the caller know we changed something */
}

ಪೂರ್ಣಾಂಕ capability_quintet_enforce(ಸ್ಥಿರಾಂಕ CapabilityQuintet *q) {
        CapabilityQuintet c;
        bool modified = false;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(q);

        ಆದರೆ (q->ambient != CAP_ಮುಖವಾಡ_UNSET ||
            q->inheritable != CAP_ಮುಖವಾಡ_UNSET ||
            q->permitted != CAP_ಮುಖವಾಡ_UNSET ||
            q->effective != CAP_ಮುಖವಾಡ_UNSET) {
                r = capability_get(&c);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (q->ambient != CAP_ಮುಖವಾಡ_UNSET) {
                /* In order to raise the ambient caps set we first need to raise the matching
                 * inheritable + permitted cap */
                ಆದರೆ (!FLAGS_ಗಣ(c.permitted, q->ambient) ||
                    !FLAGS_ಗಣ(c.inheritable, q->ambient)) {

                        c.permitted |= q->ambient;
                        c.inheritable |= q->ambient;

                        r = capability_apply(&c);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                r = capability_ambient_ಗಣ_apply(q->ambient, /* also_inherit= */ false);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (q->inheritable != CAP_ಮುಖವಾಡ_UNSET || q->permitted != CAP_ಮುಖವಾಡ_UNSET || q->effective != CAP_ಮುಖವಾಡ_UNSET) {
                ಆದರೆ (!FLAGS_ಗಣ(c.effective, q->effective) ||
                    !FLAGS_ಗಣ(c.permitted, q->permitted) ||
                    !FLAGS_ಗಣ(c.inheritable, q->inheritable)) {

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
                        ಆದರೆ (q->bounding != CAP_ಮುಖವಾಡ_UNSET &&
                            (!ಬಿಟ್_ಗಣ(c.effective, CAP_SETPCAP) || !ಬಿಟ್_ಗಣ(c.permitted, CAP_SETPCAP))) {
                                CapabilityQuintet tmp = c;

                                ಗಣ_ಬಿಟ್(c.effective, CAP_SETPCAP);
                                ಗಣ_ಬಿಟ್(c.permitted, CAP_SETPCAP);

                                modified = true;

                                r = capability_apply(&tmp);
                        } ಇಲ್ಲದಿದ್ದರೆ
                                r = capability_apply(&c);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }
        }

        ಆದರೆ (q->bounding != CAP_ಮುಖವಾಡ_UNSET) {
                r = capability_bounding_ಗಣ_drop(q->bounding, /* right_now= */ false);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        /* If needed, let's now set the caps again, this time in the final version, which differs from what
         * we have already set only in the CAP_SETPCAP bit, which we needed for dropping the bounding
         * bits. This call only undoes bits and doesn't acquire any which means the bounding caps don't
         * matter. */
        ಆದರೆ (modified) {
                r = capability_apply(&c);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ capability_get_ambient(uint64_t *ret) {
        uint64_t a = 0;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i <= cap_ಕೊನೆಯ_cap(); i++) {
                r = prctl_ಸುರಕ್ಷಿತ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r > 0)
                        ಗಣ_ಬಿಟ್(a, i);
        }

        *ret = a;
        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ pidref_get_capability(ಸ್ಥಿರಾಂಕ PidRef *pidref, CapabilityQuintet *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ = procfs_file_alloca(pidref->pid, "status");
        _cleanup_fclose_ FILE *f = fopen(ಮಾರ್ಗ, "re");
        ಆದರೆ (!f) {
                ಆದರೆ (errno == ENOENT && proc_mounted() == 0)
                        ಹಿಂತಿರುಗಿ -ENOSYS;

                ಹಿಂತಿರುಗಿ -errno;
        }

        CapabilityQuintet q = CAPABILITY_QUINTET_NULL;
        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;

                r = read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ {
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ;
                        size_t ಸ್ಥಾನಾಂತರ;
                } fields[] = {
                        { "CapBnd:", offsetof(CapabilityQuintet, bounding)    },
                        { "CapInh:", offsetof(CapabilityQuintet, inheritable) },
                        { "CapPrm:", offsetof(CapabilityQuintet, permitted)   },
                        { "CapEff:", offsetof(CapabilityQuintet, effective)   },
                        { "CapAmb:", offsetof(CapabilityQuintet, ambient)     },
                };

                FOREACH_ELEMENT(i, fields) {

                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಮೊದಲ_ಶಬ್ದ(ಸಾಲು, i->ಕ್ಷೇತ್ರ);
                        ಆದರೆ (!p)
                                ಮುಂದುವರಿ;

                        uint64_t *v = (uint64_t*) ((uint8_t*) &q + i->ಸ್ಥಾನಾಂತರ);

                        ಆದರೆ (*v != CAP_ಮುಖವಾಡ_UNSET)
                                ಹಿಂತಿರುಗಿ -EBADMSG;

                        r = ಸುರಕ್ಷಿತ_atoux64(p, v);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        ಆದರೆ (*v == CAP_ಮುಖವಾಡ_UNSET)
                                ಹಿಂತಿರುಗಿ -EBADMSG;
                }
        }

        ಆದರೆ (!capability_quintet_is_fully_ಗಣ(&q))
                ಹಿಂತಿರುಗಿ -EBADMSG;

        r = pidref_verify(pidref);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret)
                *ret = q;

        ಹಿಂತಿರುಗಿ 0;
}

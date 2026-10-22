/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdatomic.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <sys/prctl.h> /* IWYU pragma: keep */
#ଅନ୍ତର୍ଭାଵ <sys/syscall.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "bitfield.h"
#ଅନ୍ତର୍ଭାଵ "capability-list.h"
#ଅନ୍ତର୍ଭାଵ "capability-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "pidref.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "user-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_get(CapabilityQuintet *ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        ସଂରଚନା __user_cap_header_struct hdr = {
                .version = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        assert_cc(_LINUX_CAPABILITY_U32S_3 == 2);
        ସଂରଚନା __user_cap_data_struct data[_LINUX_CAPABILITY_U32S_3];
        ଯଦି (syscall(SYS_capget, &hdr, data) < 0)
                ଫେରନ୍ତୁ -errno;

        *ret = (CapabilityQuintet) {
                .effective = (uint64_t) data[0].effective | ((uint64_t) data[1].effective << 32),
                .bounding = UINT64_MAX,
                .inheritable = (uint64_t) data[0].inheritable | ((uint64_t) data[1].inheritable << 32),
                .permitted = (uint64_t) data[0].permitted | ((uint64_t) data[1].permitted << 32),
                .ambient = UINT64_MAX,
        };
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_apply(ଅଚଳ CapabilityQuintet *q) {
        ପ୍ରତିଜ୍ଞା(q);

        ସଂରଚନା __user_cap_header_struct hdr = {
                .version = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        ସଂରଚନା __user_cap_data_struct data[_LINUX_CAPABILITY_U32S_3] = {
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
        ଫେରନ୍ତୁ RET_NERRNO(syscall(SYS_capset, &hdr, data));
}

unsigned cap_last_cap(ଶୂନ୍ୟ) {
        ସ୍ଥିର atomic_int saved = INT_MAX;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, c;

        c = saved;
        ଯଦି (c != INT_MAX)
                ଫେରନ୍ତୁ c;

        /* Available since linux-3.2 */
        _cleanup_free_ ଅକ୍ଷର *content = ଖାଲି;
        r = read_one_ଧାଡ଼ି_file("/proc/sys/kernel/cap_last_cap", &content);
        ଯଦି (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/cap_last_cap, ignoring: %m");
        ଅନ୍ୟଥା {
                r = safe_atoi(content, &c);
                ଯଦି (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/kernel/cap_last_cap, ignoring: %m");
                ଅନ୍ୟଥା {
                        ଯଦି (c > CAP_LIMIT) /* Safety for the future: if one day the kernel learns more than
                                            * 64 caps, then we are in trouble (since we, as much userspace
                                            * and kernel space store capability masks in uint64_t types). We
                                            * also want to use UINT64_MAX as marker for "unset". Hence let's
                                            * hence protect ourselves against that and always cap at 62 for
                                            * now. */
                                c = CAP_LIMIT;

                        saved = c;
                        ଫେରନ୍ତୁ c;
                }
        }

        /* Fall back to syscall-probing for pre linux-3.2, or where /proc/ is not mounted */
        unsigned long p = (unsigned long) MIN(CAP_LAST_CAP, CAP_LIMIT);

        ଯଦି (prctl_safe(PR_CAPBSET_READ, p, 0, 0, 0) < 0) {

                /* Hmm, look downwards, until we find one that works */
                ପାଇଁ (p--; p > 0; p--)
                        ଯଦି (prctl_safe(PR_CAPBSET_READ, p, 0, 0, 0) >= 0)
                                ଭାଙ୍ଗ;

        } ଅନ୍ୟଥା {

                /* Hmm, look upwards, until we find one that doesn't work */
                ପାଇଁ (; p < CAP_LIMIT; p++)
                        ଯଦି (prctl_safe(PR_CAPBSET_READ, p+1, 0, 0, 0) < 0)
                                ଭାଙ୍ଗ;
        }

        c = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) p;
        saved = c;
        ଫେରନ୍ତୁ c;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା have_effective_cap(unsigned cap) {
        CapabilityQuintet q;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(cap <= CAP_LIMIT);

        r = capability_get(&q);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ BIT_SET(q.effective, cap);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା have_inheritable_cap(unsigned cap) {
        CapabilityQuintet q;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(cap <= CAP_LIMIT);

        r = capability_get(&q);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ BIT_SET(q.inheritable, cap);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_ambient_set_apply(uint64_t set, bool also_inherit) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Remove capabilities requested in ambient set, but not in the bounding set */
        ପାଇଁ (unsigned i = 0; i <= cap_last_cap(); i++) {
                ଯଦି (!BIT_SET(set, i))
                        ଜାରି;

                r = prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r != 1) {
                        log_debug("Ambient capability %s requested but missing from bounding set, suppressing automatically.",
                                  capability_to_name(i));
                        CLEAR_BIT(set, i);
                }
        }

        /* Add the capabilities to the ambient set (an possibly also the inheritable set) */

        ଯଦି (also_inherit) {
                CapabilityQuintet q;

                r = capability_get(&q);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                q.inheritable = set;

                r = capability_apply(&q);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ପାଇଁ (unsigned i = 0; i <= cap_last_cap(); i++)
                ଯଦି (BIT_SET(set, i)) {
                        /* Add the capability to the ambient set. */
                        r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, i, 0, 0);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                } ଅନ୍ୟଥା {
                        /* Drop the capability so we don't inherit capabilities we didn't ask for. */
                        r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r > 0) {
                                r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, i, 0, 0);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;
                        }
                }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_gain_cap_setpcap(ଶୂନ୍ୟ) {
        CapabilityQuintet q;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = capability_get(&q);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (BIT_SET(q.effective, CAP_SETPCAP))
                ଫେରନ୍ତୁ 1; /* We already have capability. */

        SET_BIT(q.effective, CAP_SETPCAP);

        r = capability_apply(&q);
        ଯଦି (r < 0) {
                /* If we didn't manage to acquire the CAP_SETPCAP bit, we continue anyway, after all this
                 * just means we'll fail later, when we actually intend to drop some capabilities or try to
                 * set securebits. */
                log_debug_errno(r, "Can't acquire effective CAP_SETPCAP bit, ignoring: %m");
                ଫେରନ୍ତୁ 0;
        }

        ଫେରନ୍ତୁ 1; /* acquired */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_bounding_set_drop(uint64_t keep, bool ଡାହାଣ_now) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା k, r;

        /* If we are run as PID 1 we will lack CAP_SETPCAP by default in the effective set (yes, the kernel
         * drops that when executing init!), so get it back temporarily so that we can call PR_CAPBSET_DROP. */

        CapabilityQuintet q;
        r = capability_get(&q);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        CapabilityQuintet saved = q;

        r = capability_gain_cap_setpcap();
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପାଇଁ (unsigned i = 0; i <= cap_last_cap(); i++) {
                ଯଦି (BIT_SET(keep, i))
                        ଜାରି;

                /* Drop it from the bounding set */
                r = prctl_safe(PR_CAPBSET_DROP, i, 0, 0, 0);
                ଯଦି (r < 0) {
                        /* If dropping the capability failed, let's see if we didn't have it in the first
                         * place. If so, continue anyway, as dropping a capability we didn't have in the
                         * first place doesn't really matter anyway. */
                        ଯଦି (prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0) != 0)
                                ଯାଅ finish;
                }

                /* Also drop it from the inheritable set, so that anything we exec() loses the capability for
                 * good. */
                CLEAR_BIT(q.inheritable, i);

                /* If we shall apply this right now drop it also from our own capability sets. */
                ଯଦି (ଡାହାଣ_now) {
                        CLEAR_BIT(q.effective, i);
                        CLEAR_BIT(q.permitted, i);
                }
        }

        r = 0;

finish:
        k = capability_apply(&q);
        ଯଦି (k < 0)
                /* If there are no actual changes anyway then let's ignore this error. */
                ଯଦି (!capability_quintet_equal(&q, &saved))
                        ଫେରନ୍ତୁ k;

        ଫେରନ୍ତୁ r;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା drop_from_file(ଅଚଳ ଅକ୍ଷର *fn, uint64_t keep) {
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        uint64_t current, after;
        uint32_t hi, lo;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, k;

        r = read_one_ଧାଡ଼ି_file(fn, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        k = sscanf(p, "%" PRIu32 " %" PRIu32, &lo, &hi);
        ଯଦି (k != 2)
                ଫେରନ୍ତୁ -EIO;

        current = (uint64_t) lo | ((uint64_t) hi << 32);
        after = current & keep;

        ଯଦି (current == after)
                ଫେରନ୍ତୁ 0;

        lo = after & UINT32_MAX;
        hi = (after >> 32) & UINT32_MAX;

        ଫେରନ୍ତୁ write_string_filef(fn, 0, "%" PRIu32 " %" PRIu32, lo, hi);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_bounding_set_drop_usermode(uint64_t keep) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/inheritable", keep);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/bset", keep);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Unfortunately we cannot leave privilege dropping to PID 1 here, since we want to run as user but
         * want to keep some capabilities. Since file capabilities have been introduced this cannot be done
         * across exec() anymore, unless our binary has the capability configured in the file system, which
         * we want to avoid. */

        ଯଦି (setresgid(gid, gid, gid) < 0)
                ଫେରନ୍ତୁ log_error_errno(errno, "Failed to change group ID: %m");

        r = maybe_setgroups(/* size= */ 0, /* list= */ ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_error_errno(r, "Failed to drop auxiliary groups list: %m");

        /* Ensure we keep the permitted caps across the setresuid(). Note that we do this even if we actually
         * don't want to keep any capabilities, since we want to be able to drop them from the bounding set
         * too, and we can only do that if we have capabilities. */
        r = prctl_safe(PR_SET_KEEPCAPS, 1, 0, 0, 0);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_error_errno(r, "Failed to enable keep capabilities flag: %m");

        ଯଦି (setresuid(uid, uid, uid) < 0)
                ଫେରନ୍ତୁ log_error_errno(errno, "Failed to change user ID: %m");

        r = prctl_safe(PR_SET_KEEPCAPS, 0, 0, 0, 0);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_error_errno(r, "Failed to disable keep capabilities flag: %m");

        /* Drop all caps from the bounding set (as well as the inheritable/permitted/effective sets), except
         * the ones we want to keep */
        r = capability_bounding_set_drop(keep_capabilities, /* right_now= */ true);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_error_errno(r, "Failed to drop capabilities: %m");

        /* Now upgrade the permitted caps we still kept to effective caps */
        ଯଦି (keep_capabilities != 0) {
                CapabilityQuintet q = {
                        .effective = keep_capabilities,
                        .permitted = keep_capabilities,
                };

                r = capability_apply(&q);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_error_errno(r, "Failed to increase capabilities: %m");
        }

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା change_capability(unsigned cap, bool b) {
        CapabilityQuintet q;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(cap <= CAP_LIMIT);

        r = capability_get(&q);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (b) {
                SET_BIT(q.effective, cap);
                SET_BIT(q.permitted, cap);
                SET_BIT(q.inheritable, cap);
        } ଅନ୍ୟଥା {
                CLEAR_BIT(q.effective, cap);
                CLEAR_BIT(q.permitted, cap);
                CLEAR_BIT(q.inheritable, cap);
        }

        ଫେରନ୍ତୁ capability_apply(&q);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା drop_capability(unsigned cap) {
        ଫେରନ୍ତୁ change_capability(cap, false);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା keep_capability(unsigned cap) {
        ଫେରନ୍ତୁ change_capability(cap, true);
}

bool capability_quintet_mangle(CapabilityQuintet *q) {
        uint64_t combined, drop = 0;

        ପ୍ରତିଜ୍ଞା(q);

        combined = q->effective | q->bounding | q->inheritable | q->permitted | q->ambient;

        ପାଇଁ (unsigned i = 0; i <= cap_last_cap(); i++) {
                ଯଦି (!BIT_SET(combined, i))
                        ଜାରି;

                ଯଦି (prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0) > 0)
                        ଜାରି;

                SET_BIT(drop, i);

                log_debug("Dropping capability not in the current bounding set: %s", capability_to_name(i));
        }

        q->effective &= ~drop;
        q->bounding &= ~drop;
        q->inheritable &= ~drop;
        q->permitted &= ~drop;
        q->ambient &= ~drop;

        ଫେରନ୍ତୁ drop != 0; /* Let the caller know we changed something */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_quintet_enforce(ଅଚଳ CapabilityQuintet *q) {
        CapabilityQuintet c;
        bool modified = false;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(q);

        ଯଦି (q->ambient != CAP_MASK_UNSET ||
            q->inheritable != CAP_MASK_UNSET ||
            q->permitted != CAP_MASK_UNSET ||
            q->effective != CAP_MASK_UNSET) {
                r = capability_get(&c);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (q->ambient != CAP_MASK_UNSET) {
                /* In order to raise the ambient caps set we first need to raise the matching
                 * inheritable + permitted cap */
                ଯଦି (!FLAGS_SET(c.permitted, q->ambient) ||
                    !FLAGS_SET(c.inheritable, q->ambient)) {

                        c.permitted |= q->ambient;
                        c.inheritable |= q->ambient;

                        r = capability_apply(&c);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                r = capability_ambient_set_apply(q->ambient, /* also_inherit= */ false);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (q->inheritable != CAP_MASK_UNSET || q->permitted != CAP_MASK_UNSET || q->effective != CAP_MASK_UNSET) {
                ଯଦି (!FLAGS_SET(c.effective, q->effective) ||
                    !FLAGS_SET(c.permitted, q->permitted) ||
                    !FLAGS_SET(c.inheritable, q->inheritable)) {

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
                        ଯଦି (q->bounding != CAP_MASK_UNSET &&
                            (!BIT_SET(c.effective, CAP_SETPCAP) || !BIT_SET(c.permitted, CAP_SETPCAP))) {
                                CapabilityQuintet tmp = c;

                                SET_BIT(c.effective, CAP_SETPCAP);
                                SET_BIT(c.permitted, CAP_SETPCAP);

                                modified = true;

                                r = capability_apply(&tmp);
                        } ଅନ୍ୟଥା
                                r = capability_apply(&c);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }
        }

        ଯଦି (q->bounding != CAP_MASK_UNSET) {
                r = capability_bounding_set_drop(q->bounding, /* right_now= */ false);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        /* If needed, let's now set the caps again, this time in the final version, which differs from what
         * we have already set only in the CAP_SETPCAP bit, which we needed for dropping the bounding
         * bits. This call only undoes bits and doesn't acquire any which means the bounding caps don't
         * matter. */
        ଯଦି (modified) {
                r = capability_apply(&c);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_get_ambient(uint64_t *ret) {
        uint64_t a = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        ପାଇଁ (unsigned i = 0; i <= cap_last_cap(); i++) {
                r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r > 0)
                        SET_BIT(a, i);
        }

        *ret = a;
        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_capability(ଅଚଳ PidRef *pidref, CapabilityQuintet *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!pidref_is_set(pidref))
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        ଅଚଳ ଅକ୍ଷର *path = procfs_file_alloca(pidref->pid, "status");
        _cleanup_fclose_ FILE *f = fopen(path, "re");
        ଯଦି (!f) {
                ଯଦି (errno == ENOENT && proc_mounted() == 0)
                        ଫେରନ୍ତୁ -ENOSYS;

                ଫେରନ୍ତୁ -errno;
        }

        CapabilityQuintet q = CAPABILITY_QUINTET_NULL;
        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;

                r = read_ଧାଡ଼ି(f, LONG_ଧାଡ଼ି_MAX, &ଧାଡ଼ି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                ସ୍ଥିର ଅଚଳ ସଂରଚନା {
                        ଅଚଳ ଅକ୍ଷର *field;
                        size_t offset;
                } fields[] = {
                        { "CapBnd:", offsetof(CapabilityQuintet, bounding)    },
                        { "CapInh:", offsetof(CapabilityQuintet, inheritable) },
                        { "CapPrm:", offsetof(CapabilityQuintet, permitted)   },
                        { "CapEff:", offsetof(CapabilityQuintet, effective)   },
                        { "CapAmb:", offsetof(CapabilityQuintet, ambient)     },
                };

                FOREACH_ELEMENT(i, fields) {

                        ଅଚଳ ଅକ୍ଷର *p = first_word(ଧାଡ଼ି, i->field);
                        ଯଦି (!p)
                                ଜାରି;

                        uint64_t *v = (uint64_t*) ((uint8_t*) &q + i->offset);

                        ଯଦି (*v != CAP_MASK_UNSET)
                                ଫେରନ୍ତୁ -EBADMSG;

                        r = safe_atoux64(p, v);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        ଯଦି (*v == CAP_MASK_UNSET)
                                ଫେରନ୍ତୁ -EBADMSG;
                }
        }

        ଯଦି (!capability_quintet_is_fully_set(&q))
                ଫେରନ୍ତୁ -EBADMSG;

        r = pidref_verify(pidref);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret)
                *ret = q;

        ଫେରନ୍ତୁ 0;
}

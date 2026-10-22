/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdatomic.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <sys/prctl.h> /* IWYU pragma: keep */
#அந்தர்பாவ <sys/syscall.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "bitfield.h"
#அந்தர்பாவ "capability-list.h"
#அந்தர்பாவ "capability-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "pidref.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "user-util.h"

முழு capability_get(CapabilityQuintet *ret) {
        ப்ரதிஜ்ஞா(ret);

        கட்டமைப்பு __user_cap_header_struct hdr = {
                .version = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        assert_cc(_LINUX_CAPABILITY_U32S_3 == 2);
        கட்டமைப்பு __user_cap_data_struct data[_LINUX_CAPABILITY_U32S_3];
        எனில் (syscall(SYS_capget, &hdr, data) < 0)
                திரும்பு -errno;

        *ret = (CapabilityQuintet) {
                .effective = (uint64_t) data[0].effective | ((uint64_t) data[1].effective << 32),
                .bounding = UINT64_MAX,
                .inheritable = (uint64_t) data[0].inheritable | ((uint64_t) data[1].inheritable << 32),
                .permitted = (uint64_t) data[0].permitted | ((uint64_t) data[1].permitted << 32),
                .ambient = UINT64_MAX,
        };
        திரும்பு 0;
}

நிலையான முழு capability_apply(மாறிலி CapabilityQuintet *q) {
        ப்ரதிஜ்ஞா(q);

        கட்டமைப்பு __user_cap_header_struct hdr = {
                .version = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        கட்டமைப்பு __user_cap_data_struct data[_LINUX_CAPABILITY_U32S_3] = {
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
        திரும்பு RET_NERRNO(syscall(SYS_capset, &hdr, data));
}

unsigned cap_last_cap(வெறுமை) {
        நிலையான atomic_int saved = INT_MAX;
        முழு r, c;

        c = saved;
        எனில் (c != INT_MAX)
                திரும்பு c;

        /* Available since linux-3.2 */
        _cleanup_free_ எழுத்து *content = வெற்று;
        r = read_one_வரி_file("/proc/sys/kernel/cap_last_cap", &content);
        எனில் (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/cap_last_cap, ignoring: %m");
        இல்லையெனில் {
                r = safe_atoi(content, &c);
                எனில் (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/kernel/cap_last_cap, ignoring: %m");
                இல்லையெனில் {
                        எனில் (c > CAP_LIMIT) /* Safety for the future: if one day the kernel learns more than
                                            * 64 caps, then we are in trouble (since we, as much userspace
                                            * and kernel space store capability masks in uint64_t types). We
                                            * also want to use UINT64_MAX as marker for "unset". Hence let's
                                            * hence protect ourselves against that and always cap at 62 for
                                            * now. */
                                c = CAP_LIMIT;

                        saved = c;
                        திரும்பு c;
                }
        }

        /* Fall back to syscall-probing for pre linux-3.2, or where /proc/ is not mounted */
        unsigned long p = (unsigned long) MIN(CAP_LAST_CAP, CAP_LIMIT);

        எனில் (prctl_safe(PR_CAPBSET_READ, p, 0, 0, 0) < 0) {

                /* Hmm, look downwards, until we find one that works */
                க்கு (p--; p > 0; p--)
                        எனில் (prctl_safe(PR_CAPBSET_READ, p, 0, 0, 0) >= 0)
                                நிறுத்து;

        } இல்லையெனில் {

                /* Hmm, look upwards, until we find one that doesn't work */
                க்கு (; p < CAP_LIMIT; p++)
                        எனில் (prctl_safe(PR_CAPBSET_READ, p+1, 0, 0, 0) < 0)
                                நிறுத்து;
        }

        c = (முழு) p;
        saved = c;
        திரும்பு c;
}

முழு have_effective_cap(unsigned cap) {
        CapabilityQuintet q;
        முழு r;

        ப்ரதிஜ்ஞா(cap <= CAP_LIMIT);

        r = capability_get(&q);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு BIT_SET(q.effective, cap);
}

முழு have_inheritable_cap(unsigned cap) {
        CapabilityQuintet q;
        முழு r;

        ப்ரதிஜ்ஞா(cap <= CAP_LIMIT);

        r = capability_get(&q);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு BIT_SET(q.inheritable, cap);
}

முழு capability_ambient_set_apply(uint64_t set, bool also_inherit) {
        முழு r;

        /* Remove capabilities requested in ambient set, but not in the bounding set */
        க்கு (unsigned i = 0; i <= cap_last_cap(); i++) {
                எனில் (!BIT_SET(set, i))
                        தொடர்;

                r = prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r != 1) {
                        log_debug("Ambient capability %s requested but missing from bounding set, suppressing automatically.",
                                  capability_to_name(i));
                        CLEAR_BIT(set, i);
                }
        }

        /* Add the capabilities to the ambient set (an possibly also the inheritable set) */

        எனில் (also_inherit) {
                CapabilityQuintet q;

                r = capability_get(&q);
                எனில் (r < 0)
                        திரும்பு r;

                q.inheritable = set;

                r = capability_apply(&q);
                எனில் (r < 0)
                        திரும்பு r;
        }

        க்கு (unsigned i = 0; i <= cap_last_cap(); i++)
                எனில் (BIT_SET(set, i)) {
                        /* Add the capability to the ambient set. */
                        r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, i, 0, 0);
                        எனில் (r < 0)
                                திரும்பு r;
                } இல்லையெனில் {
                        /* Drop the capability so we don't inherit capabilities we didn't ask for. */
                        r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r > 0) {
                                r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, i, 0, 0);
                                எனில் (r < 0)
                                        திரும்பு r;
                        }
                }

        திரும்பு 0;
}

முழு capability_gain_cap_setpcap(வெறுமை) {
        CapabilityQuintet q;
        முழு r;

        r = capability_get(&q);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (BIT_SET(q.effective, CAP_SETPCAP))
                திரும்பு 1; /* We already have capability. */

        SET_BIT(q.effective, CAP_SETPCAP);

        r = capability_apply(&q);
        எனில் (r < 0) {
                /* If we didn't manage to acquire the CAP_SETPCAP bit, we continue anyway, after all this
                 * just means we'll fail later, when we actually intend to drop some capabilities or try to
                 * set securebits. */
                log_debug_errno(r, "Can't acquire effective CAP_SETPCAP bit, ignoring: %m");
                திரும்பு 0;
        }

        திரும்பு 1; /* acquired */
}

முழு capability_bounding_set_drop(uint64_t keep, bool வலது_now) {
        முழு k, r;

        /* If we are run as PID 1 we will lack CAP_SETPCAP by default in the effective set (yes, the kernel
         * drops that when executing init!), so get it back temporarily so that we can call PR_CAPBSET_DROP. */

        CapabilityQuintet q;
        r = capability_get(&q);
        எனில் (r < 0)
                திரும்பு r;
        CapabilityQuintet saved = q;

        r = capability_gain_cap_setpcap();
        எனில் (r < 0)
                திரும்பு r;

        க்கு (unsigned i = 0; i <= cap_last_cap(); i++) {
                எனில் (BIT_SET(keep, i))
                        தொடர்;

                /* Drop it from the bounding set */
                r = prctl_safe(PR_CAPBSET_DROP, i, 0, 0, 0);
                எனில் (r < 0) {
                        /* If dropping the capability failed, let's see if we didn't have it in the first
                         * place. If so, continue anyway, as dropping a capability we didn't have in the
                         * first place doesn't really matter anyway. */
                        எனில் (prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0) != 0)
                                செல் finish;
                }

                /* Also drop it from the inheritable set, so that anything we exec() loses the capability for
                 * good. */
                CLEAR_BIT(q.inheritable, i);

                /* If we shall apply this right now drop it also from our own capability sets. */
                எனில் (வலது_now) {
                        CLEAR_BIT(q.effective, i);
                        CLEAR_BIT(q.permitted, i);
                }
        }

        r = 0;

finish:
        k = capability_apply(&q);
        எனில் (k < 0)
                /* If there are no actual changes anyway then let's ignore this error. */
                எனில் (!capability_quintet_equal(&q, &saved))
                        திரும்பு k;

        திரும்பு r;
}

நிலையான முழு drop_from_file(மாறிலி எழுத்து *fn, uint64_t keep) {
        _cleanup_free_ எழுத்து *p = வெற்று;
        uint64_t current, after;
        uint32_t hi, lo;
        முழு r, k;

        r = read_one_வரி_file(fn, &p);
        எனில் (r < 0)
                திரும்பு r;

        k = sscanf(p, "%" PRIu32 " %" PRIu32, &lo, &hi);
        எனில் (k != 2)
                திரும்பு -EIO;

        current = (uint64_t) lo | ((uint64_t) hi << 32);
        after = current & keep;

        எனில் (current == after)
                திரும்பு 0;

        lo = after & UINT32_MAX;
        hi = (after >> 32) & UINT32_MAX;

        திரும்பு write_string_filef(fn, 0, "%" PRIu32 " %" PRIu32, lo, hi);
}

முழு capability_bounding_set_drop_usermode(uint64_t keep) {
        முழு r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/inheritable", keep);
        எனில் (r < 0)
                திரும்பு r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/bset", keep);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு r;
}

முழு drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities) {
        முழு r;

        /* Unfortunately we cannot leave privilege dropping to PID 1 here, since we want to run as user but
         * want to keep some capabilities. Since file capabilities have been introduced this cannot be done
         * across exec() anymore, unless our binary has the capability configured in the file system, which
         * we want to avoid. */

        எனில் (setresgid(gid, gid, gid) < 0)
                திரும்பு log_error_errno(errno, "Failed to change group ID: %m");

        r = maybe_setgroups(/* size= */ 0, /* list= */ வெற்று);
        எனில் (r < 0)
                திரும்பு log_error_errno(r, "Failed to drop auxiliary groups list: %m");

        /* Ensure we keep the permitted caps across the setresuid(). Note that we do this even if we actually
         * don't want to keep any capabilities, since we want to be able to drop them from the bounding set
         * too, and we can only do that if we have capabilities. */
        r = prctl_safe(PR_SET_KEEPCAPS, 1, 0, 0, 0);
        எனில் (r < 0)
                திரும்பு log_error_errno(r, "Failed to enable keep capabilities flag: %m");

        எனில் (setresuid(uid, uid, uid) < 0)
                திரும்பு log_error_errno(errno, "Failed to change user ID: %m");

        r = prctl_safe(PR_SET_KEEPCAPS, 0, 0, 0, 0);
        எனில் (r < 0)
                திரும்பு log_error_errno(r, "Failed to disable keep capabilities flag: %m");

        /* Drop all caps from the bounding set (as well as the inheritable/permitted/effective sets), except
         * the ones we want to keep */
        r = capability_bounding_set_drop(keep_capabilities, /* right_now= */ true);
        எனில் (r < 0)
                திரும்பு log_error_errno(r, "Failed to drop capabilities: %m");

        /* Now upgrade the permitted caps we still kept to effective caps */
        எனில் (keep_capabilities != 0) {
                CapabilityQuintet q = {
                        .effective = keep_capabilities,
                        .permitted = keep_capabilities,
                };

                r = capability_apply(&q);
                எனில் (r < 0)
                        திரும்பு log_error_errno(r, "Failed to increase capabilities: %m");
        }

        திரும்பு 0;
}

நிலையான முழு change_capability(unsigned cap, bool b) {
        CapabilityQuintet q;
        முழு r;

        ப்ரதிஜ்ஞா(cap <= CAP_LIMIT);

        r = capability_get(&q);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (b) {
                SET_BIT(q.effective, cap);
                SET_BIT(q.permitted, cap);
                SET_BIT(q.inheritable, cap);
        } இல்லையெனில் {
                CLEAR_BIT(q.effective, cap);
                CLEAR_BIT(q.permitted, cap);
                CLEAR_BIT(q.inheritable, cap);
        }

        திரும்பு capability_apply(&q);
}

முழு drop_capability(unsigned cap) {
        திரும்பு change_capability(cap, false);
}

முழு keep_capability(unsigned cap) {
        திரும்பு change_capability(cap, true);
}

bool capability_quintet_mangle(CapabilityQuintet *q) {
        uint64_t combined, drop = 0;

        ப்ரதிஜ்ஞா(q);

        combined = q->effective | q->bounding | q->inheritable | q->permitted | q->ambient;

        க்கு (unsigned i = 0; i <= cap_last_cap(); i++) {
                எனில் (!BIT_SET(combined, i))
                        தொடர்;

                எனில் (prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0) > 0)
                        தொடர்;

                SET_BIT(drop, i);

                log_debug("Dropping capability not in the current bounding set: %s", capability_to_name(i));
        }

        q->effective &= ~drop;
        q->bounding &= ~drop;
        q->inheritable &= ~drop;
        q->permitted &= ~drop;
        q->ambient &= ~drop;

        திரும்பு drop != 0; /* Let the caller know we changed something */
}

முழு capability_quintet_enforce(மாறிலி CapabilityQuintet *q) {
        CapabilityQuintet c;
        bool modified = false;
        முழு r;

        ப்ரதிஜ்ஞா(q);

        எனில் (q->ambient != CAP_MASK_UNSET ||
            q->inheritable != CAP_MASK_UNSET ||
            q->permitted != CAP_MASK_UNSET ||
            q->effective != CAP_MASK_UNSET) {
                r = capability_get(&c);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (q->ambient != CAP_MASK_UNSET) {
                /* In order to raise the ambient caps set we first need to raise the matching
                 * inheritable + permitted cap */
                எனில் (!FLAGS_SET(c.permitted, q->ambient) ||
                    !FLAGS_SET(c.inheritable, q->ambient)) {

                        c.permitted |= q->ambient;
                        c.inheritable |= q->ambient;

                        r = capability_apply(&c);
                        எனில் (r < 0)
                                திரும்பு r;
                }

                r = capability_ambient_set_apply(q->ambient, /* also_inherit= */ false);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (q->inheritable != CAP_MASK_UNSET || q->permitted != CAP_MASK_UNSET || q->effective != CAP_MASK_UNSET) {
                எனில் (!FLAGS_SET(c.effective, q->effective) ||
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
                        எனில் (q->bounding != CAP_MASK_UNSET &&
                            (!BIT_SET(c.effective, CAP_SETPCAP) || !BIT_SET(c.permitted, CAP_SETPCAP))) {
                                CapabilityQuintet tmp = c;

                                SET_BIT(c.effective, CAP_SETPCAP);
                                SET_BIT(c.permitted, CAP_SETPCAP);

                                modified = true;

                                r = capability_apply(&tmp);
                        } இல்லையெனில்
                                r = capability_apply(&c);
                        எனில் (r < 0)
                                திரும்பு r;
                }
        }

        எனில் (q->bounding != CAP_MASK_UNSET) {
                r = capability_bounding_set_drop(q->bounding, /* right_now= */ false);
                எனில் (r < 0)
                        திரும்பு r;
        }

        /* If needed, let's now set the caps again, this time in the final version, which differs from what
         * we have already set only in the CAP_SETPCAP bit, which we needed for dropping the bounding
         * bits. This call only undoes bits and doesn't acquire any which means the bounding caps don't
         * matter. */
        எனில் (modified) {
                r = capability_apply(&c);
                எனில் (r < 0)
                        திரும்பு r;
        }

        திரும்பு 0;
}

முழு capability_get_ambient(uint64_t *ret) {
        uint64_t a = 0;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        க்கு (unsigned i = 0; i <= cap_last_cap(); i++) {
                r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r > 0)
                        SET_BIT(a, i);
        }

        *ret = a;
        திரும்பு 1;
}

முழு pidref_get_capability(மாறிலி PidRef *pidref, CapabilityQuintet *ret) {
        முழு r;

        எனில் (!pidref_is_set(pidref))
                திரும்பு -ESRCH;
        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        மாறிலி எழுத்து *path = procfs_file_alloca(pidref->pid, "status");
        _cleanup_fclose_ FILE *f = fopen(path, "re");
        எனில் (!f) {
                எனில் (errno == ENOENT && proc_mounted() == 0)
                        திரும்பு -ENOSYS;

                திரும்பு -errno;
        }

        CapabilityQuintet q = CAPABILITY_QUINTET_NULL;
        க்கு (;;) {
                _cleanup_free_ எழுத்து *வரி = வெற்று;

                r = read_வரி(f, LONG_வரி_MAX, &வரி);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                நிலையான மாறிலி கட்டமைப்பு {
                        மாறிலி எழுத்து *field;
                        size_t offset;
                } fields[] = {
                        { "CapBnd:", offsetof(CapabilityQuintet, bounding)    },
                        { "CapInh:", offsetof(CapabilityQuintet, inheritable) },
                        { "CapPrm:", offsetof(CapabilityQuintet, permitted)   },
                        { "CapEff:", offsetof(CapabilityQuintet, effective)   },
                        { "CapAmb:", offsetof(CapabilityQuintet, ambient)     },
                };

                FOREACH_ELEMENT(i, fields) {

                        மாறிலி எழுத்து *p = first_word(வரி, i->field);
                        எனில் (!p)
                                தொடர்;

                        uint64_t *v = (uint64_t*) ((uint8_t*) &q + i->offset);

                        எனில் (*v != CAP_MASK_UNSET)
                                திரும்பு -EBADMSG;

                        r = safe_atoux64(p, v);
                        எனில் (r < 0)
                                திரும்பு r;

                        எனில் (*v == CAP_MASK_UNSET)
                                திரும்பு -EBADMSG;
                }
        }

        எனில் (!capability_quintet_is_fully_set(&q))
                திரும்பு -EBADMSG;

        r = pidref_verify(pidref);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret)
                *ret = q;

        திரும்பு 0;
}

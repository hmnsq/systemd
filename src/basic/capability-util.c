/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdatomic.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <sys/prctl.h> /* IWYU pragma: keep */
#अन्तर्भाव <sys/syscall.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "bitfield.h"
#अन्तर्भाव "capability-list.h"
#अन्तर्भाव "capability-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "pidref.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "user-util.h"

पूर्णाङ्क capability_get(CapabilityQuintet *ret) {
        प्रतिज्ञा(ret);

        संरचना __user_cap_header_struct hdr = {
                .version = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        assert_cc(_LINUX_CAPABILITY_U32S_3 == 2);
        संरचना __user_cap_data_struct data[_LINUX_CAPABILITY_U32S_3];
        यदि (syscall(SYS_capget, &hdr, data) < 0)
                प्रत्यागम -errno;

        *ret = (CapabilityQuintet) {
                .effective = (uint64_t) data[0].effective | ((uint64_t) data[1].effective << 32),
                .bounding = UINT64_MAX,
                .inheritable = (uint64_t) data[0].inheritable | ((uint64_t) data[1].inheritable << 32),
                .permitted = (uint64_t) data[0].permitted | ((uint64_t) data[1].permitted << 32),
                .ambient = UINT64_MAX,
        };
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क capability_apply(ध्रुव CapabilityQuintet *q) {
        प्रतिज्ञा(q);

        संरचना __user_cap_header_struct hdr = {
                .version = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        संरचना __user_cap_data_struct data[_LINUX_CAPABILITY_U32S_3] = {
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
        प्रत्यागम RET_NERRNO(syscall(SYS_capset, &hdr, data));
}

unsigned cap_last_cap(शून्य) {
        स्थिर atomic_int saved = INT_MAX;
        पूर्णाङ्क r, c;

        c = saved;
        यदि (c != INT_MAX)
                प्रत्यागम c;

        /* Available since linux-3.2 */
        _cleanup_free_ वर्ण *content = रिक्त;
        r = read_one_ओळ_file("/proc/sys/kernel/cap_last_cap", &content);
        यदि (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/cap_last_cap, ignoring: %m");
        अन्यथा {
                r = safe_atoi(content, &c);
                यदि (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/kernel/cap_last_cap, ignoring: %m");
                अन्यथा {
                        यदि (c > CAP_LIMIT) /* Safety for the future: if one day the kernel learns more than
                                            * 64 caps, then we are in trouble (since we, as much userspace
                                            * and kernel space store capability masks in uint64_t types). We
                                            * also want to use UINT64_MAX as marker for "unset". Hence let's
                                            * hence protect ourselves against that and always cap at 62 for
                                            * now. */
                                c = CAP_LIMIT;

                        saved = c;
                        प्रत्यागम c;
                }
        }

        /* Fall back to syscall-probing for pre linux-3.2, or where /proc/ is not mounted */
        unsigned long p = (unsigned long) MIN(CAP_LAST_CAP, CAP_LIMIT);

        यदि (prctl_safe(PR_CAPBSET_READ, p, 0, 0, 0) < 0) {

                /* Hmm, look downwards, until we find one that works */
                प्रति (p--; p > 0; p--)
                        यदि (prctl_safe(PR_CAPBSET_READ, p, 0, 0, 0) >= 0)
                                भङ्ग;

        } अन्यथा {

                /* Hmm, look upwards, until we find one that doesn't work */
                प्रति (; p < CAP_LIMIT; p++)
                        यदि (prctl_safe(PR_CAPBSET_READ, p+1, 0, 0, 0) < 0)
                                भङ्ग;
        }

        c = (पूर्णाङ्क) p;
        saved = c;
        प्रत्यागम c;
}

पूर्णाङ्क have_effective_cap(unsigned cap) {
        CapabilityQuintet q;
        पूर्णाङ्क r;

        प्रतिज्ञा(cap <= CAP_LIMIT);

        r = capability_get(&q);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम BIT_SET(q.effective, cap);
}

पूर्णाङ्क have_inheritable_cap(unsigned cap) {
        CapabilityQuintet q;
        पूर्णाङ्क r;

        प्रतिज्ञा(cap <= CAP_LIMIT);

        r = capability_get(&q);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम BIT_SET(q.inheritable, cap);
}

पूर्णाङ्क capability_ambient_set_apply(uint64_t set, bool also_inherit) {
        पूर्णाङ्क r;

        /* Remove capabilities requested in ambient set, but not in the bounding set */
        प्रति (unsigned i = 0; i <= cap_last_cap(); i++) {
                यदि (!BIT_SET(set, i))
                        अनुवर्त;

                r = prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r != 1) {
                        log_debug("Ambient capability %s requested but missing from bounding set, suppressing automatically.",
                                  capability_to_name(i));
                        CLEAR_BIT(set, i);
                }
        }

        /* Add the capabilities to the ambient set (an possibly also the inheritable set) */

        यदि (also_inherit) {
                CapabilityQuintet q;

                r = capability_get(&q);
                यदि (r < 0)
                        प्रत्यागम r;

                q.inheritable = set;

                r = capability_apply(&q);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        प्रति (unsigned i = 0; i <= cap_last_cap(); i++)
                यदि (BIT_SET(set, i)) {
                        /* Add the capability to the ambient set. */
                        r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, i, 0, 0);
                        यदि (r < 0)
                                प्रत्यागम r;
                } अन्यथा {
                        /* Drop the capability so we don't inherit capabilities we didn't ask for. */
                        r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                        यदि (r < 0)
                                प्रत्यागम r;
                        यदि (r > 0) {
                                r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, i, 0, 0);
                                यदि (r < 0)
                                        प्रत्यागम r;
                        }
                }

        प्रत्यागम 0;
}

पूर्णाङ्क capability_gain_cap_setpcap(शून्य) {
        CapabilityQuintet q;
        पूर्णाङ्क r;

        r = capability_get(&q);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (BIT_SET(q.effective, CAP_SETPCAP))
                प्रत्यागम 1; /* We already have capability. */

        SET_BIT(q.effective, CAP_SETPCAP);

        r = capability_apply(&q);
        यदि (r < 0) {
                /* If we didn't manage to acquire the CAP_SETPCAP bit, we continue anyway, after all this
                 * just means we'll fail later, when we actually intend to drop some capabilities or try to
                 * set securebits. */
                log_debug_errno(r, "Can't acquire effective CAP_SETPCAP bit, ignoring: %m");
                प्रत्यागम 0;
        }

        प्रत्यागम 1; /* acquired */
}

पूर्णाङ्क capability_bounding_set_drop(uint64_t keep, bool उजवा_now) {
        पूर्णाङ्क k, r;

        /* If we are run as PID 1 we will lack CAP_SETPCAP by default in the effective set (yes, the kernel
         * drops that when executing init!), so get it back temporarily so that we can call PR_CAPBSET_DROP. */

        CapabilityQuintet q;
        r = capability_get(&q);
        यदि (r < 0)
                प्रत्यागम r;
        CapabilityQuintet saved = q;

        r = capability_gain_cap_setpcap();
        यदि (r < 0)
                प्रत्यागम r;

        प्रति (unsigned i = 0; i <= cap_last_cap(); i++) {
                यदि (BIT_SET(keep, i))
                        अनुवर्त;

                /* Drop it from the bounding set */
                r = prctl_safe(PR_CAPBSET_DROP, i, 0, 0, 0);
                यदि (r < 0) {
                        /* If dropping the capability failed, let's see if we didn't have it in the first
                         * place. If so, continue anyway, as dropping a capability we didn't have in the
                         * first place doesn't really matter anyway. */
                        यदि (prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0) != 0)
                                गच्छ finish;
                }

                /* Also drop it from the inheritable set, so that anything we exec() loses the capability for
                 * good. */
                CLEAR_BIT(q.inheritable, i);

                /* If we shall apply this right now drop it also from our own capability sets. */
                यदि (उजवा_now) {
                        CLEAR_BIT(q.effective, i);
                        CLEAR_BIT(q.permitted, i);
                }
        }

        r = 0;

finish:
        k = capability_apply(&q);
        यदि (k < 0)
                /* If there are no actual changes anyway then let's ignore this error. */
                यदि (!capability_quintet_equal(&q, &saved))
                        प्रत्यागम k;

        प्रत्यागम r;
}

स्थिर पूर्णाङ्क drop_from_file(ध्रुव वर्ण *fn, uint64_t keep) {
        _cleanup_free_ वर्ण *p = रिक्त;
        uint64_t current, after;
        uint32_t hi, lo;
        पूर्णाङ्क r, k;

        r = read_one_ओळ_file(fn, &p);
        यदि (r < 0)
                प्रत्यागम r;

        k = sscanf(p, "%" PRIu32 " %" PRIu32, &lo, &hi);
        यदि (k != 2)
                प्रत्यागम -EIO;

        current = (uint64_t) lo | ((uint64_t) hi << 32);
        after = current & keep;

        यदि (current == after)
                प्रत्यागम 0;

        lo = after & UINT32_MAX;
        hi = (after >> 32) & UINT32_MAX;

        प्रत्यागम write_string_filef(fn, 0, "%" PRIu32 " %" PRIu32, lo, hi);
}

पूर्णाङ्क capability_bounding_set_drop_usermode(uint64_t keep) {
        पूर्णाङ्क r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/inheritable", keep);
        यदि (r < 0)
                प्रत्यागम r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/bset", keep);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम r;
}

पूर्णाङ्क drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities) {
        पूर्णाङ्क r;

        /* Unfortunately we cannot leave privilege dropping to PID 1 here, since we want to run as user but
         * want to keep some capabilities. Since file capabilities have been introduced this cannot be done
         * across exec() anymore, unless our binary has the capability configured in the file system, which
         * we want to avoid. */

        यदि (setresgid(gid, gid, gid) < 0)
                प्रत्यागम log_error_errno(errno, "Failed to change group ID: %m");

        r = maybe_setgroups(/* size= */ 0, /* list= */ रिक्त);
        यदि (r < 0)
                प्रत्यागम log_error_errno(r, "Failed to drop auxiliary groups list: %m");

        /* Ensure we keep the permitted caps across the setresuid(). Note that we do this even if we actually
         * don't want to keep any capabilities, since we want to be able to drop them from the bounding set
         * too, and we can only do that if we have capabilities. */
        r = prctl_safe(PR_SET_KEEPCAPS, 1, 0, 0, 0);
        यदि (r < 0)
                प्रत्यागम log_error_errno(r, "Failed to enable keep capabilities flag: %m");

        यदि (setresuid(uid, uid, uid) < 0)
                प्रत्यागम log_error_errno(errno, "Failed to change user ID: %m");

        r = prctl_safe(PR_SET_KEEPCAPS, 0, 0, 0, 0);
        यदि (r < 0)
                प्रत्यागम log_error_errno(r, "Failed to disable keep capabilities flag: %m");

        /* Drop all caps from the bounding set (as well as the inheritable/permitted/effective sets), except
         * the ones we want to keep */
        r = capability_bounding_set_drop(keep_capabilities, /* right_now= */ true);
        यदि (r < 0)
                प्रत्यागम log_error_errno(r, "Failed to drop capabilities: %m");

        /* Now upgrade the permitted caps we still kept to effective caps */
        यदि (keep_capabilities != 0) {
                CapabilityQuintet q = {
                        .effective = keep_capabilities,
                        .permitted = keep_capabilities,
                };

                r = capability_apply(&q);
                यदि (r < 0)
                        प्रत्यागम log_error_errno(r, "Failed to increase capabilities: %m");
        }

        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क change_capability(unsigned cap, bool b) {
        CapabilityQuintet q;
        पूर्णाङ्क r;

        प्रतिज्ञा(cap <= CAP_LIMIT);

        r = capability_get(&q);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (b) {
                SET_BIT(q.effective, cap);
                SET_BIT(q.permitted, cap);
                SET_BIT(q.inheritable, cap);
        } अन्यथा {
                CLEAR_BIT(q.effective, cap);
                CLEAR_BIT(q.permitted, cap);
                CLEAR_BIT(q.inheritable, cap);
        }

        प्रत्यागम capability_apply(&q);
}

पूर्णाङ्क drop_capability(unsigned cap) {
        प्रत्यागम change_capability(cap, false);
}

पूर्णाङ्क keep_capability(unsigned cap) {
        प्रत्यागम change_capability(cap, true);
}

bool capability_quintet_mangle(CapabilityQuintet *q) {
        uint64_t combined, drop = 0;

        प्रतिज्ञा(q);

        combined = q->effective | q->bounding | q->inheritable | q->permitted | q->ambient;

        प्रति (unsigned i = 0; i <= cap_last_cap(); i++) {
                यदि (!BIT_SET(combined, i))
                        अनुवर्त;

                यदि (prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0) > 0)
                        अनुवर्त;

                SET_BIT(drop, i);

                log_debug("Dropping capability not in the current bounding set: %s", capability_to_name(i));
        }

        q->effective &= ~drop;
        q->bounding &= ~drop;
        q->inheritable &= ~drop;
        q->permitted &= ~drop;
        q->ambient &= ~drop;

        प्रत्यागम drop != 0; /* Let the caller know we changed something */
}

पूर्णाङ्क capability_quintet_enforce(ध्रुव CapabilityQuintet *q) {
        CapabilityQuintet c;
        bool modified = false;
        पूर्णाङ्क r;

        प्रतिज्ञा(q);

        यदि (q->ambient != CAP_MASK_UNSET ||
            q->inheritable != CAP_MASK_UNSET ||
            q->permitted != CAP_MASK_UNSET ||
            q->effective != CAP_MASK_UNSET) {
                r = capability_get(&c);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (q->ambient != CAP_MASK_UNSET) {
                /* In order to raise the ambient caps set we first need to raise the matching
                 * inheritable + permitted cap */
                यदि (!FLAGS_SET(c.permitted, q->ambient) ||
                    !FLAGS_SET(c.inheritable, q->ambient)) {

                        c.permitted |= q->ambient;
                        c.inheritable |= q->ambient;

                        r = capability_apply(&c);
                        यदि (r < 0)
                                प्रत्यागम r;
                }

                r = capability_ambient_set_apply(q->ambient, /* also_inherit= */ false);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (q->inheritable != CAP_MASK_UNSET || q->permitted != CAP_MASK_UNSET || q->effective != CAP_MASK_UNSET) {
                यदि (!FLAGS_SET(c.effective, q->effective) ||
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
                        यदि (q->bounding != CAP_MASK_UNSET &&
                            (!BIT_SET(c.effective, CAP_SETPCAP) || !BIT_SET(c.permitted, CAP_SETPCAP))) {
                                CapabilityQuintet tmp = c;

                                SET_BIT(c.effective, CAP_SETPCAP);
                                SET_BIT(c.permitted, CAP_SETPCAP);

                                modified = true;

                                r = capability_apply(&tmp);
                        } अन्यथा
                                r = capability_apply(&c);
                        यदि (r < 0)
                                प्रत्यागम r;
                }
        }

        यदि (q->bounding != CAP_MASK_UNSET) {
                r = capability_bounding_set_drop(q->bounding, /* right_now= */ false);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        /* If needed, let's now set the caps again, this time in the final version, which differs from what
         * we have already set only in the CAP_SETPCAP bit, which we needed for dropping the bounding
         * bits. This call only undoes bits and doesn't acquire any which means the bounding caps don't
         * matter. */
        यदि (modified) {
                r = capability_apply(&c);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क capability_get_ambient(uint64_t *ret) {
        uint64_t a = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        प्रति (unsigned i = 0; i <= cap_last_cap(); i++) {
                r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r > 0)
                        SET_BIT(a, i);
        }

        *ret = a;
        प्रत्यागम 1;
}

पूर्णाङ्क pidref_get_capability(ध्रुव PidRef *pidref, CapabilityQuintet *ret) {
        पूर्णाङ्क r;

        यदि (!pidref_is_set(pidref))
                प्रत्यागम -ESRCH;
        यदि (pidref_is_remote(pidref))
                प्रत्यागम -EREMOTE;

        ध्रुव वर्ण *path = procfs_file_alloca(pidref->pid, "status");
        _cleanup_fclose_ FILE *f = fopen(path, "re");
        यदि (!f) {
                यदि (errno == ENOENT && proc_mounted() == 0)
                        प्रत्यागम -ENOSYS;

                प्रत्यागम -errno;
        }

        CapabilityQuintet q = CAPABILITY_QUINTET_NULL;
        प्रति (;;) {
                _cleanup_free_ वर्ण *ओळ = रिक्त;

                r = read_ओळ(f, LONG_ओळ_MAX, &ओळ);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        भङ्ग;

                स्थिर ध्रुव संरचना {
                        ध्रुव वर्ण *field;
                        size_t offset;
                } fields[] = {
                        { "CapBnd:", offsetof(CapabilityQuintet, bounding)    },
                        { "CapInh:", offsetof(CapabilityQuintet, inheritable) },
                        { "CapPrm:", offsetof(CapabilityQuintet, permitted)   },
                        { "CapEff:", offsetof(CapabilityQuintet, effective)   },
                        { "CapAmb:", offsetof(CapabilityQuintet, ambient)     },
                };

                FOREACH_ELEMENT(i, fields) {

                        ध्रुव वर्ण *p = first_word(ओळ, i->field);
                        यदि (!p)
                                अनुवर्त;

                        uint64_t *v = (uint64_t*) ((uint8_t*) &q + i->offset);

                        यदि (*v != CAP_MASK_UNSET)
                                प्रत्यागम -EBADMSG;

                        r = safe_atoux64(p, v);
                        यदि (r < 0)
                                प्रत्यागम r;

                        यदि (*v == CAP_MASK_UNSET)
                                प्रत्यागम -EBADMSG;
                }
        }

        यदि (!capability_quintet_is_fully_set(&q))
                प्रत्यागम -EBADMSG;

        r = pidref_verify(pidref);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret)
                *ret = q;

        प्रत्यागम 0;
}

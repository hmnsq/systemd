/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdatomic.h>
#समावेश <stdio.h>
#समावेश <sys/prctl.h> /* IWYU pragma: keep */
#समावेश <sys/syscall.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "bitfield.h"
#समावेश "capability-list.h"
#समावेश "capability-util.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "log.h"
#समावेश "parse-util.h"
#समावेश "pidref.h"
#समावेश "process-util.h"
#समावेश "stat-util.h"
#समावेश "user-util.h"

पूर्णांक capability_get(CapabilityQuintet *ret) {
        निश्चित(ret);

        काष्ठा __user_cap_header_struct hdr = {
                .version = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        assert_cc(_LINUX_CAPABILITY_U32S_3 == 2);
        काष्ठा __user_cap_data_struct तथ्य[_LINUX_CAPABILITY_U32S_3];
        अगर (syscall(SYS_capget, &hdr, तथ्य) < 0)
                वापस -त्रुटि_सं;

        *ret = (CapabilityQuintet) {
                .effective = (uint64_t) तथ्य[0].effective | ((uint64_t) तथ्य[1].effective << 32),
                .bounding = UINT64_MAX,
                .inheritable = (uint64_t) तथ्य[0].inheritable | ((uint64_t) तथ्य[1].inheritable << 32),
                .permitted = (uint64_t) तथ्य[0].permitted | ((uint64_t) तथ्य[1].permitted << 32),
                .ambient = UINT64_MAX,
        };
        वापस 0;
}

अटल पूर्णांक capability_apply(स्थिर CapabilityQuintet *q) {
        निश्चित(q);

        काष्ठा __user_cap_header_struct hdr = {
                .version = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        काष्ठा __user_cap_data_struct तथ्य[_LINUX_CAPABILITY_U32S_3] = {
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
        वापस RET_NERRNO(syscall(SYS_capset, &hdr, तथ्य));
}

अचिन्हित cap_last_cap(व्योम) {
        अटल atomic_int saved = पूर्णांक_उच्च;
        पूर्णांक r, c;

        c = saved;
        अगर (c != पूर्णांक_उच्च)
                वापस c;

        /* Available since linux-3.2 */
        _cleanup_free_ अक्षर *content = शून्य;
        r = read_one_पंक्ति_file("/proc/sys/kernel/cap_last_cap", &content);
        अगर (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/cap_last_cap, ignoring: %m");
        अन्यथा {
                r = safe_atoi(content, &c);
                अगर (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/kernel/cap_last_cap, ignoring: %m");
                अन्यथा {
                        अगर (c > CAP_LIMIT) /* Safety for the future: if one day the kernel learns more than
                                            * 64 caps, then we are in trouble (since we, as much userspace
                                            * and kernel space store capability masks in uint64_t types). We
                                            * also want to use UINT64_MAX as marker for "unset". Hence let's
                                            * hence protect ourselves against that and always cap at 62 for
                                            * now. */
                                c = CAP_LIMIT;

                        saved = c;
                        वापस c;
                }
        }

        /* Fall back to syscall-probing for pre linux-3.2, or where /proc/ is not mounted */
        अचिन्हित दीर्घ p = (अचिन्हित दीर्घ) MIN(CAP_LAST_CAP, CAP_LIMIT);

        अगर (prctl_safe(PR_CAPBSET_READ, p, 0, 0, 0) < 0) {

                /* Hmm, look downwards, until we find one that works */
                क्रम (p--; p > 0; p--)
                        अगर (prctl_safe(PR_CAPBSET_READ, p, 0, 0, 0) >= 0)
                                अवरोध;

        } अन्यथा {

                /* Hmm, look upwards, until we find one that doesn't work */
                क्रम (; p < CAP_LIMIT; p++)
                        अगर (prctl_safe(PR_CAPBSET_READ, p+1, 0, 0, 0) < 0)
                                अवरोध;
        }

        c = (पूर्णांक) p;
        saved = c;
        वापस c;
}

पूर्णांक have_effective_cap(अचिन्हित cap) {
        CapabilityQuintet q;
        पूर्णांक r;

        निश्चित(cap <= CAP_LIMIT);

        r = capability_get(&q);
        अगर (r < 0)
                वापस r;

        वापस BIT_SET(q.effective, cap);
}

पूर्णांक have_inheritable_cap(अचिन्हित cap) {
        CapabilityQuintet q;
        पूर्णांक r;

        निश्चित(cap <= CAP_LIMIT);

        r = capability_get(&q);
        अगर (r < 0)
                वापस r;

        वापस BIT_SET(q.inheritable, cap);
}

पूर्णांक capability_ambient_set_apply(uint64_t set, bool also_inherit) {
        पूर्णांक r;

        /* Remove capabilities requested in ambient set, but not in the bounding set */
        क्रम (अचिन्हित i = 0; i <= cap_last_cap(); i++) {
                अगर (!BIT_SET(set, i))
                        जारी;

                r = prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0);
                अगर (r < 0)
                        वापस r;
                अगर (r != 1) {
                        log_debug("Ambient capability %s requested but missing from bounding set, suppressing automatically.",
                                  capability_to_name(i));
                        CLEAR_BIT(set, i);
                }
        }

        /* Add the capabilities to the ambient set (an possibly also the inheritable set) */

        अगर (also_inherit) {
                CapabilityQuintet q;

                r = capability_get(&q);
                अगर (r < 0)
                        वापस r;

                q.inheritable = set;

                r = capability_apply(&q);
                अगर (r < 0)
                        वापस r;
        }

        क्रम (अचिन्हित i = 0; i <= cap_last_cap(); i++)
                अगर (BIT_SET(set, i)) {
                        /* Add the capability to the ambient set. */
                        r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, i, 0, 0);
                        अगर (r < 0)
                                वापस r;
                } अन्यथा {
                        /* Drop the capability so we don't inherit capabilities we didn't ask for. */
                        r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                        अगर (r < 0)
                                वापस r;
                        अगर (r > 0) {
                                r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, i, 0, 0);
                                अगर (r < 0)
                                        वापस r;
                        }
                }

        वापस 0;
}

पूर्णांक capability_gain_cap_setpcap(व्योम) {
        CapabilityQuintet q;
        पूर्णांक r;

        r = capability_get(&q);
        अगर (r < 0)
                वापस r;

        अगर (BIT_SET(q.effective, CAP_SETPCAP))
                वापस 1; /* We already have capability. */

        SET_BIT(q.effective, CAP_SETPCAP);

        r = capability_apply(&q);
        अगर (r < 0) {
                /* If we didn't manage to acquire the CAP_SETPCAP bit, we continue anyway, after all this
                 * just means we'll fail later, when we actually intend to drop some capabilities or try to
                 * set securebits. */
                log_debug_errno(r, "Can't acquire effective CAP_SETPCAP bit, ignoring: %m");
                वापस 0;
        }

        वापस 1; /* acquired */
}

पूर्णांक capability_bounding_set_drop(uint64_t keep, bool दायाँ_now) {
        पूर्णांक k, r;

        /* If we are run as PID 1 we will lack CAP_SETPCAP by default in the effective set (yes, the kernel
         * drops that when executing init!), so get it back temporarily so that we can call PR_CAPBSET_DROP. */

        CapabilityQuintet q;
        r = capability_get(&q);
        अगर (r < 0)
                वापस r;
        CapabilityQuintet saved = q;

        r = capability_gain_cap_setpcap();
        अगर (r < 0)
                वापस r;

        क्रम (अचिन्हित i = 0; i <= cap_last_cap(); i++) {
                अगर (BIT_SET(keep, i))
                        जारी;

                /* Drop it from the bounding set */
                r = prctl_safe(PR_CAPBSET_DROP, i, 0, 0, 0);
                अगर (r < 0) {
                        /* If dropping the capability failed, let's see if we didn't have it in the first
                         * place. If so, continue anyway, as dropping a capability we didn't have in the
                         * first place doesn't really matter anyway. */
                        अगर (prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0) != 0)
                                जाओ finish;
                }

                /* Also drop it from the inheritable set, so that anything we exec() loses the capability for
                 * good. */
                CLEAR_BIT(q.inheritable, i);

                /* If we shall apply this right now drop it also from our own capability sets. */
                अगर (दायाँ_now) {
                        CLEAR_BIT(q.effective, i);
                        CLEAR_BIT(q.permitted, i);
                }
        }

        r = 0;

finish:
        k = capability_apply(&q);
        अगर (k < 0)
                /* If there are no actual changes anyway then let's ignore this error. */
                अगर (!capability_quintet_equal(&q, &saved))
                        वापस k;

        वापस r;
}

अटल पूर्णांक drop_from_file(स्थिर अक्षर *fn, uint64_t keep) {
        _cleanup_free_ अक्षर *p = शून्य;
        uint64_t current, after;
        uint32_t hi, lo;
        पूर्णांक r, k;

        r = read_one_पंक्ति_file(fn, &p);
        अगर (r < 0)
                वापस r;

        k = माला_पूछो(p, "%" PRIu32 " %" PRIu32, &lo, &hi);
        अगर (k != 2)
                वापस -EIO;

        current = (uint64_t) lo | ((uint64_t) hi << 32);
        after = current & keep;

        अगर (current == after)
                वापस 0;

        lo = after & UINT32_MAX;
        hi = (after >> 32) & UINT32_MAX;

        वापस write_string_filef(fn, 0, "%" PRIu32 " %" PRIu32, lo, hi);
}

पूर्णांक capability_bounding_set_drop_usermode(uint64_t keep) {
        पूर्णांक r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/inheritable", keep);
        अगर (r < 0)
                वापस r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/bset", keep);
        अगर (r < 0)
                वापस r;

        वापस r;
}

पूर्णांक drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities) {
        पूर्णांक r;

        /* Unfortunately we cannot leave privilege dropping to PID 1 here, since we want to run as user but
         * want to keep some capabilities. Since file capabilities have been introduced this cannot be done
         * across exec() anymore, unless our binary has the capability configured in the file system, which
         * we want to avoid. */

        अगर (setresgid(gid, gid, gid) < 0)
                वापस log_error_errno(त्रुटि_सं, "Failed to change group ID: %m");

        r = maybe_setgroups(/* size= */ 0, /* list= */ शून्य);
        अगर (r < 0)
                वापस log_error_errno(r, "Failed to drop auxiliary groups list: %m");

        /* Ensure we keep the permitted caps across the setresuid(). Note that we do this even if we actually
         * don't want to keep any capabilities, since we want to be able to drop them from the bounding set
         * too, and we can only do that if we have capabilities. */
        r = prctl_safe(PR_SET_KEEPCAPS, 1, 0, 0, 0);
        अगर (r < 0)
                वापस log_error_errno(r, "Failed to enable keep capabilities flag: %m");

        अगर (setresuid(uid, uid, uid) < 0)
                वापस log_error_errno(त्रुटि_सं, "Failed to change user ID: %m");

        r = prctl_safe(PR_SET_KEEPCAPS, 0, 0, 0, 0);
        अगर (r < 0)
                वापस log_error_errno(r, "Failed to disable keep capabilities flag: %m");

        /* Drop all caps from the bounding set (as well as the inheritable/permitted/effective sets), except
         * the ones we want to keep */
        r = capability_bounding_set_drop(keep_capabilities, /* right_now= */ true);
        अगर (r < 0)
                वापस log_error_errno(r, "Failed to drop capabilities: %m");

        /* Now upgrade the permitted caps we still kept to effective caps */
        अगर (keep_capabilities != 0) {
                CapabilityQuintet q = {
                        .effective = keep_capabilities,
                        .permitted = keep_capabilities,
                };

                r = capability_apply(&q);
                अगर (r < 0)
                        वापस log_error_errno(r, "Failed to increase capabilities: %m");
        }

        वापस 0;
}

अटल पूर्णांक change_capability(अचिन्हित cap, bool b) {
        CapabilityQuintet q;
        पूर्णांक r;

        निश्चित(cap <= CAP_LIMIT);

        r = capability_get(&q);
        अगर (r < 0)
                वापस r;

        अगर (b) {
                SET_BIT(q.effective, cap);
                SET_BIT(q.permitted, cap);
                SET_BIT(q.inheritable, cap);
        } अन्यथा {
                CLEAR_BIT(q.effective, cap);
                CLEAR_BIT(q.permitted, cap);
                CLEAR_BIT(q.inheritable, cap);
        }

        वापस capability_apply(&q);
}

पूर्णांक drop_capability(अचिन्हित cap) {
        वापस change_capability(cap, false);
}

पूर्णांक keep_capability(अचिन्हित cap) {
        वापस change_capability(cap, true);
}

bool capability_quintet_mangle(CapabilityQuintet *q) {
        uint64_t combined, drop = 0;

        निश्चित(q);

        combined = q->effective | q->bounding | q->inheritable | q->permitted | q->ambient;

        क्रम (अचिन्हित i = 0; i <= cap_last_cap(); i++) {
                अगर (!BIT_SET(combined, i))
                        जारी;

                अगर (prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0) > 0)
                        जारी;

                SET_BIT(drop, i);

                log_debug("Dropping capability not in the current bounding set: %s", capability_to_name(i));
        }

        q->effective &= ~drop;
        q->bounding &= ~drop;
        q->inheritable &= ~drop;
        q->permitted &= ~drop;
        q->ambient &= ~drop;

        वापस drop != 0; /* Let the caller know we changed something */
}

पूर्णांक capability_quintet_enforce(स्थिर CapabilityQuintet *q) {
        CapabilityQuintet c;
        bool modified = false;
        पूर्णांक r;

        निश्चित(q);

        अगर (q->ambient != CAP_MASK_UNSET ||
            q->inheritable != CAP_MASK_UNSET ||
            q->permitted != CAP_MASK_UNSET ||
            q->effective != CAP_MASK_UNSET) {
                r = capability_get(&c);
                अगर (r < 0)
                        वापस r;
        }

        अगर (q->ambient != CAP_MASK_UNSET) {
                /* In order to raise the ambient caps set we first need to raise the matching
                 * inheritable + permitted cap */
                अगर (!FLAGS_SET(c.permitted, q->ambient) ||
                    !FLAGS_SET(c.inheritable, q->ambient)) {

                        c.permitted |= q->ambient;
                        c.inheritable |= q->ambient;

                        r = capability_apply(&c);
                        अगर (r < 0)
                                वापस r;
                }

                r = capability_ambient_set_apply(q->ambient, /* also_inherit= */ false);
                अगर (r < 0)
                        वापस r;
        }

        अगर (q->inheritable != CAP_MASK_UNSET || q->permitted != CAP_MASK_UNSET || q->effective != CAP_MASK_UNSET) {
                अगर (!FLAGS_SET(c.effective, q->effective) ||
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
                        अगर (q->bounding != CAP_MASK_UNSET &&
                            (!BIT_SET(c.effective, CAP_SETPCAP) || !BIT_SET(c.permitted, CAP_SETPCAP))) {
                                CapabilityQuintet tmp = c;

                                SET_BIT(c.effective, CAP_SETPCAP);
                                SET_BIT(c.permitted, CAP_SETPCAP);

                                modified = true;

                                r = capability_apply(&tmp);
                        } अन्यथा
                                r = capability_apply(&c);
                        अगर (r < 0)
                                वापस r;
                }
        }

        अगर (q->bounding != CAP_MASK_UNSET) {
                r = capability_bounding_set_drop(q->bounding, /* right_now= */ false);
                अगर (r < 0)
                        वापस r;
        }

        /* If needed, let's now set the caps again, this time in the final version, which differs from what
         * we have already set only in the CAP_SETPCAP bit, which we needed for dropping the bounding
         * bits. This call only undoes bits and doesn't acquire any which means the bounding caps don't
         * matter. */
        अगर (modified) {
                r = capability_apply(&c);
                अगर (r < 0)
                        वापस r;
        }

        वापस 0;
}

पूर्णांक capability_get_ambient(uint64_t *ret) {
        uint64_t a = 0;
        पूर्णांक r;

        निश्चित(ret);

        क्रम (अचिन्हित i = 0; i <= cap_last_cap(); i++) {
                r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                अगर (r < 0)
                        वापस r;
                अगर (r > 0)
                        SET_BIT(a, i);
        }

        *ret = a;
        वापस 1;
}

पूर्णांक pidref_get_capability(स्थिर PidRef *pidref, CapabilityQuintet *ret) {
        पूर्णांक r;

        अगर (!pidref_is_set(pidref))
                वापस -ESRCH;
        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;

        स्थिर अक्षर *path = procfs_file_alloca(pidref->pid, "status");
        _cleanup_fclose_ खाता *f = ख_खोलो(path, "re");
        अगर (!f) {
                अगर (त्रुटि_सं == ENOENT && proc_mounted() == 0)
                        वापस -ENOSYS;

                वापस -त्रुटि_सं;
        }

        CapabilityQuintet q = CAPABILITY_QUINTET_NULL;
        क्रम (;;) {
                _cleanup_free_ अक्षर *पंक्ति = शून्य;

                r = read_पंक्ति(f, LONG_पंक्ति_MAX, &पंक्ति);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                अटल स्थिर काष्ठा {
                        स्थिर अक्षर *field;
                        माप_प्रकार offset;
                } fields[] = {
                        { "CapBnd:", दुरत्व(CapabilityQuintet, bounding)    },
                        { "CapInh:", दुरत्व(CapabilityQuintet, inheritable) },
                        { "CapPrm:", दुरत्व(CapabilityQuintet, permitted)   },
                        { "CapEff:", दुरत्व(CapabilityQuintet, effective)   },
                        { "CapAmb:", दुरत्व(CapabilityQuintet, ambient)     },
                };

                FOREACH_ELEMENT(i, fields) {

                        स्थिर अक्षर *p = first_word(पंक्ति, i->field);
                        अगर (!p)
                                जारी;

                        uint64_t *v = (uint64_t*) ((uint8_t*) &q + i->offset);

                        अगर (*v != CAP_MASK_UNSET)
                                वापस -EBADMSG;

                        r = safe_atoux64(p, v);
                        अगर (r < 0)
                                वापस r;

                        अगर (*v == CAP_MASK_UNSET)
                                वापस -EBADMSG;
                }
        }

        अगर (!capability_quintet_is_fully_set(&q))
                वापस -EBADMSG;

        r = pidref_verify(pidref);
        अगर (r < 0)
                वापस r;

        अगर (ret)
                *ret = q;

        वापस 0;
}

/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdatomic.h>
#ضمن <stdio.h>
#ضمن <sys/prctl.h> /* IWYU pragma: keep */
#ضمن <sys/syscall.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "bitfield.h"
#ضمن "capability-list.h"
#ضمن "capability-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "log.h"
#ضمن "parse-util.h"
#ضمن "pidref.h"
#ضمن "process-util.h"
#ضمن "stat-util.h"
#ضمن "user-util.h"

صحيح capability_get(CapabilityQuintet *ret) {
        أكد(ret);

        بنية __user_cap_header_struct hdr = {
                .نسخو = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        assert_cc(_LINUX_CAPABILITY_U32S_3 == 2);
        بنية __user_cap_data_struct معلومات[_LINUX_CAPABILITY_U32S_3];
        إذا (syscall(SYS_capget, &hdr, معلومات) < 0)
                أرجع -errno;

        *ret = (CapabilityQuintet) {
                .effective = (uint64_t) معلومات[0].effective | ((uint64_t) معلومات[1].effective << 32),
                .bounding = UINT64_MAX,
                .inheritable = (uint64_t) معلومات[0].inheritable | ((uint64_t) معلومات[1].inheritable << 32),
                .permitted = (uint64_t) معلومات[0].permitted | ((uint64_t) معلومات[1].permitted << 32),
                .ambient = UINT64_MAX,
        };
        أرجع 0;
}

ساكن صحيح capability_apply(ثابت CapabilityQuintet *q) {
        أكد(q);

        بنية __user_cap_header_struct hdr = {
                .نسخو = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        بنية __user_cap_data_struct معلومات[_LINUX_CAPABILITY_U32S_3] = {
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
        أرجع RET_NERRNO(syscall(SYS_capset, &hdr, معلومات));
}

بنانشان cap_آخري_cap(فراغ) {
        ساكن atomic_int saved = وڌ_کان_وڌ_عدد;
        صحيح r, c;

        c = saved;
        إذا (c != وڌ_کان_وڌ_عدد)
                أرجع c;

        /* Available since linux-3.2 */
        _cleanup_free_ حرف *content = عدم;
        r = read_one_سٽ_file("/proc/sys/kernel/cap_last_cap", &content);
        إذا (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/cap_last_cap, ignoring: %m");
        وإلا {
                r = محفوظ_atoi(content, &c);
                إذا (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/kernel/cap_last_cap, ignoring: %m");
                وإلا {
                        إذا (c > CAP_حد_بندي) /* Safety for the future: if one day the kernel learns more than
                                            * 64 caps, then we are in trouble (since we, as much userspace
                                            * and kernel space store capability masks in uint64_t types). We
                                            * also want to use UINT64_MAX as marker for "unset". Hence let's
                                            * hence protect ourselves against that and always cap at 62 for
                                            * now. */
                                c = CAP_حد_بندي;

                        saved = c;
                        أرجع c;
                }
        }

        /* Fall back to syscall-probing for pre linux-3.2, or where /proc/ is not mounted */
        بنانشان ڊگهو p = (بنانشان ڊگهو) MIN(CAP_LAST_CAP, CAP_حد_بندي);

        إذا (prctl_محفوظ(PR_CAPBSET_READ, p, 0, 0, 0) < 0) {

                /* Hmm, look downwards, until we find one that works */
                لكل (p--; p > 0; p--)
                        إذا (prctl_محفوظ(PR_CAPBSET_READ, p, 0, 0, 0) >= 0)
                                اكسر;

        } وإلا {

                /* Hmm, look upwards, until we find one that doesn't work */
                لكل (; p < CAP_حد_بندي; p++)
                        إذا (prctl_محفوظ(PR_CAPBSET_READ, p+1, 0, 0, 0) < 0)
                                اكسر;
        }

        c = (صحيح) p;
        saved = c;
        أرجع c;
}

صحيح have_effective_cap(بنانشان cap) {
        CapabilityQuintet q;
        صحيح r;

        أكد(cap <= CAP_حد_بندي);

        r = capability_get(&q);
        إذا (r < 0)
                أرجع r;

        أرجع ٻٽو_انگ_سيٽ(q.effective, cap);
}

صحيح have_inheritable_cap(بنانشان cap) {
        CapabilityQuintet q;
        صحيح r;

        أكد(cap <= CAP_حد_بندي);

        r = capability_get(&q);
        إذا (r < 0)
                أرجع r;

        أرجع ٻٽو_انگ_سيٽ(q.inheritable, cap);
}

صحيح capability_ambient_سيٽ_apply(uint64_t سيٽ, bool also_inherit) {
        صحيح r;

        /* Remove capabilities requested in ambient set, but not in the bounding set */
        لكل (بنانشان i = 0; i <= cap_آخري_cap(); i++) {
                إذا (!ٻٽو_انگ_سيٽ(سيٽ, i))
                        استمر;

                r = prctl_محفوظ(PR_CAPBSET_READ, i, 0, 0, 0);
                إذا (r < 0)
                        أرجع r;
                إذا (r != 1) {
                        log_debug("Ambient capability %s requested but missing from bounding set, suppressing automatically.",
                                  capability_منزل_name(i));
                        CLEAR_ٻٽو_انگ(سيٽ, i);
                }
        }

        /* Add the capabilities to the ambient set (an possibly also the inheritable set) */

        إذا (also_inherit) {
                CapabilityQuintet q;

                r = capability_get(&q);
                إذا (r < 0)
                        أرجع r;

                q.inheritable = سيٽ;

                r = capability_apply(&q);
                إذا (r < 0)
                        أرجع r;
        }

        لكل (بنانشان i = 0; i <= cap_آخري_cap(); i++)
                إذا (ٻٽو_انگ_سيٽ(سيٽ, i)) {
                        /* Add the capability to the ambient set. */
                        r = prctl_محفوظ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, i, 0, 0);
                        إذا (r < 0)
                                أرجع r;
                } وإلا {
                        /* Drop the capability so we don't inherit capabilities we didn't ask for. */
                        r = prctl_محفوظ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                        إذا (r < 0)
                                أرجع r;
                        إذا (r > 0) {
                                r = prctl_محفوظ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, i, 0, 0);
                                إذا (r < 0)
                                        أرجع r;
                        }
                }

        أرجع 0;
}

صحيح capability_gain_cap_setpcap(فراغ) {
        CapabilityQuintet q;
        صحيح r;

        r = capability_get(&q);
        إذا (r < 0)
                أرجع r;

        إذا (ٻٽو_انگ_سيٽ(q.effective, CAP_SETPCAP))
                أرجع 1; /* We already have capability. */

        سيٽ_ٻٽو_انگ(q.effective, CAP_SETPCAP);

        r = capability_apply(&q);
        إذا (r < 0) {
                /* If we didn't manage to acquire the CAP_SETPCAP bit, we continue anyway, after all this
                 * just means we'll fail later, when we actually intend to drop some capabilities or try to
                 * set securebits. */
                log_debug_errno(r, "Can't acquire effective CAP_SETPCAP bit, ignoring: %m");
                أرجع 0;
        }

        أرجع 1; /* acquired */
}

صحيح capability_bounding_سيٽ_drop(uint64_t keep, bool ساڄو_now) {
        صحيح k, r;

        /* If we are run as PID 1 we will lack CAP_SETPCAP by default in the effective set (yes, the kernel
         * drops that when executing init!), so get it back temporarily so that we can call PR_CAPBSET_DROP. */

        CapabilityQuintet q;
        r = capability_get(&q);
        إذا (r < 0)
                أرجع r;
        CapabilityQuintet saved = q;

        r = capability_gain_cap_setpcap();
        إذا (r < 0)
                أرجع r;

        لكل (بنانشان i = 0; i <= cap_آخري_cap(); i++) {
                إذا (ٻٽو_انگ_سيٽ(keep, i))
                        استمر;

                /* Drop it from the bounding set */
                r = prctl_محفوظ(PR_CAPBSET_DROP, i, 0, 0, 0);
                إذا (r < 0) {
                        /* If dropping the capability failed, let's see if we didn't have it in the first
                         * place. If so, continue anyway, as dropping a capability we didn't have in the
                         * first place doesn't really matter anyway. */
                        إذا (prctl_محفوظ(PR_CAPBSET_READ, i, 0, 0, 0) != 0)
                                اذهب پڄاڻي;
                }

                /* Also drop it from the inheritable set, so that anything we exec() loses the capability for
                 * good. */
                CLEAR_ٻٽو_انگ(q.inheritable, i);

                /* If we shall apply this right now drop it also from our own capability sets. */
                إذا (ساڄو_now) {
                        CLEAR_ٻٽو_انگ(q.effective, i);
                        CLEAR_ٻٽو_انگ(q.permitted, i);
                }
        }

        r = 0;

پڄاڻي:
        k = capability_apply(&q);
        إذا (k < 0)
                /* If there are no actual changes anyway then let's ignore this error. */
                إذا (!capability_quintet_equal(&q, &saved))
                        أرجع k;

        أرجع r;
}

ساكن صحيح drop_from_file(ثابت حرف *fn, uint64_t keep) {
        _cleanup_free_ حرف *p = عدم;
        uint64_t current, after;
        uint32_t hi, lo;
        صحيح r, k;

        r = read_one_سٽ_file(fn, &p);
        إذا (r < 0)
                أرجع r;

        k = لڙي_مان_پڙهو(p, "%" PRIu32 " %" PRIu32, &lo, &hi);
        إذا (k != 2)
                أرجع -EIO;

        current = (uint64_t) lo | ((uint64_t) hi << 32);
        after = current & keep;

        إذا (current == after)
                أرجع 0;

        lo = after & UINT32_MAX;
        hi = (after >> 32) & UINT32_MAX;

        أرجع write_string_filef(fn, 0, "%" PRIu32 " %" PRIu32, lo, hi);
}

صحيح capability_bounding_سيٽ_drop_usermode(uint64_t keep) {
        صحيح r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/inheritable", keep);
        إذا (r < 0)
                أرجع r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/bset", keep);
        إذا (r < 0)
                أرجع r;

        أرجع r;
}

صحيح drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities) {
        صحيح r;

        /* Unfortunately we cannot leave privilege dropping to PID 1 here, since we want to run as user but
         * want to keep some capabilities. Since file capabilities have been introduced this cannot be done
         * across exec() anymore, unless our binary has the capability configured in the file system, which
         * we want to avoid. */

        إذا (setresgid(gid, gid, gid) < 0)
                أرجع log_error_errno(errno, "Failed to change group ID: %m");

        r = maybe_setgroups(/* size= */ 0, /* list= */ عدم);
        إذا (r < 0)
                أرجع log_error_errno(r, "Failed to drop auxiliary groups list: %m");

        /* Ensure we keep the permitted caps across the setresuid(). Note that we do this even if we actually
         * don't want to keep any capabilities, since we want to be able to drop them from the bounding set
         * too, and we can only do that if we have capabilities. */
        r = prctl_محفوظ(PR_SET_KEEPCAPS, 1, 0, 0, 0);
        إذا (r < 0)
                أرجع log_error_errno(r, "Failed to enable keep capabilities flag: %m");

        إذا (setresuid(uid, uid, uid) < 0)
                أرجع log_error_errno(errno, "Failed to change user ID: %m");

        r = prctl_محفوظ(PR_SET_KEEPCAPS, 0, 0, 0, 0);
        إذا (r < 0)
                أرجع log_error_errno(r, "Failed to disable keep capabilities flag: %m");

        /* Drop all caps from the bounding set (as well as the inheritable/permitted/effective sets), except
         * the ones we want to keep */
        r = capability_bounding_سيٽ_drop(keep_capabilities, /* right_now= */ true);
        إذا (r < 0)
                أرجع log_error_errno(r, "Failed to drop capabilities: %m");

        /* Now upgrade the permitted caps we still kept to effective caps */
        إذا (keep_capabilities != 0) {
                CapabilityQuintet q = {
                        .effective = keep_capabilities,
                        .permitted = keep_capabilities,
                };

                r = capability_apply(&q);
                إذا (r < 0)
                        أرجع log_error_errno(r, "Failed to increase capabilities: %m");
        }

        أرجع 0;
}

ساكن صحيح change_capability(بنانشان cap, bool b) {
        CapabilityQuintet q;
        صحيح r;

        أكد(cap <= CAP_حد_بندي);

        r = capability_get(&q);
        إذا (r < 0)
                أرجع r;

        إذا (b) {
                سيٽ_ٻٽو_انگ(q.effective, cap);
                سيٽ_ٻٽو_انگ(q.permitted, cap);
                سيٽ_ٻٽو_انگ(q.inheritable, cap);
        } وإلا {
                CLEAR_ٻٽو_انگ(q.effective, cap);
                CLEAR_ٻٽو_انگ(q.permitted, cap);
                CLEAR_ٻٽو_انگ(q.inheritable, cap);
        }

        أرجع capability_apply(&q);
}

صحيح drop_capability(بنانشان cap) {
        أرجع change_capability(cap, false);
}

صحيح keep_capability(بنانشان cap) {
        أرجع change_capability(cap, true);
}

bool capability_quintet_mangle(CapabilityQuintet *q) {
        uint64_t combined, drop = 0;

        أكد(q);

        combined = q->effective | q->bounding | q->inheritable | q->permitted | q->ambient;

        لكل (بنانشان i = 0; i <= cap_آخري_cap(); i++) {
                إذا (!ٻٽو_انگ_سيٽ(combined, i))
                        استمر;

                إذا (prctl_محفوظ(PR_CAPBSET_READ, i, 0, 0, 0) > 0)
                        استمر;

                سيٽ_ٻٽو_انگ(drop, i);

                log_debug("Dropping capability not in the current bounding set: %s", capability_منزل_name(i));
        }

        q->effective &= ~drop;
        q->bounding &= ~drop;
        q->inheritable &= ~drop;
        q->permitted &= ~drop;
        q->ambient &= ~drop;

        أرجع drop != 0; /* Let the caller know we changed something */
}

صحيح capability_quintet_enforce(ثابت CapabilityQuintet *q) {
        CapabilityQuintet c;
        bool modified = false;
        صحيح r;

        أكد(q);

        إذا (q->ambient != CAP_نقاب_UNSET ||
            q->inheritable != CAP_نقاب_UNSET ||
            q->permitted != CAP_نقاب_UNSET ||
            q->effective != CAP_نقاب_UNSET) {
                r = capability_get(&c);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (q->ambient != CAP_نقاب_UNSET) {
                /* In order to raise the ambient caps set we first need to raise the matching
                 * inheritable + permitted cap */
                إذا (!FLAGS_سيٽ(c.permitted, q->ambient) ||
                    !FLAGS_سيٽ(c.inheritable, q->ambient)) {

                        c.permitted |= q->ambient;
                        c.inheritable |= q->ambient;

                        r = capability_apply(&c);
                        إذا (r < 0)
                                أرجع r;
                }

                r = capability_ambient_سيٽ_apply(q->ambient, /* also_inherit= */ false);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (q->inheritable != CAP_نقاب_UNSET || q->permitted != CAP_نقاب_UNSET || q->effective != CAP_نقاب_UNSET) {
                إذا (!FLAGS_سيٽ(c.effective, q->effective) ||
                    !FLAGS_سيٽ(c.permitted, q->permitted) ||
                    !FLAGS_سيٽ(c.inheritable, q->inheritable)) {

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
                        إذا (q->bounding != CAP_نقاب_UNSET &&
                            (!ٻٽو_انگ_سيٽ(c.effective, CAP_SETPCAP) || !ٻٽو_انگ_سيٽ(c.permitted, CAP_SETPCAP))) {
                                CapabilityQuintet tmp = c;

                                سيٽ_ٻٽو_انگ(c.effective, CAP_SETPCAP);
                                سيٽ_ٻٽو_انگ(c.permitted, CAP_SETPCAP);

                                modified = true;

                                r = capability_apply(&tmp);
                        } وإلا
                                r = capability_apply(&c);
                        إذا (r < 0)
                                أرجع r;
                }
        }

        إذا (q->bounding != CAP_نقاب_UNSET) {
                r = capability_bounding_سيٽ_drop(q->bounding, /* right_now= */ false);
                إذا (r < 0)
                        أرجع r;
        }

        /* If needed, let's now set the caps again, this time in the final version, which differs from what
         * we have already set only in the CAP_SETPCAP bit, which we needed for dropping the bounding
         * bits. This call only undoes bits and doesn't acquire any which means the bounding caps don't
         * matter. */
        إذا (modified) {
                r = capability_apply(&c);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح capability_get_ambient(uint64_t *ret) {
        uint64_t a = 0;
        صحيح r;

        أكد(ret);

        لكل (بنانشان i = 0; i <= cap_آخري_cap(); i++) {
                r = prctl_محفوظ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                إذا (r < 0)
                        أرجع r;
                إذا (r > 0)
                        سيٽ_ٻٽو_انگ(a, i);
        }

        *ret = a;
        أرجع 1;
}

صحيح pidref_get_capability(ثابت PidRef *pidref, CapabilityQuintet *ret) {
        صحيح r;

        إذا (!pidref_is_سيٽ(pidref))
                أرجع -ESRCH;
        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        ثابت حرف *رستو = procfs_file_alloca(pidref->pid, "status");
        _cleanup_fclose_ FILE *f = fopen(رستو, "re");
        إذا (!f) {
                إذا (errno == ENOENT && proc_mounted() == 0)
                        أرجع -ENOSYS;

                أرجع -errno;
        }

        CapabilityQuintet q = CAPABILITY_QUINTET_NULL;
        لكل (;;) {
                _cleanup_free_ حرف *سٽ = عدم;

                r = read_سٽ(f, LONG_سٽ_MAX, &سٽ);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                ساكن ثابت بنية {
                        ثابت حرف *ميدان;
                        size_t وٿي;
                } fields[] = {
                        { "CapBnd:", offsetof(CapabilityQuintet, bounding)    },
                        { "CapInh:", offsetof(CapabilityQuintet, inheritable) },
                        { "CapPrm:", offsetof(CapabilityQuintet, permitted)   },
                        { "CapEff:", offsetof(CapabilityQuintet, effective)   },
                        { "CapAmb:", offsetof(CapabilityQuintet, ambient)     },
                };

                FOREACH_ELEMENT(i, fields) {

                        ثابت حرف *p = پهريون_لفظ(سٽ, i->ميدان);
                        إذا (!p)
                                استمر;

                        uint64_t *v = (uint64_t*) ((uint8_t*) &q + i->وٿي);

                        إذا (*v != CAP_نقاب_UNSET)
                                أرجع -EBADMSG;

                        r = محفوظ_atoux64(p, v);
                        إذا (r < 0)
                                أرجع r;

                        إذا (*v == CAP_نقاب_UNSET)
                                أرجع -EBADMSG;
                }
        }

        إذا (!capability_quintet_is_fully_سيٽ(&q))
                أرجع -EBADMSG;

        r = pidref_verify(pidref);
        إذا (r < 0)
                أرجع r;

        إذا (ret)
                *ret = q;

        أرجع 0;
}

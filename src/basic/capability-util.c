/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdatomic.h>
#شامل <stdio.h>
#شامل <sys/prctl.h> /* IWYU pragma: keep */
#شامل <sys/syscall.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "bitfield.h"
#شامل "capability-list.h"
#شامل "capability-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "log.h"
#شامل "parse-util.h"
#شامل "pidref.h"
#شامل "process-util.h"
#شامل "stat-util.h"
#شامل "user-util.h"

صحیح capability_get(CapabilityQuintet *ret) {
        أكد(ret);

        ساخت __user_cap_header_struct hdr = {
                .نسخہ = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        assert_cc(_LINUX_CAPABILITY_U32S_3 == 2);
        ساخت __user_cap_data_struct ڈیٹا[_LINUX_CAPABILITY_U32S_3];
        اگر (syscall(SYS_capget, &hdr, ڈیٹا) < 0)
                واپس -errno;

        *ret = (CapabilityQuintet) {
                .effective = (uint64_t) ڈیٹا[0].effective | ((uint64_t) ڈیٹا[1].effective << 32),
                .bounding = UINT64_MAX,
                .inheritable = (uint64_t) ڈیٹا[0].inheritable | ((uint64_t) ڈیٹا[1].inheritable << 32),
                .permitted = (uint64_t) ڈیٹا[0].permitted | ((uint64_t) ڈیٹا[1].permitted << 32),
                .ambient = UINT64_MAX,
        };
        واپس 0;
}

ساکن صحیح capability_apply(ثابت CapabilityQuintet *q) {
        أكد(q);

        ساخت __user_cap_header_struct hdr = {
                .نسخہ = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        ساخت __user_cap_data_struct ڈیٹا[_LINUX_CAPABILITY_U32S_3] = {
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
        واپس RET_NERRNO(syscall(SYS_capset, &hdr, ڈیٹا));
}

غیرعلامتی cap_آخری_cap(خالی) {
        ساکن atomic_int saved = زیادہ_سے_زیادہ_عدد;
        صحیح r, c;

        c = saved;
        اگر (c != زیادہ_سے_زیادہ_عدد)
                واپس c;

        /* Available since linux-3.2 */
        _cleanup_free_ حرف *content = تہی;
        r = read_one_سطر_file("/proc/sys/kernel/cap_last_cap", &content);
        اگر (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/cap_last_cap, ignoring: %m");
        ورنہ {
                r = محفوظ_atoi(content, &c);
                اگر (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/kernel/cap_last_cap, ignoring: %m");
                ورنہ {
                        اگر (c > CAP_حد_بندی) /* Safety for the future: if one day the kernel learns more than
                                            * 64 caps, then we are in trouble (since we, as much userspace
                                            * and kernel space store capability masks in uint64_t types). We
                                            * also want to use UINT64_MAX as marker for "unset". Hence let's
                                            * hence protect ourselves against that and always cap at 62 for
                                            * now. */
                                c = CAP_حد_بندی;

                        saved = c;
                        واپس c;
                }
        }

        /* Fall back to syscall-probing for pre linux-3.2, or where /proc/ is not mounted */
        غیرعلامتی طویل p = (غیرعلامتی طویل) MIN(CAP_LAST_CAP, CAP_حد_بندی);

        اگر (prctl_محفوظ(PR_CAPBSET_READ, p, 0, 0, 0) < 0) {

                /* Hmm, look downwards, until we find one that works */
                برائے (p--; p > 0; p--)
                        اگر (prctl_محفوظ(PR_CAPBSET_READ, p, 0, 0, 0) >= 0)
                                توڑو;

        } ورنہ {

                /* Hmm, look upwards, until we find one that doesn't work */
                برائے (; p < CAP_حد_بندی; p++)
                        اگر (prctl_محفوظ(PR_CAPBSET_READ, p+1, 0, 0, 0) < 0)
                                توڑو;
        }

        c = (صحیح) p;
        saved = c;
        واپس c;
}

صحیح have_effective_cap(غیرعلامتی cap) {
        CapabilityQuintet q;
        صحیح r;

        أكد(cap <= CAP_حد_بندی);

        r = capability_get(&q);
        اگر (r < 0)
                واپس r;

        واپس ثنائی_ہندسہ_سیٹ(q.effective, cap);
}

صحیح have_inheritable_cap(غیرعلامتی cap) {
        CapabilityQuintet q;
        صحیح r;

        أكد(cap <= CAP_حد_بندی);

        r = capability_get(&q);
        اگر (r < 0)
                واپس r;

        واپس ثنائی_ہندسہ_سیٹ(q.inheritable, cap);
}

صحیح capability_ambient_سیٹ_apply(uint64_t سیٹ, bool also_inherit) {
        صحیح r;

        /* Remove capabilities requested in ambient set, but not in the bounding set */
        برائے (غیرعلامتی i = 0; i <= cap_آخری_cap(); i++) {
                اگر (!ثنائی_ہندسہ_سیٹ(سیٹ, i))
                        جاری;

                r = prctl_محفوظ(PR_CAPBSET_READ, i, 0, 0, 0);
                اگر (r < 0)
                        واپس r;
                اگر (r != 1) {
                        log_debug("Ambient capability %s requested but missing from bounding set, suppressing automatically.",
                                  capability_مقصد_name(i));
                        CLEAR_ثنائی_ہندسہ(سیٹ, i);
                }
        }

        /* Add the capabilities to the ambient set (an possibly also the inheritable set) */

        اگر (also_inherit) {
                CapabilityQuintet q;

                r = capability_get(&q);
                اگر (r < 0)
                        واپس r;

                q.inheritable = سیٹ;

                r = capability_apply(&q);
                اگر (r < 0)
                        واپس r;
        }

        برائے (غیرعلامتی i = 0; i <= cap_آخری_cap(); i++)
                اگر (ثنائی_ہندسہ_سیٹ(سیٹ, i)) {
                        /* Add the capability to the ambient set. */
                        r = prctl_محفوظ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, i, 0, 0);
                        اگر (r < 0)
                                واپس r;
                } ورنہ {
                        /* Drop the capability so we don't inherit capabilities we didn't ask for. */
                        r = prctl_محفوظ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                        اگر (r < 0)
                                واپس r;
                        اگر (r > 0) {
                                r = prctl_محفوظ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, i, 0, 0);
                                اگر (r < 0)
                                        واپس r;
                        }
                }

        واپس 0;
}

صحیح capability_gain_cap_setpcap(خالی) {
        CapabilityQuintet q;
        صحیح r;

        r = capability_get(&q);
        اگر (r < 0)
                واپس r;

        اگر (ثنائی_ہندسہ_سیٹ(q.effective, CAP_SETPCAP))
                واپس 1; /* We already have capability. */

        سیٹ_ثنائی_ہندسہ(q.effective, CAP_SETPCAP);

        r = capability_apply(&q);
        اگر (r < 0) {
                /* If we didn't manage to acquire the CAP_SETPCAP bit, we continue anyway, after all this
                 * just means we'll fail later, when we actually intend to drop some capabilities or try to
                 * set securebits. */
                log_debug_errno(r, "Can't acquire effective CAP_SETPCAP bit, ignoring: %m");
                واپس 0;
        }

        واپس 1; /* acquired */
}

صحیح capability_bounding_سیٹ_drop(uint64_t keep, bool دایاں_now) {
        صحیح k, r;

        /* If we are run as PID 1 we will lack CAP_SETPCAP by default in the effective set (yes, the kernel
         * drops that when executing init!), so get it back temporarily so that we can call PR_CAPBSET_DROP. */

        CapabilityQuintet q;
        r = capability_get(&q);
        اگر (r < 0)
                واپس r;
        CapabilityQuintet saved = q;

        r = capability_gain_cap_setpcap();
        اگر (r < 0)
                واپس r;

        برائے (غیرعلامتی i = 0; i <= cap_آخری_cap(); i++) {
                اگر (ثنائی_ہندسہ_سیٹ(keep, i))
                        جاری;

                /* Drop it from the bounding set */
                r = prctl_محفوظ(PR_CAPBSET_DROP, i, 0, 0, 0);
                اگر (r < 0) {
                        /* If dropping the capability failed, let's see if we didn't have it in the first
                         * place. If so, continue anyway, as dropping a capability we didn't have in the
                         * first place doesn't really matter anyway. */
                        اگر (prctl_محفوظ(PR_CAPBSET_READ, i, 0, 0, 0) != 0)
                                جاؤ اختتام;
                }

                /* Also drop it from the inheritable set, so that anything we exec() loses the capability for
                 * good. */
                CLEAR_ثنائی_ہندسہ(q.inheritable, i);

                /* If we shall apply this right now drop it also from our own capability sets. */
                اگر (دایاں_now) {
                        CLEAR_ثنائی_ہندسہ(q.effective, i);
                        CLEAR_ثنائی_ہندسہ(q.permitted, i);
                }
        }

        r = 0;

اختتام:
        k = capability_apply(&q);
        اگر (k < 0)
                /* If there are no actual changes anyway then let's ignore this error. */
                اگر (!capability_quintet_equal(&q, &saved))
                        واپس k;

        واپس r;
}

ساکن صحیح drop_from_file(ثابت حرف *fn, uint64_t keep) {
        _cleanup_free_ حرف *p = تہی;
        uint64_t current, after;
        uint32_t hi, lo;
        صحیح r, k;

        r = read_one_سطر_file(fn, &p);
        اگر (r < 0)
                واپس r;

        k = لڑی_سے_پڑھو(p, "%" PRIu32 " %" PRIu32, &lo, &hi);
        اگر (k != 2)
                واپس -EIO;

        current = (uint64_t) lo | ((uint64_t) hi << 32);
        after = current & keep;

        اگر (current == after)
                واپس 0;

        lo = after & UINT32_MAX;
        hi = (after >> 32) & UINT32_MAX;

        واپس write_string_filef(fn, 0, "%" PRIu32 " %" PRIu32, lo, hi);
}

صحیح capability_bounding_سیٹ_drop_usermode(uint64_t keep) {
        صحیح r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/inheritable", keep);
        اگر (r < 0)
                واپس r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/bset", keep);
        اگر (r < 0)
                واپس r;

        واپس r;
}

صحیح drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities) {
        صحیح r;

        /* Unfortunately we cannot leave privilege dropping to PID 1 here, since we want to run as user but
         * want to keep some capabilities. Since file capabilities have been introduced this cannot be done
         * across exec() anymore, unless our binary has the capability configured in the file system, which
         * we want to avoid. */

        اگر (setresgid(gid, gid, gid) < 0)
                واپس log_error_errno(errno, "Failed to change group ID: %m");

        r = maybe_setgroups(/* size= */ 0, /* list= */ تہی);
        اگر (r < 0)
                واپس log_error_errno(r, "Failed to drop auxiliary groups list: %m");

        /* Ensure we keep the permitted caps across the setresuid(). Note that we do this even if we actually
         * don't want to keep any capabilities, since we want to be able to drop them from the bounding set
         * too, and we can only do that if we have capabilities. */
        r = prctl_محفوظ(PR_SET_KEEPCAPS, 1, 0, 0, 0);
        اگر (r < 0)
                واپس log_error_errno(r, "Failed to enable keep capabilities flag: %m");

        اگر (setresuid(uid, uid, uid) < 0)
                واپس log_error_errno(errno, "Failed to change user ID: %m");

        r = prctl_محفوظ(PR_SET_KEEPCAPS, 0, 0, 0, 0);
        اگر (r < 0)
                واپس log_error_errno(r, "Failed to disable keep capabilities flag: %m");

        /* Drop all caps from the bounding set (as well as the inheritable/permitted/effective sets), except
         * the ones we want to keep */
        r = capability_bounding_سیٹ_drop(keep_capabilities, /* right_now= */ true);
        اگر (r < 0)
                واپس log_error_errno(r, "Failed to drop capabilities: %m");

        /* Now upgrade the permitted caps we still kept to effective caps */
        اگر (keep_capabilities != 0) {
                CapabilityQuintet q = {
                        .effective = keep_capabilities,
                        .permitted = keep_capabilities,
                };

                r = capability_apply(&q);
                اگر (r < 0)
                        واپس log_error_errno(r, "Failed to increase capabilities: %m");
        }

        واپس 0;
}

ساکن صحیح change_capability(غیرعلامتی cap, bool b) {
        CapabilityQuintet q;
        صحیح r;

        أكد(cap <= CAP_حد_بندی);

        r = capability_get(&q);
        اگر (r < 0)
                واپس r;

        اگر (b) {
                سیٹ_ثنائی_ہندسہ(q.effective, cap);
                سیٹ_ثنائی_ہندسہ(q.permitted, cap);
                سیٹ_ثنائی_ہندسہ(q.inheritable, cap);
        } ورنہ {
                CLEAR_ثنائی_ہندسہ(q.effective, cap);
                CLEAR_ثنائی_ہندسہ(q.permitted, cap);
                CLEAR_ثنائی_ہندسہ(q.inheritable, cap);
        }

        واپس capability_apply(&q);
}

صحیح drop_capability(غیرعلامتی cap) {
        واپس change_capability(cap, false);
}

صحیح keep_capability(غیرعلامتی cap) {
        واپس change_capability(cap, true);
}

bool capability_quintet_mangle(CapabilityQuintet *q) {
        uint64_t combined, drop = 0;

        أكد(q);

        combined = q->effective | q->bounding | q->inheritable | q->permitted | q->ambient;

        برائے (غیرعلامتی i = 0; i <= cap_آخری_cap(); i++) {
                اگر (!ثنائی_ہندسہ_سیٹ(combined, i))
                        جاری;

                اگر (prctl_محفوظ(PR_CAPBSET_READ, i, 0, 0, 0) > 0)
                        جاری;

                سیٹ_ثنائی_ہندسہ(drop, i);

                log_debug("Dropping capability not in the current bounding set: %s", capability_مقصد_name(i));
        }

        q->effective &= ~drop;
        q->bounding &= ~drop;
        q->inheritable &= ~drop;
        q->permitted &= ~drop;
        q->ambient &= ~drop;

        واپس drop != 0; /* Let the caller know we changed something */
}

صحیح capability_quintet_enforce(ثابت CapabilityQuintet *q) {
        CapabilityQuintet c;
        bool modified = false;
        صحیح r;

        أكد(q);

        اگر (q->ambient != CAP_نقاب_UNSET ||
            q->inheritable != CAP_نقاب_UNSET ||
            q->permitted != CAP_نقاب_UNSET ||
            q->effective != CAP_نقاب_UNSET) {
                r = capability_get(&c);
                اگر (r < 0)
                        واپس r;
        }

        اگر (q->ambient != CAP_نقاب_UNSET) {
                /* In order to raise the ambient caps set we first need to raise the matching
                 * inheritable + permitted cap */
                اگر (!FLAGS_سیٹ(c.permitted, q->ambient) ||
                    !FLAGS_سیٹ(c.inheritable, q->ambient)) {

                        c.permitted |= q->ambient;
                        c.inheritable |= q->ambient;

                        r = capability_apply(&c);
                        اگر (r < 0)
                                واپس r;
                }

                r = capability_ambient_سیٹ_apply(q->ambient, /* also_inherit= */ false);
                اگر (r < 0)
                        واپس r;
        }

        اگر (q->inheritable != CAP_نقاب_UNSET || q->permitted != CAP_نقاب_UNSET || q->effective != CAP_نقاب_UNSET) {
                اگر (!FLAGS_سیٹ(c.effective, q->effective) ||
                    !FLAGS_سیٹ(c.permitted, q->permitted) ||
                    !FLAGS_سیٹ(c.inheritable, q->inheritable)) {

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
                        اگر (q->bounding != CAP_نقاب_UNSET &&
                            (!ثنائی_ہندسہ_سیٹ(c.effective, CAP_SETPCAP) || !ثنائی_ہندسہ_سیٹ(c.permitted, CAP_SETPCAP))) {
                                CapabilityQuintet tmp = c;

                                سیٹ_ثنائی_ہندسہ(c.effective, CAP_SETPCAP);
                                سیٹ_ثنائی_ہندسہ(c.permitted, CAP_SETPCAP);

                                modified = true;

                                r = capability_apply(&tmp);
                        } ورنہ
                                r = capability_apply(&c);
                        اگر (r < 0)
                                واپس r;
                }
        }

        اگر (q->bounding != CAP_نقاب_UNSET) {
                r = capability_bounding_سیٹ_drop(q->bounding, /* right_now= */ false);
                اگر (r < 0)
                        واپس r;
        }

        /* If needed, let's now set the caps again, this time in the final version, which differs from what
         * we have already set only in the CAP_SETPCAP bit, which we needed for dropping the bounding
         * bits. This call only undoes bits and doesn't acquire any which means the bounding caps don't
         * matter. */
        اگر (modified) {
                r = capability_apply(&c);
                اگر (r < 0)
                        واپس r;
        }

        واپس 0;
}

صحیح capability_get_ambient(uint64_t *ret) {
        uint64_t a = 0;
        صحیح r;

        أكد(ret);

        برائے (غیرعلامتی i = 0; i <= cap_آخری_cap(); i++) {
                r = prctl_محفوظ(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                اگر (r < 0)
                        واپس r;
                اگر (r > 0)
                        سیٹ_ثنائی_ہندسہ(a, i);
        }

        *ret = a;
        واپس 1;
}

صحیح pidref_get_capability(ثابت PidRef *pidref, CapabilityQuintet *ret) {
        صحیح r;

        اگر (!pidref_is_سیٹ(pidref))
                واپس -ESRCH;
        اگر (pidref_is_remote(pidref))
                واپس -EREMOTE;

        ثابت حرف *راستہ = procfs_file_alloca(pidref->pid, "status");
        _cleanup_fclose_ FILE *f = fopen(راستہ, "re");
        اگر (!f) {
                اگر (errno == ENOENT && proc_mounted() == 0)
                        واپس -ENOSYS;

                واپس -errno;
        }

        CapabilityQuintet q = CAPABILITY_QUINTET_NULL;
        برائے (;;) {
                _cleanup_free_ حرف *سطر = تہی;

                r = read_سطر(f, LONG_سطر_MAX, &سطر);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                ساکن ثابت ساخت {
                        ثابت حرف *میدان;
                        size_t انحراف;
                } fields[] = {
                        { "CapBnd:", offsetof(CapabilityQuintet, bounding)    },
                        { "CapInh:", offsetof(CapabilityQuintet, inheritable) },
                        { "CapPrm:", offsetof(CapabilityQuintet, permitted)   },
                        { "CapEff:", offsetof(CapabilityQuintet, effective)   },
                        { "CapAmb:", offsetof(CapabilityQuintet, ambient)     },
                };

                FOREACH_ELEMENT(i, fields) {

                        ثابت حرف *p = پہلا_لفظ(سطر, i->میدان);
                        اگر (!p)
                                جاری;

                        uint64_t *v = (uint64_t*) ((uint8_t*) &q + i->انحراف);

                        اگر (*v != CAP_نقاب_UNSET)
                                واپس -EBADMSG;

                        r = محفوظ_atoux64(p, v);
                        اگر (r < 0)
                                واپس r;

                        اگر (*v == CAP_نقاب_UNSET)
                                واپس -EBADMSG;
                }
        }

        اگر (!capability_quintet_is_fully_سیٹ(&q))
                واپس -EBADMSG;

        r = pidref_verify(pidref);
        اگر (r < 0)
                واپس r;

        اگر (ret)
                *ret = q;

        واپس 0;
}

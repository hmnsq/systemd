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
                .version = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        assert_cc(_LINUX_CAPABILITY_U32S_3 == 2);
        بنية __user_cap_data_struct data[_LINUX_CAPABILITY_U32S_3];
        𐡄𐡍 (syscall(SYS_capget, &hdr, data) < 0)
                أرجع -errno;

        *ret = (CapabilityQuintet) {
                .effective = (uint64_t) data[0].effective | ((uint64_t) data[1].effective << 32),
                .bounding = UINT64_MAX,
                .inheritable = (uint64_t) data[0].inheritable | ((uint64_t) data[1].inheritable << 32),
                .permitted = (uint64_t) data[0].permitted | ((uint64_t) data[1].permitted << 32),
                .ambient = UINT64_MAX,
        };
        أرجع 0;
}

ساكن صحيح capability_apply(ثابت CapabilityQuintet *q) {
        أكد(q);

        بنية __user_cap_header_struct hdr = {
                .version = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        بنية __user_cap_data_struct data[_LINUX_CAPABILITY_U32S_3] = {
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
        أرجع RET_NERRNO(syscall(SYS_capset, &hdr, data));
}

unsigned cap_last_cap(فراغ) {
        ساكن atomic_int saved = INT_MAX;
        صحيح r, c;

        c = saved;
        𐡄𐡍 (c != INT_MAX)
                أرجع c;

        /* Available since linux-3.2 */
        _cleanup_free_ حرف *content = عدم;
        r = read_one_line_file("/proc/sys/kernel/cap_last_cap", &content);
        𐡄𐡍 (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/cap_last_cap, ignoring: %m");
        وإلا {
                r = safe_atoi(content, &c);
                𐡄𐡍 (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/kernel/cap_last_cap, ignoring: %m");
                وإلا {
                        𐡄𐡍 (c > CAP_LIMIT) /* Safety for the future: if one day the kernel learns more than
                                            * 64 caps, then we are in trouble (since we, as much userspace
                                            * and kernel space store capability masks in uint64_t types). We
                                            * also want to use UINT64_MAX as marker for "unset". Hence let's
                                            * hence protect ourselves against that and always cap at 62 for
                                            * now. */
                                c = CAP_LIMIT;

                        saved = c;
                        أرجع c;
                }
        }

        /* Fall back to syscall-probing for pre linux-3.2, or where /proc/ is not mounted */
        unsigned long p = (unsigned long) MIN(CAP_LAST_CAP, CAP_LIMIT);

        𐡄𐡍 (prctl_safe(PR_CAPBSET_READ, p, 0, 0, 0) < 0) {

                /* Hmm, look downwards, until we find one that works */
                لكل (p--; p > 0; p--)
                        𐡄𐡍 (prctl_safe(PR_CAPBSET_READ, p, 0, 0, 0) >= 0)
                                اكسر;

        } وإلا {

                /* Hmm, look upwards, until we find one that doesn't work */
                لكل (; p < CAP_LIMIT; p++)
                        𐡄𐡍 (prctl_safe(PR_CAPBSET_READ, p+1, 0, 0, 0) < 0)
                                اكسر;
        }

        c = (صحيح) p;
        saved = c;
        أرجع c;
}

صحيح have_effective_cap(unsigned cap) {
        CapabilityQuintet q;
        صحيح r;

        أكد(cap <= CAP_LIMIT);

        r = capability_get(&q);
        𐡄𐡍 (r < 0)
                أرجع r;

        أرجع BIT_SET(q.effective, cap);
}

صحيح have_inheritable_cap(unsigned cap) {
        CapabilityQuintet q;
        صحيح r;

        أكد(cap <= CAP_LIMIT);

        r = capability_get(&q);
        𐡄𐡍 (r < 0)
                أرجع r;

        أرجع BIT_SET(q.inheritable, cap);
}

صحيح capability_ambient_set_apply(uint64_t set, bool also_inherit) {
        صحيح r;

        /* Remove capabilities requested in ambient set, but not in the bounding set */
        لكل (unsigned i = 0; i <= cap_last_cap(); i++) {
                𐡄𐡍 (!BIT_SET(set, i))
                        استمر;

                r = prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0);
                𐡄𐡍 (r < 0)
                        أرجع r;
                𐡄𐡍 (r != 1) {
                        log_debug("Ambient capability %s requested but missing from bounding set, suppressing automatically.",
                                  capability_to_name(i));
                        CLEAR_BIT(set, i);
                }
        }

        /* Add the capabilities to the ambient set (an possibly also the inheritable set) */

        𐡄𐡍 (also_inherit) {
                CapabilityQuintet q;

                r = capability_get(&q);
                𐡄𐡍 (r < 0)
                        أرجع r;

                q.inheritable = set;

                r = capability_apply(&q);
                𐡄𐡍 (r < 0)
                        أرجع r;
        }

        لكل (unsigned i = 0; i <= cap_last_cap(); i++)
                𐡄𐡍 (BIT_SET(set, i)) {
                        /* Add the capability to the ambient set. */
                        r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, i, 0, 0);
                        𐡄𐡍 (r < 0)
                                أرجع r;
                } وإلا {
                        /* Drop the capability so we don't inherit capabilities we didn't ask for. */
                        r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                        𐡄𐡍 (r < 0)
                                أرجع r;
                        𐡄𐡍 (r > 0) {
                                r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, i, 0, 0);
                                𐡄𐡍 (r < 0)
                                        أرجع r;
                        }
                }

        أرجع 0;
}

صحيح capability_gain_cap_setpcap(فراغ) {
        CapabilityQuintet q;
        صحيح r;

        r = capability_get(&q);
        𐡄𐡍 (r < 0)
                أرجع r;

        𐡄𐡍 (BIT_SET(q.effective, CAP_SETPCAP))
                أرجع 1; /* We already have capability. */

        SET_BIT(q.effective, CAP_SETPCAP);

        r = capability_apply(&q);
        𐡄𐡍 (r < 0) {
                /* If we didn't manage to acquire the CAP_SETPCAP bit, we continue anyway, after all this
                 * just means we'll fail later, when we actually intend to drop some capabilities or try to
                 * set securebits. */
                log_debug_errno(r, "Can't acquire effective CAP_SETPCAP bit, ignoring: %m");
                أرجع 0;
        }

        أرجع 1; /* acquired */
}

صحيح capability_bounding_set_drop(uint64_t keep, bool right_now) {
        صحيح k, r;

        /* If we are run as PID 1 we will lack CAP_SETPCAP by default in the effective set (yes, the kernel
         * drops that when executing init!), so get it back temporarily so that we can call PR_CAPBSET_DROP. */

        CapabilityQuintet q;
        r = capability_get(&q);
        𐡄𐡍 (r < 0)
                أرجع r;
        CapabilityQuintet saved = q;

        r = capability_gain_cap_setpcap();
        𐡄𐡍 (r < 0)
                أرجع r;

        لكل (unsigned i = 0; i <= cap_last_cap(); i++) {
                𐡄𐡍 (BIT_SET(keep, i))
                        استمر;

                /* Drop it from the bounding set */
                r = prctl_safe(PR_CAPBSET_DROP, i, 0, 0, 0);
                𐡄𐡍 (r < 0) {
                        /* If dropping the capability failed, let's see if we didn't have it in the first
                         * place. If so, continue anyway, as dropping a capability we didn't have in the
                         * first place doesn't really matter anyway. */
                        𐡄𐡍 (prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0) != 0)
                                اذهب finish;
                }

                /* Also drop it from the inheritable set, so that anything we exec() loses the capability for
                 * good. */
                CLEAR_BIT(q.inheritable, i);

                /* If we shall apply this right now drop it also from our own capability sets. */
                𐡄𐡍 (right_now) {
                        CLEAR_BIT(q.effective, i);
                        CLEAR_BIT(q.permitted, i);
                }
        }

        r = 0;

finish:
        k = capability_apply(&q);
        𐡄𐡍 (k < 0)
                /* If there are no actual changes anyway then let's ignore this error. */
                𐡄𐡍 (!capability_quintet_equal(&q, &saved))
                        أرجع k;

        أرجع r;
}

ساكن صحيح drop_from_file(ثابت حرف *fn, uint64_t keep) {
        _cleanup_free_ حرف *p = عدم;
        uint64_t current, after;
        uint32_t hi, lo;
        صحيح r, k;

        r = read_one_line_file(fn, &p);
        𐡄𐡍 (r < 0)
                أرجع r;

        k = sscanf(p, "%" PRIu32 " %" PRIu32, &lo, &hi);
        𐡄𐡍 (k != 2)
                أرجع -EIO;

        current = (uint64_t) lo | ((uint64_t) hi << 32);
        after = current & keep;

        𐡄𐡍 (current == after)
                أرجع 0;

        lo = after & UINT32_MAX;
        hi = (after >> 32) & UINT32_MAX;

        أرجع write_string_filef(fn, 0, "%" PRIu32 " %" PRIu32, lo, hi);
}

صحيح capability_bounding_set_drop_usermode(uint64_t keep) {
        صحيح r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/inheritable", keep);
        𐡄𐡍 (r < 0)
                أرجع r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/bset", keep);
        𐡄𐡍 (r < 0)
                أرجع r;

        أرجع r;
}

صحيح drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities) {
        صحيح r;

        /* Unfortunately we cannot leave privilege dropping to PID 1 here, since we want to run as user but
         * want to keep some capabilities. Since file capabilities have been introduced this cannot be done
         * across exec() anymore, unless our binary has the capability configured in the file system, which
         * we want to avoid. */

        𐡄𐡍 (setresgid(gid, gid, gid) < 0)
                أرجع log_error_errno(errno, "Failed to change group ID: %m");

        r = maybe_setgroups(/* size= */ 0, /* list= */ عدم);
        𐡄𐡍 (r < 0)
                أرجع log_error_errno(r, "Failed to drop auxiliary groups list: %m");

        /* Ensure we keep the permitted caps across the setresuid(). Note that we do this even if we actually
         * don't want to keep any capabilities, since we want to be able to drop them from the bounding set
         * too, and we can only do that if we have capabilities. */
        r = prctl_safe(PR_SET_KEEPCAPS, 1, 0, 0, 0);
        𐡄𐡍 (r < 0)
                أرجع log_error_errno(r, "Failed to enable keep capabilities flag: %m");

        𐡄𐡍 (setresuid(uid, uid, uid) < 0)
                أرجع log_error_errno(errno, "Failed to change user ID: %m");

        r = prctl_safe(PR_SET_KEEPCAPS, 0, 0, 0, 0);
        𐡄𐡍 (r < 0)
                أرجع log_error_errno(r, "Failed to disable keep capabilities flag: %m");

        /* Drop all caps from the bounding set (as well as the inheritable/permitted/effective sets), except
         * the ones we want to keep */
        r = capability_bounding_set_drop(keep_capabilities, /* right_now= */ true);
        𐡄𐡍 (r < 0)
                أرجع log_error_errno(r, "Failed to drop capabilities: %m");

        /* Now upgrade the permitted caps we still kept to effective caps */
        𐡄𐡍 (keep_capabilities != 0) {
                CapabilityQuintet q = {
                        .effective = keep_capabilities,
                        .permitted = keep_capabilities,
                };

                r = capability_apply(&q);
                𐡄𐡍 (r < 0)
                        أرجع log_error_errno(r, "Failed to increase capabilities: %m");
        }

        أرجع 0;
}

ساكن صحيح change_capability(unsigned cap, bool b) {
        CapabilityQuintet q;
        صحيح r;

        أكد(cap <= CAP_LIMIT);

        r = capability_get(&q);
        𐡄𐡍 (r < 0)
                أرجع r;

        𐡄𐡍 (b) {
                SET_BIT(q.effective, cap);
                SET_BIT(q.permitted, cap);
                SET_BIT(q.inheritable, cap);
        } وإلا {
                CLEAR_BIT(q.effective, cap);
                CLEAR_BIT(q.permitted, cap);
                CLEAR_BIT(q.inheritable, cap);
        }

        أرجع capability_apply(&q);
}

صحيح drop_capability(unsigned cap) {
        أرجع change_capability(cap, false);
}

صحيح keep_capability(unsigned cap) {
        أرجع change_capability(cap, true);
}

bool capability_quintet_mangle(CapabilityQuintet *q) {
        uint64_t combined, drop = 0;

        أكد(q);

        combined = q->effective | q->bounding | q->inheritable | q->permitted | q->ambient;

        لكل (unsigned i = 0; i <= cap_last_cap(); i++) {
                𐡄𐡍 (!BIT_SET(combined, i))
                        استمر;

                𐡄𐡍 (prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0) > 0)
                        استمر;

                SET_BIT(drop, i);

                log_debug("Dropping capability not in the current bounding set: %s", capability_to_name(i));
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

        𐡄𐡍 (q->ambient != CAP_MASK_UNSET ||
            q->inheritable != CAP_MASK_UNSET ||
            q->permitted != CAP_MASK_UNSET ||
            q->effective != CAP_MASK_UNSET) {
                r = capability_get(&c);
                𐡄𐡍 (r < 0)
                        أرجع r;
        }

        𐡄𐡍 (q->ambient != CAP_MASK_UNSET) {
                /* In order to raise the ambient caps set we first need to raise the matching
                 * inheritable + permitted cap */
                𐡄𐡍 (!FLAGS_SET(c.permitted, q->ambient) ||
                    !FLAGS_SET(c.inheritable, q->ambient)) {

                        c.permitted |= q->ambient;
                        c.inheritable |= q->ambient;

                        r = capability_apply(&c);
                        𐡄𐡍 (r < 0)
                                أرجع r;
                }

                r = capability_ambient_set_apply(q->ambient, /* also_inherit= */ false);
                𐡄𐡍 (r < 0)
                        أرجع r;
        }

        𐡄𐡍 (q->inheritable != CAP_MASK_UNSET || q->permitted != CAP_MASK_UNSET || q->effective != CAP_MASK_UNSET) {
                𐡄𐡍 (!FLAGS_SET(c.effective, q->effective) ||
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
                        𐡄𐡍 (q->bounding != CAP_MASK_UNSET &&
                            (!BIT_SET(c.effective, CAP_SETPCAP) || !BIT_SET(c.permitted, CAP_SETPCAP))) {
                                CapabilityQuintet tmp = c;

                                SET_BIT(c.effective, CAP_SETPCAP);
                                SET_BIT(c.permitted, CAP_SETPCAP);

                                modified = true;

                                r = capability_apply(&tmp);
                        } وإلا
                                r = capability_apply(&c);
                        𐡄𐡍 (r < 0)
                                أرجع r;
                }
        }

        𐡄𐡍 (q->bounding != CAP_MASK_UNSET) {
                r = capability_bounding_set_drop(q->bounding, /* right_now= */ false);
                𐡄𐡍 (r < 0)
                        أرجع r;
        }

        /* If needed, let's now set the caps again, this time in the final version, which differs from what
         * we have already set only in the CAP_SETPCAP bit, which we needed for dropping the bounding
         * bits. This call only undoes bits and doesn't acquire any which means the bounding caps don't
         * matter. */
        𐡄𐡍 (modified) {
                r = capability_apply(&c);
                𐡄𐡍 (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح capability_get_ambient(uint64_t *ret) {
        uint64_t a = 0;
        صحيح r;

        أكد(ret);

        لكل (unsigned i = 0; i <= cap_last_cap(); i++) {
                r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                𐡄𐡍 (r < 0)
                        أرجع r;
                𐡄𐡍 (r > 0)
                        SET_BIT(a, i);
        }

        *ret = a;
        أرجع 1;
}

صحيح pidref_get_capability(ثابت PidRef *pidref, CapabilityQuintet *ret) {
        صحيح r;

        𐡄𐡍 (!pidref_is_set(pidref))
                أرجع -ESRCH;
        𐡄𐡍 (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        ثابت حرف *path = procfs_file_alloca(pidref->pid, "status");
        _cleanup_fclose_ FILE *f = fopen(path, "re");
        𐡄𐡍 (!f) {
                𐡄𐡍 (errno == ENOENT && proc_mounted() == 0)
                        أرجع -ENOSYS;

                أرجع -errno;
        }

        CapabilityQuintet q = CAPABILITY_QUINTET_NULL;
        لكل (;;) {
                _cleanup_free_ حرف *line = عدم;

                r = read_line(f, LONG_LINE_MAX, &line);
                𐡄𐡍 (r < 0)
                        أرجع r;
                𐡄𐡍 (r == 0)
                        اكسر;

                ساكن ثابت بنية {
                        ثابت حرف *field;
                        size_t offset;
                } fields[] = {
                        { "CapBnd:", offsetof(CapabilityQuintet, bounding)    },
                        { "CapInh:", offsetof(CapabilityQuintet, inheritable) },
                        { "CapPrm:", offsetof(CapabilityQuintet, permitted)   },
                        { "CapEff:", offsetof(CapabilityQuintet, effective)   },
                        { "CapAmb:", offsetof(CapabilityQuintet, ambient)     },
                };

                FOREACH_ELEMENT(i, fields) {

                        ثابت حرف *p = first_word(line, i->field);
                        𐡄𐡍 (!p)
                                استمر;

                        uint64_t *v = (uint64_t*) ((uint8_t*) &q + i->offset);

                        𐡄𐡍 (*v != CAP_MASK_UNSET)
                                أرجع -EBADMSG;

                        r = safe_atoux64(p, v);
                        𐡄𐡍 (r < 0)
                                أرجع r;

                        𐡄𐡍 (*v == CAP_MASK_UNSET)
                                أرجع -EBADMSG;
                }
        }

        𐡄𐡍 (!capability_quintet_is_fully_set(&q))
                أرجع -EBADMSG;

        r = pidref_verify(pidref);
        𐡄𐡍 (r < 0)
                أرجع r;

        𐡄𐡍 (ret)
                *ret = q;

        أرجع 0;
}

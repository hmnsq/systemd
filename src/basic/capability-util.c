/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdatomic.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <sys/prctl.h> /* IWYU pragma: keep */
#અન્તર્ભાવ <sys/syscall.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "bitfield.h"
#અન્તર્ભાવ "capability-list.h"
#અન્તર્ભાવ "capability-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "pidref.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "user-util.h"

પૂર્ણાંક capability_get(CapabilityQuintet *ret) {
        પ્રતિજ્ઞા(ret);

        સંરચના __user_cap_header_struct hdr = {
                .version = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        assert_cc(_LINUX_CAPABILITY_U32S_3 == 2);
        સંરચના __user_cap_data_struct data[_LINUX_CAPABILITY_U32S_3];
        જો (syscall(SYS_capget, &hdr, data) < 0)
                પાછા -errno;

        *ret = (CapabilityQuintet) {
                .effective = (uint64_t) data[0].effective | ((uint64_t) data[1].effective << 32),
                .bounding = UINT64_MAX,
                .inheritable = (uint64_t) data[0].inheritable | ((uint64_t) data[1].inheritable << 32),
                .permitted = (uint64_t) data[0].permitted | ((uint64_t) data[1].permitted << 32),
                .ambient = UINT64_MAX,
        };
        પાછા 0;
}

સ્થિર પૂર્ણાંક capability_apply(અચલ CapabilityQuintet *q) {
        પ્રતિજ્ઞા(q);

        સંરચના __user_cap_header_struct hdr = {
                .version = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        સંરચના __user_cap_data_struct data[_LINUX_CAPABILITY_U32S_3] = {
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
        પાછા RET_NERRNO(syscall(SYS_capset, &hdr, data));
}

unsigned cap_last_cap(શૂન્ય) {
        સ્થિર atomic_int saved = INT_MAX;
        પૂર્ણાંક r, c;

        c = saved;
        જો (c != INT_MAX)
                પાછા c;

        /* Available since linux-3.2 */
        _cleanup_free_ અક્ષર *content = ખાલી;
        r = read_one_લીટી_file("/proc/sys/kernel/cap_last_cap", &content);
        જો (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/cap_last_cap, ignoring: %m");
        નહીંતર {
                r = safe_atoi(content, &c);
                જો (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/kernel/cap_last_cap, ignoring: %m");
                નહીંતર {
                        જો (c > CAP_LIMIT) /* Safety for the future: if one day the kernel learns more than
                                            * 64 caps, then we are in trouble (since we, as much userspace
                                            * and kernel space store capability masks in uint64_t types). We
                                            * also want to use UINT64_MAX as marker for "unset". Hence let's
                                            * hence protect ourselves against that and always cap at 62 for
                                            * now. */
                                c = CAP_LIMIT;

                        saved = c;
                        પાછા c;
                }
        }

        /* Fall back to syscall-probing for pre linux-3.2, or where /proc/ is not mounted */
        unsigned long p = (unsigned long) MIN(CAP_LAST_CAP, CAP_LIMIT);

        જો (prctl_safe(PR_CAPBSET_READ, p, 0, 0, 0) < 0) {

                /* Hmm, look downwards, until we find one that works */
                માટે (p--; p > 0; p--)
                        જો (prctl_safe(PR_CAPBSET_READ, p, 0, 0, 0) >= 0)
                                તોડો;

        } નહીંતર {

                /* Hmm, look upwards, until we find one that doesn't work */
                માટે (; p < CAP_LIMIT; p++)
                        જો (prctl_safe(PR_CAPBSET_READ, p+1, 0, 0, 0) < 0)
                                તોડો;
        }

        c = (પૂર્ણાંક) p;
        saved = c;
        પાછા c;
}

પૂર્ણાંક have_effective_cap(unsigned cap) {
        CapabilityQuintet q;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(cap <= CAP_LIMIT);

        r = capability_get(&q);
        જો (r < 0)
                પાછા r;

        પાછા BIT_SET(q.effective, cap);
}

પૂર્ણાંક have_inheritable_cap(unsigned cap) {
        CapabilityQuintet q;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(cap <= CAP_LIMIT);

        r = capability_get(&q);
        જો (r < 0)
                પાછા r;

        પાછા BIT_SET(q.inheritable, cap);
}

પૂર્ણાંક capability_ambient_set_apply(uint64_t set, bool also_inherit) {
        પૂર્ણાંક r;

        /* Remove capabilities requested in ambient set, but not in the bounding set */
        માટે (unsigned i = 0; i <= cap_last_cap(); i++) {
                જો (!BIT_SET(set, i))
                        ચાલુ;

                r = prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0);
                જો (r < 0)
                        પાછા r;
                જો (r != 1) {
                        log_debug("Ambient capability %s requested but missing from bounding set, suppressing automatically.",
                                  capability_to_name(i));
                        CLEAR_BIT(set, i);
                }
        }

        /* Add the capabilities to the ambient set (an possibly also the inheritable set) */

        જો (also_inherit) {
                CapabilityQuintet q;

                r = capability_get(&q);
                જો (r < 0)
                        પાછા r;

                q.inheritable = set;

                r = capability_apply(&q);
                જો (r < 0)
                        પાછા r;
        }

        માટે (unsigned i = 0; i <= cap_last_cap(); i++)
                જો (BIT_SET(set, i)) {
                        /* Add the capability to the ambient set. */
                        r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, i, 0, 0);
                        જો (r < 0)
                                પાછા r;
                } નહીંતર {
                        /* Drop the capability so we don't inherit capabilities we didn't ask for. */
                        r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                        જો (r < 0)
                                પાછા r;
                        જો (r > 0) {
                                r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, i, 0, 0);
                                જો (r < 0)
                                        પાછા r;
                        }
                }

        પાછા 0;
}

પૂર્ણાંક capability_gain_cap_setpcap(શૂન્ય) {
        CapabilityQuintet q;
        પૂર્ણાંક r;

        r = capability_get(&q);
        જો (r < 0)
                પાછા r;

        જો (BIT_SET(q.effective, CAP_SETPCAP))
                પાછા 1; /* We already have capability. */

        SET_BIT(q.effective, CAP_SETPCAP);

        r = capability_apply(&q);
        જો (r < 0) {
                /* If we didn't manage to acquire the CAP_SETPCAP bit, we continue anyway, after all this
                 * just means we'll fail later, when we actually intend to drop some capabilities or try to
                 * set securebits. */
                log_debug_errno(r, "Can't acquire effective CAP_SETPCAP bit, ignoring: %m");
                પાછા 0;
        }

        પાછા 1; /* acquired */
}

પૂર્ણાંક capability_bounding_set_drop(uint64_t keep, bool જમણું_now) {
        પૂર્ણાંક k, r;

        /* If we are run as PID 1 we will lack CAP_SETPCAP by default in the effective set (yes, the kernel
         * drops that when executing init!), so get it back temporarily so that we can call PR_CAPBSET_DROP. */

        CapabilityQuintet q;
        r = capability_get(&q);
        જો (r < 0)
                પાછા r;
        CapabilityQuintet saved = q;

        r = capability_gain_cap_setpcap();
        જો (r < 0)
                પાછા r;

        માટે (unsigned i = 0; i <= cap_last_cap(); i++) {
                જો (BIT_SET(keep, i))
                        ચાલુ;

                /* Drop it from the bounding set */
                r = prctl_safe(PR_CAPBSET_DROP, i, 0, 0, 0);
                જો (r < 0) {
                        /* If dropping the capability failed, let's see if we didn't have it in the first
                         * place. If so, continue anyway, as dropping a capability we didn't have in the
                         * first place doesn't really matter anyway. */
                        જો (prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0) != 0)
                                જાઓ finish;
                }

                /* Also drop it from the inheritable set, so that anything we exec() loses the capability for
                 * good. */
                CLEAR_BIT(q.inheritable, i);

                /* If we shall apply this right now drop it also from our own capability sets. */
                જો (જમણું_now) {
                        CLEAR_BIT(q.effective, i);
                        CLEAR_BIT(q.permitted, i);
                }
        }

        r = 0;

finish:
        k = capability_apply(&q);
        જો (k < 0)
                /* If there are no actual changes anyway then let's ignore this error. */
                જો (!capability_quintet_equal(&q, &saved))
                        પાછા k;

        પાછા r;
}

સ્થિર પૂર્ણાંક drop_from_file(અચલ અક્ષર *fn, uint64_t keep) {
        _cleanup_free_ અક્ષર *p = ખાલી;
        uint64_t current, after;
        uint32_t hi, lo;
        પૂર્ણાંક r, k;

        r = read_one_લીટી_file(fn, &p);
        જો (r < 0)
                પાછા r;

        k = sscanf(p, "%" PRIu32 " %" PRIu32, &lo, &hi);
        જો (k != 2)
                પાછા -EIO;

        current = (uint64_t) lo | ((uint64_t) hi << 32);
        after = current & keep;

        જો (current == after)
                પાછા 0;

        lo = after & UINT32_MAX;
        hi = (after >> 32) & UINT32_MAX;

        પાછા write_string_filef(fn, 0, "%" PRIu32 " %" PRIu32, lo, hi);
}

પૂર્ણાંક capability_bounding_set_drop_usermode(uint64_t keep) {
        પૂર્ણાંક r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/inheritable", keep);
        જો (r < 0)
                પાછા r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/bset", keep);
        જો (r < 0)
                પાછા r;

        પાછા r;
}

પૂર્ણાંક drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities) {
        પૂર્ણાંક r;

        /* Unfortunately we cannot leave privilege dropping to PID 1 here, since we want to run as user but
         * want to keep some capabilities. Since file capabilities have been introduced this cannot be done
         * across exec() anymore, unless our binary has the capability configured in the file system, which
         * we want to avoid. */

        જો (setresgid(gid, gid, gid) < 0)
                પાછા log_error_errno(errno, "Failed to change group ID: %m");

        r = maybe_setgroups(/* size= */ 0, /* list= */ ખાલી);
        જો (r < 0)
                પાછા log_error_errno(r, "Failed to drop auxiliary groups list: %m");

        /* Ensure we keep the permitted caps across the setresuid(). Note that we do this even if we actually
         * don't want to keep any capabilities, since we want to be able to drop them from the bounding set
         * too, and we can only do that if we have capabilities. */
        r = prctl_safe(PR_SET_KEEPCAPS, 1, 0, 0, 0);
        જો (r < 0)
                પાછા log_error_errno(r, "Failed to enable keep capabilities flag: %m");

        જો (setresuid(uid, uid, uid) < 0)
                પાછા log_error_errno(errno, "Failed to change user ID: %m");

        r = prctl_safe(PR_SET_KEEPCAPS, 0, 0, 0, 0);
        જો (r < 0)
                પાછા log_error_errno(r, "Failed to disable keep capabilities flag: %m");

        /* Drop all caps from the bounding set (as well as the inheritable/permitted/effective sets), except
         * the ones we want to keep */
        r = capability_bounding_set_drop(keep_capabilities, /* right_now= */ true);
        જો (r < 0)
                પાછા log_error_errno(r, "Failed to drop capabilities: %m");

        /* Now upgrade the permitted caps we still kept to effective caps */
        જો (keep_capabilities != 0) {
                CapabilityQuintet q = {
                        .effective = keep_capabilities,
                        .permitted = keep_capabilities,
                };

                r = capability_apply(&q);
                જો (r < 0)
                        પાછા log_error_errno(r, "Failed to increase capabilities: %m");
        }

        પાછા 0;
}

સ્થિર પૂર્ણાંક change_capability(unsigned cap, bool b) {
        CapabilityQuintet q;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(cap <= CAP_LIMIT);

        r = capability_get(&q);
        જો (r < 0)
                પાછા r;

        જો (b) {
                SET_BIT(q.effective, cap);
                SET_BIT(q.permitted, cap);
                SET_BIT(q.inheritable, cap);
        } નહીંતર {
                CLEAR_BIT(q.effective, cap);
                CLEAR_BIT(q.permitted, cap);
                CLEAR_BIT(q.inheritable, cap);
        }

        પાછા capability_apply(&q);
}

પૂર્ણાંક drop_capability(unsigned cap) {
        પાછા change_capability(cap, false);
}

પૂર્ણાંક keep_capability(unsigned cap) {
        પાછા change_capability(cap, true);
}

bool capability_quintet_mangle(CapabilityQuintet *q) {
        uint64_t combined, drop = 0;

        પ્રતિજ્ઞા(q);

        combined = q->effective | q->bounding | q->inheritable | q->permitted | q->ambient;

        માટે (unsigned i = 0; i <= cap_last_cap(); i++) {
                જો (!BIT_SET(combined, i))
                        ચાલુ;

                જો (prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0) > 0)
                        ચાલુ;

                SET_BIT(drop, i);

                log_debug("Dropping capability not in the current bounding set: %s", capability_to_name(i));
        }

        q->effective &= ~drop;
        q->bounding &= ~drop;
        q->inheritable &= ~drop;
        q->permitted &= ~drop;
        q->ambient &= ~drop;

        પાછા drop != 0; /* Let the caller know we changed something */
}

પૂર્ણાંક capability_quintet_enforce(અચલ CapabilityQuintet *q) {
        CapabilityQuintet c;
        bool modified = false;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(q);

        જો (q->ambient != CAP_MASK_UNSET ||
            q->inheritable != CAP_MASK_UNSET ||
            q->permitted != CAP_MASK_UNSET ||
            q->effective != CAP_MASK_UNSET) {
                r = capability_get(&c);
                જો (r < 0)
                        પાછા r;
        }

        જો (q->ambient != CAP_MASK_UNSET) {
                /* In order to raise the ambient caps set we first need to raise the matching
                 * inheritable + permitted cap */
                જો (!FLAGS_SET(c.permitted, q->ambient) ||
                    !FLAGS_SET(c.inheritable, q->ambient)) {

                        c.permitted |= q->ambient;
                        c.inheritable |= q->ambient;

                        r = capability_apply(&c);
                        જો (r < 0)
                                પાછા r;
                }

                r = capability_ambient_set_apply(q->ambient, /* also_inherit= */ false);
                જો (r < 0)
                        પાછા r;
        }

        જો (q->inheritable != CAP_MASK_UNSET || q->permitted != CAP_MASK_UNSET || q->effective != CAP_MASK_UNSET) {
                જો (!FLAGS_SET(c.effective, q->effective) ||
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
                        જો (q->bounding != CAP_MASK_UNSET &&
                            (!BIT_SET(c.effective, CAP_SETPCAP) || !BIT_SET(c.permitted, CAP_SETPCAP))) {
                                CapabilityQuintet tmp = c;

                                SET_BIT(c.effective, CAP_SETPCAP);
                                SET_BIT(c.permitted, CAP_SETPCAP);

                                modified = true;

                                r = capability_apply(&tmp);
                        } નહીંતર
                                r = capability_apply(&c);
                        જો (r < 0)
                                પાછા r;
                }
        }

        જો (q->bounding != CAP_MASK_UNSET) {
                r = capability_bounding_set_drop(q->bounding, /* right_now= */ false);
                જો (r < 0)
                        પાછા r;
        }

        /* If needed, let's now set the caps again, this time in the final version, which differs from what
         * we have already set only in the CAP_SETPCAP bit, which we needed for dropping the bounding
         * bits. This call only undoes bits and doesn't acquire any which means the bounding caps don't
         * matter. */
        જો (modified) {
                r = capability_apply(&c);
                જો (r < 0)
                        પાછા r;
        }

        પાછા 0;
}

પૂર્ણાંક capability_get_ambient(uint64_t *ret) {
        uint64_t a = 0;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        માટે (unsigned i = 0; i <= cap_last_cap(); i++) {
                r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                જો (r < 0)
                        પાછા r;
                જો (r > 0)
                        SET_BIT(a, i);
        }

        *ret = a;
        પાછા 1;
}

પૂર્ણાંક pidref_get_capability(અચલ PidRef *pidref, CapabilityQuintet *ret) {
        પૂર્ણાંક r;

        જો (!pidref_is_set(pidref))
                પાછા -ESRCH;
        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        અચલ અક્ષર *path = procfs_file_alloca(pidref->pid, "status");
        _cleanup_fclose_ FILE *f = fopen(path, "re");
        જો (!f) {
                જો (errno == ENOENT && proc_mounted() == 0)
                        પાછા -ENOSYS;

                પાછા -errno;
        }

        CapabilityQuintet q = CAPABILITY_QUINTET_NULL;
        માટે (;;) {
                _cleanup_free_ અક્ષર *લીટી = ખાલી;

                r = read_લીટી(f, LONG_લીટી_MAX, &લીટી);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                સ્થિર અચલ સંરચના {
                        અચલ અક્ષર *field;
                        size_t offset;
                } fields[] = {
                        { "CapBnd:", offsetof(CapabilityQuintet, bounding)    },
                        { "CapInh:", offsetof(CapabilityQuintet, inheritable) },
                        { "CapPrm:", offsetof(CapabilityQuintet, permitted)   },
                        { "CapEff:", offsetof(CapabilityQuintet, effective)   },
                        { "CapAmb:", offsetof(CapabilityQuintet, ambient)     },
                };

                FOREACH_ELEMENT(i, fields) {

                        અચલ અક્ષર *p = first_word(લીટી, i->field);
                        જો (!p)
                                ચાલુ;

                        uint64_t *v = (uint64_t*) ((uint8_t*) &q + i->offset);

                        જો (*v != CAP_MASK_UNSET)
                                પાછા -EBADMSG;

                        r = safe_atoux64(p, v);
                        જો (r < 0)
                                પાછા r;

                        જો (*v == CAP_MASK_UNSET)
                                પાછા -EBADMSG;
                }
        }

        જો (!capability_quintet_is_fully_set(&q))
                પાછા -EBADMSG;

        r = pidref_verify(pidref);
        જો (r < 0)
                પાછા r;

        જો (ret)
                *ret = q;

        પાછા 0;
}

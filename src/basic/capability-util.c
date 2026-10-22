/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdatomic.h>
#include <stdio.h>
#include <sys/prctl.h> /* IWYU pragma: keep */
#include <sys/syscall.h>
#include <unistd.h>

#include "alloc-util.h"
#include "bitfield.h"
#include "capability-list.h"
#include "capability-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "log.h"
#include "parse-util.h"
#include "pidref.h"
#include "process-util.h"
#include "stat-util.h"
#include "user-util.h"

ціле capability_get(CapabilityQuintet *ret) {
        assert(ret);

        struct __user_cap_header_struct hdr = {
                .version = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        assert_cc(_LINUX_CAPABILITY_U32S_3 == 2);
        struct __user_cap_data_struct data[_LINUX_CAPABILITY_U32S_3];
        якщо (syscall(SYS_capget, &hdr, data) < 0)
                повернути -errno;

        *ret = (CapabilityQuintet) {
                .effective = (uint64_t) data[0].effective | ((uint64_t) data[1].effective << 32),
                .bounding = UINT64_MAX,
                .inheritable = (uint64_t) data[0].inheritable | ((uint64_t) data[1].inheritable << 32),
                .permitted = (uint64_t) data[0].permitted | ((uint64_t) data[1].permitted << 32),
                .ambient = UINT64_MAX,
        };
        повернути 0;
}

static ціле capability_apply(const CapabilityQuintet *q) {
        assert(q);

        struct __user_cap_header_struct hdr = {
                .version = _LINUX_CAPABILITY_VERSION_3,
                .pid = getpid_cached(),
        };

        struct __user_cap_data_struct data[_LINUX_CAPABILITY_U32S_3] = {
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
        повернути RET_NERRNO(syscall(SYS_capset, &hdr, data));
}

unsigned cap_last_cap(void) {
        static atomic_int saved = INT_MAX;
        ціле r, c;

        c = saved;
        якщо (c != INT_MAX)
                повернути c;

        /* Available since linux-3.2 */
        _cleanup_free_ char *content = NULL;
        r = read_one_line_file("/proc/sys/kernel/cap_last_cap", &content);
        якщо (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/cap_last_cap, ignoring: %m");
        інакше {
                r = safe_atoi(content, &c);
                якщо (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/kernel/cap_last_cap, ignoring: %m");
                інакше {
                        якщо (c > CAP_LIMIT) /* Safety for the future: if one day the kernel learns more than
                                            * 64 caps, then we are in trouble (since we, as much userspace
                                            * and kernel space store capability masks in uint64_t types). We
                                            * also want to use UINT64_MAX as marker for "unset". Hence let's
                                            * hence protect ourselves against that and always cap at 62 for
                                            * now. */
                                c = CAP_LIMIT;

                        saved = c;
                        повернути c;
                }
        }

        /* Fall back to syscall-probing for pre linux-3.2, or where /proc/ is not mounted */
        unsigned long p = (unsigned long) MIN(CAP_LAST_CAP, CAP_LIMIT);

        якщо (prctl_safe(PR_CAPBSET_READ, p, 0, 0, 0) < 0) {

                /* Hmm, look downwards, until we find one that works */
                для (p--; p > 0; p--)
                        якщо (prctl_safe(PR_CAPBSET_READ, p, 0, 0, 0) >= 0)
                                break;

        } інакше {

                /* Hmm, look upwards, until we find one that doesn't work */
                для (; p < CAP_LIMIT; p++)
                        якщо (prctl_safe(PR_CAPBSET_READ, p+1, 0, 0, 0) < 0)
                                break;
        }

        c = (ціле) p;
        saved = c;
        повернути c;
}

ціле have_effective_cap(unsigned cap) {
        CapabilityQuintet q;
        ціле r;

        assert(cap <= CAP_LIMIT);

        r = capability_get(&q);
        якщо (r < 0)
                повернути r;

        повернути BIT_SET(q.effective, cap);
}

ціле have_inheritable_cap(unsigned cap) {
        CapabilityQuintet q;
        ціле r;

        assert(cap <= CAP_LIMIT);

        r = capability_get(&q);
        якщо (r < 0)
                повернути r;

        повернути BIT_SET(q.inheritable, cap);
}

ціле capability_ambient_set_apply(uint64_t set, bool also_inherit) {
        ціле r;

        /* Remove capabilities requested in ambient set, but not in the bounding set */
        для (unsigned i = 0; i <= cap_last_cap(); i++) {
                якщо (!BIT_SET(set, i))
                        continue;

                r = prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0);
                якщо (r < 0)
                        повернути r;
                якщо (r != 1) {
                        log_debug("Ambient capability %s requested but missing from bounding set, suppressing automatically.",
                                  capability_to_name(i));
                        CLEAR_BIT(set, i);
                }
        }

        /* Add the capabilities to the ambient set (an possibly also the inheritable set) */

        якщо (also_inherit) {
                CapabilityQuintet q;

                r = capability_get(&q);
                якщо (r < 0)
                        повернути r;

                q.inheritable = set;

                r = capability_apply(&q);
                якщо (r < 0)
                        повернути r;
        }

        для (unsigned i = 0; i <= cap_last_cap(); i++)
                якщо (BIT_SET(set, i)) {
                        /* Add the capability to the ambient set. */
                        r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, i, 0, 0);
                        якщо (r < 0)
                                повернути r;
                } інакше {
                        /* Drop the capability so we don't inherit capabilities we didn't ask for. */
                        r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                        якщо (r < 0)
                                повернути r;
                        якщо (r > 0) {
                                r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, i, 0, 0);
                                якщо (r < 0)
                                        повернути r;
                        }
                }

        повернути 0;
}

ціле capability_gain_cap_setpcap(void) {
        CapabilityQuintet q;
        ціле r;

        r = capability_get(&q);
        якщо (r < 0)
                повернути r;

        якщо (BIT_SET(q.effective, CAP_SETPCAP))
                повернути 1; /* We already have capability. */

        SET_BIT(q.effective, CAP_SETPCAP);

        r = capability_apply(&q);
        якщо (r < 0) {
                /* If we didn't manage to acquire the CAP_SETPCAP bit, we continue anyway, after all this
                 * just means we'll fail later, when we actually intend to drop some capabilities or try to
                 * set securebits. */
                log_debug_errno(r, "Can't acquire effective CAP_SETPCAP bit, ignoring: %m");
                повернути 0;
        }

        повернути 1; /* acquired */
}

ціле capability_bounding_set_drop(uint64_t keep, bool right_now) {
        ціле k, r;

        /* If we are run as PID 1 we will lack CAP_SETPCAP by default in the effective set (yes, the kernel
         * drops that when executing init!), so get it back temporarily so that we can call PR_CAPBSET_DROP. */

        CapabilityQuintet q;
        r = capability_get(&q);
        якщо (r < 0)
                повернути r;
        CapabilityQuintet saved = q;

        r = capability_gain_cap_setpcap();
        якщо (r < 0)
                повернути r;

        для (unsigned i = 0; i <= cap_last_cap(); i++) {
                якщо (BIT_SET(keep, i))
                        continue;

                /* Drop it from the bounding set */
                r = prctl_safe(PR_CAPBSET_DROP, i, 0, 0, 0);
                якщо (r < 0) {
                        /* If dropping the capability failed, let's see if we didn't have it in the first
                         * place. If so, continue anyway, as dropping a capability we didn't have in the
                         * first place doesn't really matter anyway. */
                        якщо (prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0) != 0)
                                goto finish;
                }

                /* Also drop it from the inheritable set, so that anything we exec() loses the capability for
                 * good. */
                CLEAR_BIT(q.inheritable, i);

                /* If we shall apply this right now drop it also from our own capability sets. */
                якщо (right_now) {
                        CLEAR_BIT(q.effective, i);
                        CLEAR_BIT(q.permitted, i);
                }
        }

        r = 0;

finish:
        k = capability_apply(&q);
        якщо (k < 0)
                /* If there are no actual changes anyway then let's ignore this error. */
                якщо (!capability_quintet_equal(&q, &saved))
                        повернути k;

        повернути r;
}

static ціле drop_from_file(const char *fn, uint64_t keep) {
        _cleanup_free_ char *p = NULL;
        uint64_t current, after;
        uint32_t hi, lo;
        ціле r, k;

        r = read_one_line_file(fn, &p);
        якщо (r < 0)
                повернути r;

        k = sscanf(p, "%" PRIu32 " %" PRIu32, &lo, &hi);
        якщо (k != 2)
                повернути -EIO;

        current = (uint64_t) lo | ((uint64_t) hi << 32);
        after = current & keep;

        якщо (current == after)
                повернути 0;

        lo = after & UINT32_MAX;
        hi = (after >> 32) & UINT32_MAX;

        повернути write_string_filef(fn, 0, "%" PRIu32 " %" PRIu32, lo, hi);
}

ціле capability_bounding_set_drop_usermode(uint64_t keep) {
        ціле r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/inheritable", keep);
        якщо (r < 0)
                повернути r;

        r = drop_from_file("/proc/sys/kernel/usermodehelper/bset", keep);
        якщо (r < 0)
                повернути r;

        повернути r;
}

ціле drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities) {
        ціле r;

        /* Unfortunately we cannot leave privilege dropping to PID 1 here, since we want to run as user but
         * want to keep some capabilities. Since file capabilities have been introduced this cannot be done
         * across exec() anymore, unless our binary has the capability configured in the file system, which
         * we want to avoid. */

        якщо (setresgid(gid, gid, gid) < 0)
                повернути log_error_errno(errno, "Failed to change group ID: %m");

        r = maybe_setgroups(/* size= */ 0, /* list= */ NULL);
        якщо (r < 0)
                повернути log_error_errno(r, "Failed to drop auxiliary groups list: %m");

        /* Ensure we keep the permitted caps across the setresuid(). Note that we do this even if we actually
         * don't want to keep any capabilities, since we want to be able to drop them from the bounding set
         * too, and we can only do that if we have capabilities. */
        r = prctl_safe(PR_SET_KEEPCAPS, 1, 0, 0, 0);
        якщо (r < 0)
                повернути log_error_errno(r, "Failed to enable keep capabilities flag: %m");

        якщо (setresuid(uid, uid, uid) < 0)
                повернути log_error_errno(errno, "Failed to change user ID: %m");

        r = prctl_safe(PR_SET_KEEPCAPS, 0, 0, 0, 0);
        якщо (r < 0)
                повернути log_error_errno(r, "Failed to disable keep capabilities flag: %m");

        /* Drop all caps from the bounding set (as well as the inheritable/permitted/effective sets), except
         * the ones we want to keep */
        r = capability_bounding_set_drop(keep_capabilities, /* right_now= */ true);
        якщо (r < 0)
                повернути log_error_errno(r, "Failed to drop capabilities: %m");

        /* Now upgrade the permitted caps we still kept to effective caps */
        якщо (keep_capabilities != 0) {
                CapabilityQuintet q = {
                        .effective = keep_capabilities,
                        .permitted = keep_capabilities,
                };

                r = capability_apply(&q);
                якщо (r < 0)
                        повернути log_error_errno(r, "Failed to increase capabilities: %m");
        }

        повернути 0;
}

static ціле change_capability(unsigned cap, bool b) {
        CapabilityQuintet q;
        ціле r;

        assert(cap <= CAP_LIMIT);

        r = capability_get(&q);
        якщо (r < 0)
                повернути r;

        якщо (b) {
                SET_BIT(q.effective, cap);
                SET_BIT(q.permitted, cap);
                SET_BIT(q.inheritable, cap);
        } інакше {
                CLEAR_BIT(q.effective, cap);
                CLEAR_BIT(q.permitted, cap);
                CLEAR_BIT(q.inheritable, cap);
        }

        повернути capability_apply(&q);
}

ціле drop_capability(unsigned cap) {
        повернути change_capability(cap, false);
}

ціле keep_capability(unsigned cap) {
        повернути change_capability(cap, true);
}

bool capability_quintet_mangle(CapabilityQuintet *q) {
        uint64_t combined, drop = 0;

        assert(q);

        combined = q->effective | q->bounding | q->inheritable | q->permitted | q->ambient;

        для (unsigned i = 0; i <= cap_last_cap(); i++) {
                якщо (!BIT_SET(combined, i))
                        continue;

                якщо (prctl_safe(PR_CAPBSET_READ, i, 0, 0, 0) > 0)
                        continue;

                SET_BIT(drop, i);

                log_debug("Dropping capability not in the current bounding set: %s", capability_to_name(i));
        }

        q->effective &= ~drop;
        q->bounding &= ~drop;
        q->inheritable &= ~drop;
        q->permitted &= ~drop;
        q->ambient &= ~drop;

        повернути drop != 0; /* Let the caller know we changed something */
}

ціле capability_quintet_enforce(const CapabilityQuintet *q) {
        CapabilityQuintet c;
        bool modified = false;
        ціле r;

        assert(q);

        якщо (q->ambient != CAP_MASK_UNSET ||
            q->inheritable != CAP_MASK_UNSET ||
            q->permitted != CAP_MASK_UNSET ||
            q->effective != CAP_MASK_UNSET) {
                r = capability_get(&c);
                якщо (r < 0)
                        повернути r;
        }

        якщо (q->ambient != CAP_MASK_UNSET) {
                /* In order to raise the ambient caps set we first need to raise the matching
                 * inheritable + permitted cap */
                якщо (!FLAGS_SET(c.permitted, q->ambient) ||
                    !FLAGS_SET(c.inheritable, q->ambient)) {

                        c.permitted |= q->ambient;
                        c.inheritable |= q->ambient;

                        r = capability_apply(&c);
                        якщо (r < 0)
                                повернути r;
                }

                r = capability_ambient_set_apply(q->ambient, /* also_inherit= */ false);
                якщо (r < 0)
                        повернути r;
        }

        якщо (q->inheritable != CAP_MASK_UNSET || q->permitted != CAP_MASK_UNSET || q->effective != CAP_MASK_UNSET) {
                якщо (!FLAGS_SET(c.effective, q->effective) ||
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
                        якщо (q->bounding != CAP_MASK_UNSET &&
                            (!BIT_SET(c.effective, CAP_SETPCAP) || !BIT_SET(c.permitted, CAP_SETPCAP))) {
                                CapabilityQuintet tmp = c;

                                SET_BIT(c.effective, CAP_SETPCAP);
                                SET_BIT(c.permitted, CAP_SETPCAP);

                                modified = true;

                                r = capability_apply(&tmp);
                        } інакше
                                r = capability_apply(&c);
                        якщо (r < 0)
                                повернути r;
                }
        }

        якщо (q->bounding != CAP_MASK_UNSET) {
                r = capability_bounding_set_drop(q->bounding, /* right_now= */ false);
                якщо (r < 0)
                        повернути r;
        }

        /* If needed, let's now set the caps again, this time in the final version, which differs from what
         * we have already set only in the CAP_SETPCAP bit, which we needed for dropping the bounding
         * bits. This call only undoes bits and doesn't acquire any which means the bounding caps don't
         * matter. */
        якщо (modified) {
                r = capability_apply(&c);
                якщо (r < 0)
                        повернути r;
        }

        повернути 0;
}

ціле capability_get_ambient(uint64_t *ret) {
        uint64_t a = 0;
        ціле r;

        assert(ret);

        для (unsigned i = 0; i <= cap_last_cap(); i++) {
                r = prctl_safe(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, i, 0, 0);
                якщо (r < 0)
                        повернути r;
                якщо (r > 0)
                        SET_BIT(a, i);
        }

        *ret = a;
        повернути 1;
}

ціле pidref_get_capability(const PidRef *pidref, CapabilityQuintet *ret) {
        ціле r;

        якщо (!pidref_is_set(pidref))
                повернути -ESRCH;
        якщо (pidref_is_remote(pidref))
                повернути -EREMOTE;

        const char *path = procfs_file_alloca(pidref->pid, "status");
        _cleanup_fclose_ FILE *f = fopen(path, "re");
        якщо (!f) {
                якщо (errno == ENOENT && proc_mounted() == 0)
                        повернути -ENOSYS;

                повернути -errno;
        }

        CapabilityQuintet q = CAPABILITY_QUINTET_NULL;
        для (;;) {
                _cleanup_free_ char *line = NULL;

                r = read_line(f, LONG_LINE_MAX, &line);
                якщо (r < 0)
                        повернути r;
                якщо (r == 0)
                        break;

                static const struct {
                        const char *field;
                        size_t offset;
                } fields[] = {
                        { "CapBnd:", offsetof(CapabilityQuintet, bounding)    },
                        { "CapInh:", offsetof(CapabilityQuintet, inheritable) },
                        { "CapPrm:", offsetof(CapabilityQuintet, permitted)   },
                        { "CapEff:", offsetof(CapabilityQuintet, effective)   },
                        { "CapAmb:", offsetof(CapabilityQuintet, ambient)     },
                };

                FOREACH_ELEMENT(i, fields) {

                        const char *p = first_word(line, i->field);
                        якщо (!p)
                                continue;

                        uint64_t *v = (uint64_t*) ((uint8_t*) &q + i->offset);

                        якщо (*v != CAP_MASK_UNSET)
                                повернути -EBADMSG;

                        r = safe_atoux64(p, v);
                        якщо (r < 0)
                                повернути r;

                        якщо (*v == CAP_MASK_UNSET)
                                повернути -EBADMSG;
                }
        }

        якщо (!capability_quintet_is_fully_set(&q))
                повернути -EBADMSG;

        r = pidref_verify(pidref);
        якщо (r < 0)
                повернути r;

        якщо (ret)
                *ret = q;

        повернути 0;
}

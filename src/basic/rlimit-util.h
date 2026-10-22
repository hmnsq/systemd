/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/resource.h>       /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(rlimit, ਪੂਰਨ);
ਪੂਰਨ rlimit_from_string_harder(ਅਚਲ ਅੱਖਰ *s) ਸ਼ੁੱਧ;
ਖਾਲੀ rlimits_list(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ);

ਪੂਰਨ setrlimit_closest(ਪੂਰਨ resource, ਅਚਲ ਬਣਤਰ rlimit *rlim);
ਪੂਰਨ setrlimit_closest_all(ਅਚਲ ਬਣਤਰ rlimit * ਅਚਲ *rlim, ਪੂਰਨ *which_failed);

ਪੂਰਨ rlimit_parse_one(ਪੂਰਨ resource, ਅਚਲ ਅੱਖਰ *val, rlim_t *ret);
ਪੂਰਨ rlimit_parse(ਪੂਰਨ resource, ਅਚਲ ਅੱਖਰ *val, ਬਣਤਰ rlimit *ret);

ਪੂਰਨ rlimit_format(ਅਚਲ ਬਣਤਰ rlimit *rl, ਅੱਖਰ **ret);

ਪੂਰਨ rlimit_copy_all(ਬਣਤਰ rlimit* ਨਿਸ਼ਾਨਾ[ਸਥਿਰ _RLIMIT_MAX], ਬਣਤਰ rlimit* ਅਚਲ ਸਰੋਤ[ਸਥਿਰ _RLIMIT_MAX]);
ਖਾਲੀ rlimit_free_all(ਬਣਤਰ rlimit **rl);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RLIMIT_MAKE_CONST(lim) ((ਬਣਤਰ rlimit) { lim, lim })

ਪੂਰਨ rlimit_nofile_bump(ਪੂਰਨ limit);
ਪੂਰਨ rlimit_nofile_ਸੁਰੱਖਿਅਤ(ਖਾਲੀ);

ਪੂਰਨ pid_getrlimit(pid_t pid, ਪੂਰਨ resource, ਬਣਤਰ rlimit *ret);

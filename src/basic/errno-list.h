/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਅਚਲ ਅੱਖਰ* errno_name_no_fallback(ਪੂਰਨ id) _const_;
ਪੂਰਨ errno_from_name(ਅਚਲ ਅੱਖਰ *name) ਸ਼ੁੱਧ;

ਸਥਿਰ ਸਤਰਅੰਦਰ bool errno_is_valid(ਪੂਰਨ n) {
        ਵਾਪਸ n > 0 && n <= ERRNO_MAX;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(ਪੂਰਨ)
/* Like errno_name, but always returns a string. */
ਅਚਲ ਅੱਖਰ* errno_name(ਪੂਰਨ id, ਅੱਖਰ buf[ਸਥਿਰ ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERRNO_NAME(errnum) errno_name(errnum, (ਅੱਖਰ[ERRNO_NAME_BUF_LEN]){})

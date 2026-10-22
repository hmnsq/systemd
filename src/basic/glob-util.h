/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <glob.h>       /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ DIR* (*opendir_t)(ਅਚਲ ਅੱਖਰ *);

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_glob_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags, opendir_t opendir_func, ਅੱਖਰ ***ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_glob(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags, ਅੱਖਰ ***ret) {
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_glob_ਭਰਿਆ(ਰਸਤਾ, flags, ਨਿਰਾ, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
ਪੂਰਨ glob_ਪਹਿਲਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ glob_exists(ਰਸਤਾ) glob_ਪਹਿਲਾ(ਰਸਤਾ, ਨਿਰਾ)
ਪੂਰਨ glob_extend(ਅੱਖਰ ***strv, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags);

ਪੂਰਨ glob_non_glob_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret);

bool string_is_glob(ਅਚਲ ਅੱਖਰ *p) ਸ਼ੁੱਧ;

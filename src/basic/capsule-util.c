/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "capsule-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user-util.h"

ਪੂਰਨ capsule_name_is_valid(ਅਚਲ ਅੱਖਰ *name) {

        ਜੇ (!filename_is_valid(name))
                ਵਾਪਸ false;

        _cleanup_free_ ਅੱਖਰ *prefixed = strjoin("c-", name);
        ਜੇ (!prefixed)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ valid_user_ਸਮੂਹ_name(prefixed, /* flags= */ 0);
}

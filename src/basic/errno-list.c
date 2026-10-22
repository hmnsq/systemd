/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "errno-list.h"

ਸਥਿਰ ਅਚਲ ਬਣਤਰ errno_name* lookup_errno(ਰਜਿਸਟਰ ਅਚਲ ਅੱਖਰ *str,
                                             ਰਜਿਸਟਰ GPERF_LEN_TYPE len);

#ਅਨ੍ਤਰ੍ਭਾਵ "errno-from-name.inc"

ਪੂਰਨ errno_from_name(ਅਚਲ ਅੱਖਰ *name) {
        ਅਚਲ ਬਣਤਰ errno_name *sc;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        sc = lookup_errno(name, ਲੜੀ_ਲੰਬਾਈ(name));
        ਜੇ (!sc)
                ਵਾਪਸ -EINVAL;

        ਪ੍ਰਤਿਜ੍ਞਾ(sc->id > 0);
        ਵਾਪਸ sc->id;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GLIBC__
ਅਚਲ ਅੱਖਰ* errno_name_no_fallback(ਪੂਰਨ id) {
        ਜੇ (id == 0) /* To stay in line with our implementation below.  */
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ strerrorname_np(ABS(id));
}
#ਨਹੀਂਤਾਂ
#  include "errno-to-name.inc"

ਅਚਲ ਅੱਖਰ* errno_name_no_fallback(ਪੂਰਨ id) {
        ਜੇ (id < 0)
                id = -id;

        ਜੇ ((size_t) id >= ELEMENTSOF(errno_ਨਾਂ))
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ errno_ਨਾਂ[id];
}
#ਜੇ_ਅੰਤ

ਅਚਲ ਅੱਖਰ* errno_name(ਪੂਰਨ id, ਅੱਖਰ buf[ਸਥਿਰ ERRNO_NAME_BUF_LEN]) {
        ਅਚਲ ਅੱਖਰ *a = errno_name_no_fallback(id);
        ਜੇ (a)
                ਵਾਪਸ a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", ਨਿਰਪੇਖ(id));
        ਵਾਪਸ buf;
}

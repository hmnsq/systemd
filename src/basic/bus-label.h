/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਅੱਖਰ* bus_label_escape(ਅਚਲ ਅੱਖਰ *s);
ਅੱਖਰ* bus_label_unescape_n(ਅਚਲ ਅੱਖਰ *f, size_t l);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* bus_label_unescape(ਅਚਲ ਅੱਖਰ *f) {
        ਵਾਪਸ bus_label_unescape_n(f, SIZE_MAX);
}

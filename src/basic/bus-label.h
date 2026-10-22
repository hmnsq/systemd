/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

حرف* bus_label_escape(ثابت حرف *s);
حرف* bus_label_unescape_n(ثابت حرف *f, size_t l);

ساکن اندرونی حرف* bus_label_unescape(ثابت حرف *f) {
        واپس bus_label_unescape_n(f, SIZE_MAX);
}

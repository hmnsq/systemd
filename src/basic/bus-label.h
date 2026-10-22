/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

حرف* bus_label_escape(ثابت حرف *s);
حرف* bus_label_unescape_n(ثابت حرف *f, size_t l);

ساكن مضمن حرف* bus_label_unescape(ثابت حرف *f) {
        أرجع bus_label_unescape_n(f, SIZE_MAX);
}

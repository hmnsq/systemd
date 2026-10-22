/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

எழுத்து* bus_label_escape(மாறிலி எழுத்து *s);
எழுத்து* bus_label_unescape_n(மாறிலி எழுத்து *f, size_t l);

நிலையான உள்வரி எழுத்து* bus_label_unescape(மாறிலி எழுத்து *f) {
        திரும்பு bus_label_unescape_n(f, SIZE_MAX);
}

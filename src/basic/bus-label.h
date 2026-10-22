/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

वर्ण* bus_label_escape(ध्रुव वर्ण *s);
वर्ण* bus_label_unescape_n(ध्रुव वर्ण *f, size_t l);

स्थिर अन्तःस्थ वर्ण* bus_label_unescape(ध्रुव वर्ण *f) {
        प्रत्यागम bus_label_unescape_n(f, SIZE_MAX);
}

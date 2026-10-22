/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

ৱর্ণ* bus_label_escape(ধ্রুৱ ৱর্ণ *s);
ৱর্ণ* bus_label_unescape_n(ধ্রুৱ ৱর্ণ *f, size_t l);

স্থির অন্তঃস্থ ৱর্ণ* bus_label_unescape(ধ্রুৱ ৱর্ণ *f) {
        ঘূৰাই bus_label_unescape_n(f, SIZE_MAX);
}

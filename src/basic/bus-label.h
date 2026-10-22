/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

বর্ণ* bus_label_escape(ধ্রুব বর্ণ *s);
বর্ণ* bus_label_unescape_n(ধ্রুব বর্ণ *f, size_t l);

স্থির অন্তঃস্থ বর্ণ* bus_label_unescape(ধ্রুব বর্ণ *f) {
        প্রত্যাগম bus_label_unescape_n(f, SIZE_MAX);
}

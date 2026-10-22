/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

অক্ষর* bus_label_escape(ধ্রুবক অক্ষর *s);
অক্ষর* bus_label_unescape_n(ধ্রুবক অক্ষর *f, size_t l);

স্থির অন্তঃরেখ অক্ষর* bus_label_unescape(ধ্রুবক অক্ষর *f) {
        ফেরত bus_label_unescape_n(f, SIZE_MAX);
}

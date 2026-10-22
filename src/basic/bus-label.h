/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

അക്ഷരം* bus_label_escape(സ്ഥിരം അക്ഷരം *s);
അക്ഷരം* bus_label_unescape_n(സ്ഥിരം അക്ഷരം *f, size_t l);

സ്ഥിതം വരിയിൽ അക്ഷരം* bus_label_unescape(സ്ഥിരം അക്ഷരം *f) {
        തിരികെ bus_label_unescape_n(f, SIZE_MAX);
}

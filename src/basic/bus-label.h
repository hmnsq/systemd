/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ವರ್ಣ* bus_label_escape(ಧ್ರುವ ವರ್ಣ *s);
ವರ್ಣ* bus_label_unescape_n(ಧ್ರುವ ವರ್ಣ *f, size_t l);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* bus_label_unescape(ಧ್ರುವ ವರ್ಣ *f) {
        ಪ್ರತ್ಯಾಗಮ bus_label_unescape_n(f, SIZE_MAX);
}

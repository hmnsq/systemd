/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ଅକ୍ଷର* bus_label_escape(ଅଚଳ ଅକ୍ଷର *s);
ଅକ୍ଷର* bus_label_unescape_n(ଅଚଳ ଅକ୍ଷର *f, size_t l);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* bus_label_unescape(ଅଚଳ ଅକ୍ଷର *f) {
        ଫେରନ୍ତୁ bus_label_unescape_n(f, SIZE_MAX);
}

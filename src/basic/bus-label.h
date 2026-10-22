/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

var_na* bus_label_escape(const var_na *s);
var_na* bus_label_unescape_n(const var_na *f, size_t l);

static anta_hstha var_na* bus_label_unescape(const var_na *f) {
        ᨀᨘᨒ bus_label_unescape_n(f, SIZE_MAX);
}

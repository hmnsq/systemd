/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

अक्षर* bus_label_escape(स्थिरांक अक्षर *s);
अक्षर* bus_label_unescape_n(स्थिरांक अक्षर *f, माप_प्रकार l);

स्थिर ओळीत अक्षर* bus_label_unescape(स्थिरांक अक्षर *f) {
        परत bus_label_unescape_n(f, SIZE_MAX);
}

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

अक्षर* bus_label_escape(स्थिर अक्षर *s);
अक्षर* bus_label_unescape_n(स्थिर अक्षर *f, माप_प्रकार l);

अटल अंतरभूत अक्षर* bus_label_unescape(स्थिर अक्षर *f) {
        वापस bus_label_unescape_n(f, SIZE_MAX);
}

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

نویسه* bus_label_escape(ثابت نویسه *s);
نویسه* bus_label_unescape_n(ثابت نویسه *f, size_t l);

ایستا درونخطی نویسه* bus_label_unescape(ثابت نویسه *f) {
        بازگشت bus_label_unescape_n(f, SIZE_MAX);
}

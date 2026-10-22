/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

અક્ષર* bus_label_escape(અચલ અક્ષર *s);
અક્ષર* bus_label_unescape_n(અચલ અક્ષર *f, size_t l);

સ્થિર અંતઃરેખ અક્ષર* bus_label_unescape(અચલ અક્ષર *f) {
        પાછા bus_label_unescape_n(f, SIZE_MAX);
}

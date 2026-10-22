/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પૂર્ણાંક encode_devnode_name(અચલ અક્ષર *str, અક્ષર *str_enc, size_t len);
પૂર્ણાંક allow_listed_char_for_devnode(અક્ષર c, અચલ અક્ષર *additional);

પૂર્ણાંક devnode_same(અચલ અક્ષર *a, અચલ અક્ષર *b);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ကိန်းပြည့် encode_devnode_name(const var_na *str, var_na *str_enc, size_t len);
ကိန်းပြည့် allow_listed_char_for_devnode(var_na c, const var_na *additional);

ကိန်းပြည့် devnode_same(const var_na *a, const var_na *b);

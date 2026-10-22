/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

පූර්ණ_සංඛ්‍යා encode_devnode_name(const var_na *str, var_na *str_enc, size_t len);
පූර්ණ_සංඛ්‍යා allow_listed_char_for_devnode(var_na c, const var_na *additional);

පූර්ණ_සංඛ්‍යා devnode_same(const var_na *a, const var_na *b);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

số_nguyên encode_devnode_name(const char *str, char *str_enc, size_t len);
số_nguyên allow_listed_char_for_devnode(char c, const char *additional);

số_nguyên devnode_same(const char *a, const char *b);

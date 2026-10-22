/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

整数 encode_devnode_name(const char *str, char *str_enc, size_t len);
整数 allow_listed_char_for_devnode(char c, const char *additional);

整数 devnode_same(const char *a, const char *b);

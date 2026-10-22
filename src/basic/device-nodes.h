/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

цео encode_devnode_name(const char *str, char *str_enc, size_t len);
цео allow_listed_char_for_devnode(char c, const char *additional);

цео devnode_same(const char *a, const char *b);

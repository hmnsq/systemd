/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

цяло encode_devnode_name(const char *str, char *str_enc, size_t len);
цяло allow_listed_char_for_devnode(char c, const char *additional);

цяло devnode_same(const char *a, const char *b);

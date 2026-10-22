/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

পূর্ণাঙ্ক encode_devnode_name(ধ্রুব বর্ণ *str, বর্ণ *str_enc, size_t len);
পূর্ণাঙ্ক allow_listed_char_for_devnode(বর্ণ c, ধ্রুব বর্ণ *additional);

পূর্ণাঙ্ক devnode_same(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b);

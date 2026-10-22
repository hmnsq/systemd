/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

পূর্ণসংখ্যা encode_devnode_name(ধ্রুবক অক্ষর *str, অক্ষর *str_enc, size_t len);
পূর্ণসংখ্যা allow_listed_char_for_devnode(অক্ষর c, ধ্রুবক অক্ষর *additional);

পূর্ণসংখ্যা devnode_same(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b);

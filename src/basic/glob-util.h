/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <glob.h>       /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

প্রকারনির্দেশ DIR* (*opendir_t)(ধ্রুব বর্ণ *);

পূর্ণাঙ্ক safe_glob_full(ধ্রুব বর্ণ *path, পূর্ণাঙ্ক flags, opendir_t opendir_func, বর্ণ ***ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_glob(ধ্রুব বর্ণ *path, পূর্ণাঙ্ক flags, বর্ণ ***ret) {
        প্রত্যাগম safe_glob_full(path, flags, রিক্ত, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
পূর্ণাঙ্ক glob_first(ধ্রুব বর্ণ *path, বর্ণ **ret);
#define glob_exists(path) glob_first(path, রিক্ত)
পূর্ণাঙ্ক glob_extend(বর্ণ ***strv, ধ্রুব বর্ণ *path, পূর্ণাঙ্ক flags);

পূর্ণাঙ্ক glob_non_glob_prefix(ধ্রুব বর্ণ *path, বর্ণ **ret);

bool string_is_glob(ধ্রুব বর্ণ *p) _pure_;

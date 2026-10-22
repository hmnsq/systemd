/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <glob.h>       /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

প্রকারলেখ DIR* (*opendir_t)(ধ্রুবক অক্ষর *);

পূর্ণসংখ্যা safe_glob_full(ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags, opendir_t opendir_func, অক্ষর ***ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_glob(ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags, অক্ষর ***ret) {
        ফেরত safe_glob_full(path, flags, ফাঁকা, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
পূর্ণসংখ্যা glob_first(ধ্রুবক অক্ষর *path, অক্ষর **ret);
#define glob_exists(path) glob_first(path, ফাঁকা)
পূর্ণসংখ্যা glob_extend(অক্ষর ***strv, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags);

পূর্ণসংখ্যা glob_non_glob_prefix(ধ্রুবক অক্ষর *path, অক্ষর **ret);

bool string_is_glob(ধ্রুবক অক্ষর *p) _pure_;

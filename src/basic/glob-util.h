/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ <glob.h>       /* IWYU pragma: export */

#অন্তর্ভাৱ "forward.h"

প্রকারনির্দেশ DIR* (*opendir_t)(ধ্রুৱ ৱর্ণ *);

পূর্ণাঙ্ক safe_glob_full(ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags, opendir_t opendir_func, ৱর্ণ ***ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_glob(ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags, ৱর্ণ ***ret) {
        ঘূৰাই safe_glob_full(path, flags, রিক্ত, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
পূর্ণাঙ্ক glob_first(ধ্রুৱ ৱর্ণ *path, ৱর্ণ **ret);
#define glob_exists(path) glob_first(path, রিক্ত)
পূর্ণাঙ্ক glob_extend(ৱর্ণ ***strv, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags);

পূর্ণাঙ্ক glob_non_glob_prefix(ধ্রুৱ ৱর্ণ *path, ৱর্ণ **ret);

bool string_is_glob(ধ্রুৱ ৱর্ণ *p) _pure_;

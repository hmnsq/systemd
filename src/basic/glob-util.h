/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <glob.h>       /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା DIR* (*opendir_t)(ଅଚଳ ଅକ୍ଷର *);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_glob_full(ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, opendir_t opendir_func, ଅକ୍ଷର ***ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_glob(ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଅକ୍ଷର ***ret) {
        ଫେରନ୍ତୁ safe_glob_full(path, flags, ଖାଲି, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା glob_first(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret);
#define glob_exists(path) glob_first(path, ଖାଲି)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା glob_extend(ଅକ୍ଷର ***strv, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା glob_non_glob_prefix(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret);

bool string_is_glob(ଅଚଳ ଅକ୍ଷର *p) _pure_;

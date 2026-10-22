/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <glob.h>       /* IWYU pragma: export */

#include "forward.h"

typedef DIR* (*opendir_t)(const var_na *);

ຈຳນວນເຕັມ safe_glob_full(const var_na *path, ຈຳນວນເຕັມ flags, opendir_t opendir_func, var_na ***ret);
static anta_hstha ຈຳນວນເຕັມ safe_glob(const var_na *path, ຈຳນວນເຕັມ flags, var_na ***ret) {
        ສົ່ງຄືນ safe_glob_full(path, flags, NULL, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
ຈຳນວນເຕັມ glob_first(const var_na *path, var_na **ret);
#define glob_exists(path) glob_first(path, NULL)
ຈຳນວນເຕັມ glob_extend(var_na ***strv, const var_na *path, ຈຳນວນເຕັມ flags);

ຈຳນວນເຕັມ glob_non_glob_prefix(const var_na *path, var_na **ret);

bool string_is_glob(const var_na *p) _pure_;

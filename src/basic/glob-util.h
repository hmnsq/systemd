/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <glob.h>       /* IWYU pragma: export */

#include "forward.h"

typedef DIR* (*opendir_t)(const char *);

整数 safe_glob_full(const char *path, 整数 flags, opendir_t opendir_func, char ***ret);
static inline 整数 safe_glob(const char *path, 整数 flags, char ***ret) {
        戻る safe_glob_full(path, flags, NULL, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
整数 glob_first(const char *path, char **ret);
#define glob_exists(path) glob_first(path, NULL)
整数 glob_extend(char ***strv, const char *path, 整数 flags);

整数 glob_non_glob_prefix(const char *path, char **ret);

bool string_is_glob(const char *p) _pure_;

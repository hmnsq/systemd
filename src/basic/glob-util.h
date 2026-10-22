/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <glob.h>       /* IWYU pragma: export */

#include "forward.h"

typedef DIR* (*opendir_t)(const char *);

정수 safe_glob_full(const char *path, 정수 flags, opendir_t opendir_func, char ***ret);
static inline 정수 safe_glob(const char *path, 정수 flags, char ***ret) {
        반환 safe_glob_full(path, flags, NULL, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
정수 glob_first(const char *path, char **ret);
#define glob_exists(path) glob_first(path, NULL)
정수 glob_extend(char ***strv, const char *path, 정수 flags);

정수 glob_non_glob_prefix(const char *path, char **ret);

bool string_is_glob(const char *p) _pure_;

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <glob.h>       /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ DIR* (*opendir_t)(ಧ್ರುವ ವರ್ಣ *);

ಪೂರ್ಣಾಙ್ಕ safe_glob_full(ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags, opendir_t opendir_func, ವರ್ಣ ***ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_glob(ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags, ವರ್ಣ ***ret) {
        ಪ್ರತ್ಯಾಗಮ safe_glob_full(path, flags, ರಿಕ್ತ, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
ಪೂರ್ಣಾಙ್ಕ glob_first(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret);
#define glob_exists(path) glob_first(path, ರಿಕ್ತ)
ಪೂರ್ಣಾಙ್ಕ glob_extend(ವರ್ಣ ***strv, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags);

ಪೂರ್ಣಾಙ್ಕ glob_non_glob_prefix(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret);

bool string_is_glob(ಧ್ರುವ ವರ್ಣ *p) _pure_;

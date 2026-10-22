/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <glob.h>       /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

வகைவரை DIR* (*opendir_t)(மாறிலி எழுத்து *);

முழு safe_glob_full(மாறிலி எழுத்து *path, முழு flags, opendir_t opendir_func, எழுத்து ***ret);
நிலையான உள்வரி முழு safe_glob(மாறிலி எழுத்து *path, முழு flags, எழுத்து ***ret) {
        திரும்பு safe_glob_full(path, flags, வெற்று, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
முழு glob_first(மாறிலி எழுத்து *path, எழுத்து **ret);
#define glob_exists(path) glob_first(path, வெற்று)
முழு glob_extend(எழுத்து ***strv, மாறிலி எழுத்து *path, முழு flags);

முழு glob_non_glob_prefix(மாறிலி எழுத்து *path, எழுத்து **ret);

bool string_is_glob(மாறிலி எழுத்து *p) _pure_;

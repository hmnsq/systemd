/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <glob.h>       /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

प्रकारनिर्देश सूची* (*opendir_t)(अचर वर्ण *);

पूर्णाङ्क safe_glob_full(अचर वर्ण *path, पूर्णाङ्क flags, opendir_t opendir_func, वर्ण ***ret);
स्थिर अन्तःस्थ पूर्णाङ्क safe_glob(अचर वर्ण *path, पूर्णाङ्क flags, वर्ण ***ret) {
        फर्काऊ safe_glob_full(path, flags, रिक्त, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
पूर्णाङ्क glob_first(अचर वर्ण *path, वर्ण **ret);
#define glob_exists(path) glob_first(path, रिक्त)
पूर्णाङ्क glob_extend(वर्ण ***strv, अचर वर्ण *path, पूर्णाङ्क flags);

पूर्णाङ्क glob_non_glob_prefix(अचर वर्ण *path, वर्ण **ret);

bool string_is_glob(अचर वर्ण *p) _pure_;

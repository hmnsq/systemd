/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <glob.h>       /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

प्रकारनिर्देश सूची* (*opendir_t)(ध्रुव वर्ण *);

पूर्णाङ्क safe_glob_full(ध्रुव वर्ण *path, पूर्णाङ्क flags, opendir_t opendir_func, वर्ण ***ret);
स्थिर अन्तःस्थ पूर्णाङ्क safe_glob(ध्रुव वर्ण *path, पूर्णाङ्क flags, वर्ण ***ret) {
        प्रत्यागम safe_glob_full(path, flags, रिक्त, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
पूर्णाङ्क glob_first(ध्रुव वर्ण *path, वर्ण **ret);
#define glob_exists(path) glob_first(path, रिक्त)
पूर्णाङ्क glob_extend(वर्ण ***strv, ध्रुव वर्ण *path, पूर्णाङ्क flags);

पूर्णाङ्क glob_non_glob_prefix(ध्रुव वर्ण *path, वर्ण **ret);

bool string_is_glob(ध्रुव वर्ण *p) _pure_;

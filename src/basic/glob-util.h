/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <glob.h>       /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

प्रकारनिर्देश DIR* (*opendir_t)(ध्रुव वर्ण *);

पूर्णाङ्क safe_glob_पूर्णम्(ध्रुव वर्ण *मार्गः, पूर्णाङ्क flags, opendir_t opendir_func, वर्ण ***ret);
स्थिर अन्तःस्थ पूर्णाङ्क safe_glob(ध्रुव वर्ण *मार्गः, पूर्णाङ्क flags, वर्ण ***ret) {
        प्रत्यागम safe_glob_पूर्णम्(मार्गः, flags, रिक्त, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
पूर्णाङ्क glob_first(ध्रुव वर्ण *मार्गः, वर्ण **ret);
#परिभाषय glob_exists(मार्गः) glob_first(मार्गः, रिक्त)
पूर्णाङ्क glob_extend(वर्ण ***strv, ध्रुव वर्ण *मार्गः, पूर्णाङ्क flags);

पूर्णाङ्क glob_non_glob_उपसर्गः(ध्रुव वर्ण *मार्गः, वर्ण **ret);

bool string_is_glob(ध्रुव वर्ण *p) विशुद्धम्;

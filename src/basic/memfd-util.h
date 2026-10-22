/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/mman.h>           /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

पूर्णांक memfd_create_wrapper(स्थिरांक अक्षर *name, अचिन्हित mode);

पूर्णांक memfd_new_full(स्थिरांक अक्षर *name, अचिन्हित extra_flags);
स्थिर ओळीत पूर्णांक memfd_new(स्थिरांक अक्षर *name) {
        परत memfd_new_full(name, 0);
}

पूर्णांक memfd_new_and_seal(स्थिरांक अक्षर *name, स्थिरांक शून्य *data, माप_प्रकार sz) _nonnull_if_nonzero_(2, 3);
स्थिर ओळीत पूर्णांक memfd_new_and_seal_string(स्थिरांक अक्षर *name, स्थिरांक अक्षर *s) {
        परत memfd_new_and_seal(name, s, SIZE_MAX);
}

पूर्णांक memfd_set_sealed(पूर्णांक fd);
पूर्णांक memfd_get_sealed(पूर्णांक fd);
पूर्णांक memfd_set_sealed_gracefully(पूर्णांक fd);

पूर्णांक memfd_get_size(पूर्णांक fd, uint64_t *ret);
पूर्णांक memfd_set_size(पूर्णांक fd, uint64_t sz);

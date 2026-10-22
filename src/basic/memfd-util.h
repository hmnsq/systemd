/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <sys/mman.h>           /* IWYU pragma: export */

#समावेश "forward.h"

पूर्णांक memfd_create_wrapper(स्थिर अक्षर *name, अचिन्हित mode);

पूर्णांक memfd_new_full(स्थिर अक्षर *name, अचिन्हित extra_flags);
अटल अंतरभूत पूर्णांक memfd_new(स्थिर अक्षर *name) {
        वापस memfd_new_full(name, 0);
}

पूर्णांक memfd_new_and_seal(स्थिर अक्षर *name, स्थिर व्योम *तथ्य, माप_प्रकार sz) _nonnull_if_nonzero_(2, 3);
अटल अंतरभूत पूर्णांक memfd_new_and_seal_string(स्थिर अक्षर *name, स्थिर अक्षर *s) {
        वापस memfd_new_and_seal(name, s, SIZE_MAX);
}

पूर्णांक memfd_set_sealed(पूर्णांक fd);
पूर्णांक memfd_get_sealed(पूर्णांक fd);
पूर्णांक memfd_set_sealed_gracefully(पूर्णांक fd);

पूर्णांक memfd_get_आकार(पूर्णांक fd, uint64_t *ret);
पूर्णांक memfd_set_आकार(पूर्णांक fd, uint64_t sz);

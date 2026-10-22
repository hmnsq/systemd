/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/mman.h>           /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

पूर्णाङ्क memfd_create_wrapper(अचर वर्ण *name, अचिन्हित mode);

पूर्णाङ्क memfd_new_full(अचर वर्ण *name, अचिन्हित extra_flags);
स्थिर अन्तःस्थ पूर्णाङ्क memfd_new(अचर वर्ण *name) {
        फर्काऊ memfd_new_full(name, 0);
}

पूर्णाङ्क memfd_new_and_seal(अचर वर्ण *name, अचर शून्य *data, माप_प्रकार sz) _nonnull_if_nonzero_(2, 3);
स्थिर अन्तःस्थ पूर्णाङ्क memfd_new_and_seal_string(अचर वर्ण *name, अचर वर्ण *s) {
        फर्काऊ memfd_new_and_seal(name, s, SIZE_MAX);
}

पूर्णाङ्क memfd_set_sealed(पूर्णाङ्क fd);
पूर्णाङ्क memfd_get_sealed(पूर्णाङ्क fd);
पूर्णाङ्क memfd_set_sealed_gracefully(पूर्णाङ्क fd);

पूर्णाङ्क memfd_get_size(पूर्णाङ्क fd, uint64_t *ret);
पूर्णाङ्क memfd_set_size(पूर्णाङ्क fd, uint64_t sz);

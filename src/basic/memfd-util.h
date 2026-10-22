/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/mman.h>           /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

पूर्णाङ्क memfd_create_wrapper(ध्रुव वर्ण *name, unsigned mode);

पूर्णाङ्क memfd_new_full(ध्रुव वर्ण *name, unsigned extra_flags);
स्थिर अन्तःस्थ पूर्णाङ्क memfd_new(ध्रुव वर्ण *name) {
        पटिनिवत्त memfd_new_full(name, 0);
}

पूर्णाङ्क memfd_new_and_seal(ध्रुव वर्ण *name, ध्रुव शून्य *data, size_t sz) _nonnull_if_nonzero_(2, 3);
स्थिर अन्तःस्थ पूर्णाङ्क memfd_new_and_seal_string(ध्रुव वर्ण *name, ध्रुव वर्ण *s) {
        पटिनिवत्त memfd_new_and_seal(name, s, SIZE_MAX);
}

पूर्णाङ्क memfd_set_sealed(पूर्णाङ्क fd);
पूर्णाङ्क memfd_get_sealed(पूर्णाङ्क fd);
पूर्णाङ्क memfd_set_sealed_gracefully(पूर्णाङ्क fd);

पूर्णाङ्क memfd_get_size(पूर्णाङ्क fd, uint64_t *ret);
पूर्णाङ्क memfd_set_size(पूर्णाङ्क fd, uint64_t sz);

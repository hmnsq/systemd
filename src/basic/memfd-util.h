/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <sys/mman.h>           /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

पूर्णाङ्क memfd_create_wrapper(ध्रुव वर्ण *name, अचिह्नित बहुलकम्);

पूर्णाङ्क memfd_new_पूर्णम्(ध्रुव वर्ण *name, अचिह्नित extra_flags);
स्थिर अन्तःस्थ पूर्णाङ्क memfd_new(ध्रुव वर्ण *name) {
        प्रत्यागम memfd_new_पूर्णम्(name, 0);
}

पूर्णाङ्क memfd_new_and_seal(ध्रुव वर्ण *name, ध्रुव शून्य *दत्त, size_t sz) _nonnull_if_nonzero_(2, 3);
स्थिर अन्तःस्थ पूर्णाङ्क memfd_new_and_seal_string(ध्रुव वर्ण *name, ध्रुव वर्ण *s) {
        प्रत्यागम memfd_new_and_seal(name, s, SIZE_MAX);
}

पूर्णाङ्क memfd_समुच्चयः_sealed(पूर्णाङ्क fd);
पूर्णाङ्क memfd_get_sealed(पूर्णाङ्क fd);
पूर्णाङ्क memfd_समुच्चयः_sealed_gracefully(पूर्णाङ्क fd);

पूर्णाङ्क memfd_get_आकार(पूर्णाङ्क fd, uint64_t *ret);
पूर्णाङ्क memfd_समुच्चयः_आकार(पूर्णाङ्क fd, uint64_t sz);

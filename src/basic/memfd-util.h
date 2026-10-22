/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <sys/mman.h>           /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಙ್ಕ memfd_create_wrapper(ಧ್ರುವ ವರ್ಣ *name, unsigned mode);

ಪೂರ್ಣಾಙ್ಕ memfd_new_full(ಧ್ರುವ ವರ್ಣ *name, unsigned extra_flags);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ memfd_new(ಧ್ರುವ ವರ್ಣ *name) {
        ಪ್ರತ್ಯಾಗಮ memfd_new_full(name, 0);
}

ಪೂರ್ಣಾಙ್ಕ memfd_new_and_seal(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ಶೂನ್ಯ *data, size_t sz) _nonnull_if_nonzero_(2, 3);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ memfd_new_and_seal_string(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *s) {
        ಪ್ರತ್ಯಾಗಮ memfd_new_and_seal(name, s, SIZE_MAX);
}

ಪೂರ್ಣಾಙ್ಕ memfd_set_sealed(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ memfd_get_sealed(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ memfd_set_sealed_gracefully(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ memfd_get_size(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t *ret);
ಪೂರ್ಣಾಙ್ಕ memfd_set_size(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t sz);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <sys/mman.h>           /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಂಕ memfd_create_wrapper(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಚಿಹ್ನೆರಹಿತ ಬಹುಲಕ);

ಪೂರ್ಣಾಂಕ memfd_new_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಚಿಹ್ನೆರಹಿತ extra_flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ memfd_new(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಹಿಂತಿರುಗಿ memfd_new_ತುಂಬಿದ(name, 0);
}

ಪೂರ್ಣಾಂಕ memfd_new_and_seal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t sz) _nonnull_if_nonzero_(2, 3);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ memfd_new_and_seal_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಹಿಂತಿರುಗಿ memfd_new_and_seal(name, s, SIZE_MAX);
}

ಪೂರ್ಣಾಂಕ memfd_ಗಣ_sealed(ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ memfd_get_sealed(ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ memfd_ಗಣ_sealed_gracefully(ಪೂರ್ಣಾಂಕ fd);

ಪೂರ್ಣಾಂಕ memfd_get_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret);
ಪೂರ್ಣಾಂಕ memfd_ಗಣ_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ fd, uint64_t sz);

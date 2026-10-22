/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <glob.h>       /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ DIR* (*opendir_t)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_glob_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags, opendir_t opendir_func, ಅಕ್ಷರ ***ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_glob(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags, ಅಕ್ಷರ ***ret) {
        ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_glob_ತುಂಬಿದ(ಮಾರ್ಗ, flags, ಖಾಲಿ, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
ಪೂರ್ಣಾಂಕ glob_ಮೊದಲ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret);
#ವ್ಯಾಖ್ಯಾನಿಸು glob_exists(ಮಾರ್ಗ) glob_ಮೊದಲ(ಮಾರ್ಗ, ಖಾಲಿ)
ಪೂರ್ಣಾಂಕ glob_extend(ಅಕ್ಷರ ***strv, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags);

ಪೂರ್ಣಾಂಕ glob_non_glob_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret);

bool string_is_glob(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) ಶುದ್ಧ;

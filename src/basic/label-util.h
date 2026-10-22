/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ LabelOps {
        ಪೂರ್ಣಾಂಕ (*pre)(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, LabelContext *label_context);
        ಪೂರ್ಣಾಂಕ (*post)(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool created, LabelContext *label_context);
} LabelOps;

ಪೂರ್ಣಾಂಕ label_ops_ಗಣ(ಸ್ಥಿರಾಂಕ LabelOps *label_ops);
ಶೂನ್ಯ label_ops_reset(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ label_ops_pre(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, LabelContext *label_context);
ಪೂರ್ಣಾಂಕ label_ops_post(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool created, LabelContext *label_context);

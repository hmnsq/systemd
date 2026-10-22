/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "label-util.h"

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ LabelOps *label_ops = ಖಾಲಿ;

ಪೂರ್ಣಾಂಕ label_ops_ಗಣ(ಸ್ಥಿರಾಂಕ LabelOps *ops) {
        ಪ್ರತಿಜ್ಞಾ(ops);

        ಆದರೆ (label_ops)
                ಹಿಂತಿರುಗಿ -EBUSY;

        label_ops = ops;
        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ label_ops_reset(ಶೂನ್ಯ) {
        label_ops = ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ label_ops_pre(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, LabelContext *label_context) {
        ಆದರೆ (!label_ops || !label_ops->pre)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ label_ops->pre(dir_fd, ಮಾರ್ಗ, ಬಹುಲಕ, label_context);
}

ಪೂರ್ಣಾಂಕ label_ops_post(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool created, LabelContext *label_context) {
        ಆದರೆ (!label_ops || !label_ops->post)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ label_ops->post(dir_fd, ಮಾರ್ಗ, created, label_context);
}

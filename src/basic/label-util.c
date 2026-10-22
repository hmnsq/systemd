/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "label-util.h"

ಸ್ಥಿರ ಧ್ರುವ LabelOps *label_ops = ರಿಕ್ತ;

ಪೂರ್ಣಾಙ್ಕ label_ops_set(ಧ್ರುವ LabelOps *ops) {
        ಪ್ರತಿಜ್ಞಾ(ops);

        ಯದಿ (label_ops)
                ಪ್ರತ್ಯಾಗಮ -EBUSY;

        label_ops = ops;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ label_ops_reset(ಶೂನ್ಯ) {
        label_ops = ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ label_ops_pre(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, LabelContext *label_context) {
        ಯದಿ (!label_ops || !label_ops->pre)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ label_ops->pre(dir_fd, path, mode, label_context);
}

ಪೂರ್ಣಾಙ್ಕ label_ops_post(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, bool created, LabelContext *label_context) {
        ಯದಿ (!label_ops || !label_ops->post)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ label_ops->post(dir_fd, path, created, label_context);
}

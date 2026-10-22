/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "label-util.h"

static const LabelOps *label_ops = NULL;

පූර්ණ_සංඛ්‍යා label_ops_set(const LabelOps *ops) {
        assert(ops);

        නම් (label_ops)
                ආපසු -EBUSY;

        label_ops = ops;
        ආපසු 0;
}

void label_ops_reset(void) {
        label_ops = NULL;
}

පූර්ණ_සංඛ්‍යා label_ops_pre(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, mode_t mode, LabelContext *label_context) {
        නම් (!label_ops || !label_ops->pre)
                ආපසු 0;

        ආපසු label_ops->pre(dir_fd, path, mode, label_context);
}

පූර්ණ_සංඛ්‍යා label_ops_post(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, bool created, LabelContext *label_context) {
        නම් (!label_ops || !label_ops->post)
                ආපසු 0;

        ආපසු label_ops->post(dir_fd, path, created, label_context);
}

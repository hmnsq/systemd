/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "label-util.h"

static const LabelOps *label_ops = NULL;

целое label_ops_set(const LabelOps *ops) {
        assert(ops);

        если (label_ops)
                вернуть -EBUSY;

        label_ops = ops;
        вернуть 0;
}

void label_ops_reset(void) {
        label_ops = NULL;
}

целое label_ops_pre(целое dir_fd, const char *path, mode_t mode, LabelContext *label_context) {
        если (!label_ops || !label_ops->pre)
                вернуть 0;

        вернуть label_ops->pre(dir_fd, path, mode, label_context);
}

целое label_ops_post(целое dir_fd, const char *path, bool created, LabelContext *label_context) {
        если (!label_ops || !label_ops->post)
                вернуть 0;

        вернуть label_ops->post(dir_fd, path, created, label_context);
}

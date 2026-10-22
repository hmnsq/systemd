/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "label-util.h"

static const LabelOps *label_ops = NULL;

цео label_ops_set(const LabelOps *ops) {
        assert(ops);

        ако (label_ops)
                врати -EBUSY;

        label_ops = ops;
        врати 0;
}

void label_ops_reset(void) {
        label_ops = NULL;
}

цео label_ops_pre(цео dir_fd, const char *path, mode_t mode, LabelContext *label_context) {
        ако (!label_ops || !label_ops->pre)
                врати 0;

        врати label_ops->pre(dir_fd, path, mode, label_context);
}

цео label_ops_post(цео dir_fd, const char *path, bool created, LabelContext *label_context) {
        ако (!label_ops || !label_ops->post)
                врати 0;

        врати label_ops->post(dir_fd, path, created, label_context);
}

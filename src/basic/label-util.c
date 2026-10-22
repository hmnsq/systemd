/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "label-util.h"

static const LabelOps *label_ops = NULL;

整数 label_ops_set(const LabelOps *ops) {
        assert(ops);

        もし (label_ops)
                戻る -EBUSY;

        label_ops = ops;
        戻る 0;
}

void label_ops_reset(void) {
        label_ops = NULL;
}

整数 label_ops_pre(整数 dir_fd, const char *path, mode_t mode, LabelContext *label_context) {
        もし (!label_ops || !label_ops->pre)
                戻る 0;

        戻る label_ops->pre(dir_fd, path, mode, label_context);
}

整数 label_ops_post(整数 dir_fd, const char *path, bool created, LabelContext *label_context) {
        もし (!label_ops || !label_ops->post)
                戻る 0;

        戻る label_ops->post(dir_fd, path, created, label_context);
}

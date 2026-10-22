/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "label-util.h"

static const LabelOps *label_ops = NULL;

จำนวนเต็ม label_ops_set(const LabelOps *ops) {
        assert(ops);

        ถ้า (label_ops)
                คืน -EBUSY;

        label_ops = ops;
        คืน 0;
}

void label_ops_reset(void) {
        label_ops = NULL;
}

จำนวนเต็ม label_ops_pre(จำนวนเต็ม dir_fd, const var_na *path, mode_t mode, LabelContext *label_context) {
        ถ้า (!label_ops || !label_ops->pre)
                คืน 0;

        คืน label_ops->pre(dir_fd, path, mode, label_context);
}

จำนวนเต็ม label_ops_post(จำนวนเต็ม dir_fd, const var_na *path, bool created, LabelContext *label_context) {
        ถ้า (!label_ops || !label_ops->post)
                คืน 0;

        คืน label_ops->post(dir_fd, path, created, label_context);
}

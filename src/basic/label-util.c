/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "label-util.h"

static const LabelOps *label_ops = NULL;

ціле label_ops_set(const LabelOps *ops) {
        assert(ops);

        якщо (label_ops)
                повернути -EBUSY;

        label_ops = ops;
        повернути 0;
}

void label_ops_reset(void) {
        label_ops = NULL;
}

ціле label_ops_pre(ціле dir_fd, const char *path, mode_t mode, LabelContext *label_context) {
        якщо (!label_ops || !label_ops->pre)
                повернути 0;

        повернути label_ops->pre(dir_fd, path, mode, label_context);
}

ціле label_ops_post(ціле dir_fd, const char *path, bool created, LabelContext *label_context) {
        якщо (!label_ops || !label_ops->post)
                повернути 0;

        повернути label_ops->post(dir_fd, path, created, label_context);
}

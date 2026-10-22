/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "label-util.h"

static const LabelOps *label_ops = NULL;

цяло label_ops_set(const LabelOps *ops) {
        assert(ops);

        ако (label_ops)
                върни -EBUSY;

        label_ops = ops;
        върни 0;
}

void label_ops_reset(void) {
        label_ops = NULL;
}

цяло label_ops_pre(цяло dir_fd, const char *path, mode_t mode, LabelContext *label_context) {
        ако (!label_ops || !label_ops->pre)
                върни 0;

        върни label_ops->pre(dir_fd, path, mode, label_context);
}

цяло label_ops_post(цяло dir_fd, const char *path, bool created, LabelContext *label_context) {
        ако (!label_ops || !label_ops->post)
                върни 0;

        върни label_ops->post(dir_fd, path, created, label_context);
}

/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "label-util.h"

static const LabelOps *label_ops = NULL;

정수 label_ops_set(const LabelOps *ops) {
        assert(ops);

        만약 (label_ops)
                반환 -EBUSY;

        label_ops = ops;
        반환 0;
}

void label_ops_reset(void) {
        label_ops = NULL;
}

정수 label_ops_pre(정수 dir_fd, const char *path, mode_t mode, LabelContext *label_context) {
        만약 (!label_ops || !label_ops->pre)
                반환 0;

        반환 label_ops->pre(dir_fd, path, mode, label_context);
}

정수 label_ops_post(정수 dir_fd, const char *path, bool created, LabelContext *label_context) {
        만약 (!label_ops || !label_ops->post)
                반환 0;

        반환 label_ops->post(dir_fd, path, created, label_context);
}

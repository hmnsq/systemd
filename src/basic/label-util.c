/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "label-util.h"

static const LabelOps *label_ops = NULL;

số_nguyên label_ops_set(const LabelOps *ops) {
        assert(ops);

        nếu (label_ops)
                trả_về -EBUSY;

        label_ops = ops;
        trả_về 0;
}

void label_ops_reset(void) {
        label_ops = NULL;
}

số_nguyên label_ops_pre(số_nguyên dir_fd, const char *path, mode_t mode, LabelContext *label_context) {
        nếu (!label_ops || !label_ops->pre)
                trả_về 0;

        trả_về label_ops->pre(dir_fd, path, mode, label_context);
}

số_nguyên label_ops_post(số_nguyên dir_fd, const char *path, bool created, LabelContext *label_context) {
        nếu (!label_ops || !label_ops->post)
                trả_về 0;

        trả_về label_ops->post(dir_fd, path, created, label_context);
}

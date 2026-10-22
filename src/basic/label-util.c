/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "label-util.h"

static const LabelOps *label_ops = NULL;

ཧྲིལ label_ops_set(const LabelOps *ops) {
        assert(ops);

        གལ་སྲིད (label_ops)
                ལོག -EBUSY;

        label_ops = ops;
        ལོག 0;
}

void label_ops_reset(void) {
        label_ops = NULL;
}

ཧྲིལ label_ops_pre(ཧྲིལ dir_fd, const var_na *path, mode_t mode, LabelContext *label_context) {
        གལ་སྲིད (!label_ops || !label_ops->pre)
                ལོག 0;

        ལོག label_ops->pre(dir_fd, path, mode, label_context);
}

ཧྲིལ label_ops_post(ཧྲིལ dir_fd, const var_na *path, bool created, LabelContext *label_context) {
        གལ་སྲིད (!label_ops || !label_ops->post)
                ལོག 0;

        ལོག label_ops->post(dir_fd, path, created, label_context);
}

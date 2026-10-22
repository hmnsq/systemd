/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "label-util.h"

static const LabelOps *label_ops = NULL;

ཧྲིལ label_ops_set(const LabelOps *ops) {
        assert(ops);

        གལ་ཏེ (label_ops)
                ཕྱིར -EBUSY;

        label_ops = ops;
        ཕྱིར 0;
}

void label_ops_reset(void) {
        label_ops = NULL;
}

ཧྲིལ label_ops_pre(ཧྲིལ dir_fd, const var_na *path, mode_t mode, LabelContext *label_context) {
        གལ་ཏེ (!label_ops || !label_ops->pre)
                ཕྱིར 0;

        ཕྱིར label_ops->pre(dir_fd, path, mode, label_context);
}

ཧྲིལ label_ops_post(ཧྲིལ dir_fd, const var_na *path, bool created, LabelContext *label_context) {
        གལ་ཏེ (!label_ops || !label_ops->post)
                ཕྱིར 0;

        ཕྱིར label_ops->post(dir_fd, path, created, label_context);
}

/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "label-util.h"

static const LabelOps *label_ops = NULL;

ຈຳນວນເຕັມ label_ops_set(const LabelOps *ops) {
        assert(ops);

        ຖ້າ (label_ops)
                ສົ່ງຄືນ -EBUSY;

        label_ops = ops;
        ສົ່ງຄືນ 0;
}

void label_ops_reset(void) {
        label_ops = NULL;
}

ຈຳນວນເຕັມ label_ops_pre(ຈຳນວນເຕັມ dir_fd, const var_na *path, mode_t mode, LabelContext *label_context) {
        ຖ້າ (!label_ops || !label_ops->pre)
                ສົ່ງຄືນ 0;

        ສົ່ງຄືນ label_ops->pre(dir_fd, path, mode, label_context);
}

ຈຳນວນເຕັມ label_ops_post(ຈຳນວນເຕັມ dir_fd, const var_na *path, bool created, LabelContext *label_context) {
        ຖ້າ (!label_ops || !label_ops->post)
                ສົ່ງຄືນ 0;

        ສົ່ງຄືນ label_ops->post(dir_fd, path, created, label_context);
}

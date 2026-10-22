/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "label-util.h"

static const LabelOps *label_ops = NULL;

ကိန်းပြည့် label_ops_set(const LabelOps *ops) {
        assert(ops);

        အကယ်၍ (label_ops)
                ပြန်ပေး -EBUSY;

        label_ops = ops;
        ပြန်ပေး 0;
}

void label_ops_reset(void) {
        label_ops = NULL;
}

ကိန်းပြည့် label_ops_pre(ကိန်းပြည့် dir_fd, const var_na *path, mode_t mode, LabelContext *label_context) {
        အကယ်၍ (!label_ops || !label_ops->pre)
                ပြန်ပေး 0;

        ပြန်ပေး label_ops->pre(dir_fd, path, mode, label_context);
}

ကိန်းပြည့် label_ops_post(ကိန်းပြည့် dir_fd, const var_na *path, bool created, LabelContext *label_context) {
        အကယ်၍ (!label_ops || !label_ops->post)
                ပြန်ပေး 0;

        ပြန်ပေး label_ops->post(dir_fd, path, created, label_context);
}

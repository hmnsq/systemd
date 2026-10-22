/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "label-util.h"

ساكن ثابت LabelOps *label_ops = عدم;

صحيح label_ops_set(ثابت LabelOps *ops) {
        أكد(ops);

        إذا (label_ops)
                إرجاع -EBUSY;

        label_ops = ops;
        إرجاع 0;
}

فراغ label_ops_reset(فراغ) {
        label_ops = عدم;
}

صحيح label_ops_pre(صحيح dir_fd, ثابت حرف *path, mode_t mode, LabelContext *label_context) {
        إذا (!label_ops || !label_ops->pre)
                إرجاع 0;

        إرجاع label_ops->pre(dir_fd, path, mode, label_context);
}

صحيح label_ops_post(صحيح dir_fd, ثابت حرف *path, bool created, LabelContext *label_context) {
        إذا (!label_ops || !label_ops->post)
                إرجاع 0;

        إرجاع label_ops->post(dir_fd, path, created, label_context);
}

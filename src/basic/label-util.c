/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "label-util.h"

ایستا ثابت LabelOps *label_ops = پوچ;

صحیح label_ops_set(ثابت LabelOps *ops) {
        أكد(ops);

        اگر (label_ops)
                بازگشت -EBUSY;

        label_ops = ops;
        بازگشت 0;
}

تهی label_ops_reset(تهی) {
        label_ops = پوچ;
}

صحیح label_ops_pre(صحیح dir_fd, ثابت نویسه *path, mode_t mode, LabelContext *label_context) {
        اگر (!label_ops || !label_ops->pre)
                بازگشت 0;

        بازگشت label_ops->pre(dir_fd, path, mode, label_context);
}

صحیح label_ops_post(صحیح dir_fd, ثابت نویسه *path, bool created, LabelContext *label_context) {
        اگر (!label_ops || !label_ops->post)
                بازگشت 0;

        بازگشت label_ops->post(dir_fd, path, created, label_context);
}

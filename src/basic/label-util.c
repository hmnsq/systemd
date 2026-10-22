/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "label-util.h"

ساکن ثابت LabelOps *label_ops = تہی;

صحیح label_ops_سیٹ(ثابت LabelOps *ops) {
        أكد(ops);

        اگر (label_ops)
                واپس -EBUSY;

        label_ops = ops;
        واپس 0;
}

خالی label_ops_reset(خالی) {
        label_ops = تہی;
}

صحیح label_ops_pre(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال, LabelContext *label_context) {
        اگر (!label_ops || !label_ops->pre)
                واپس 0;

        واپس label_ops->pre(dir_fd, راستہ, منوال, label_context);
}

صحیح label_ops_post(صحیح dir_fd, ثابت حرف *راستہ, bool created, LabelContext *label_context) {
        اگر (!label_ops || !label_ops->post)
                واپس 0;

        واپس label_ops->post(dir_fd, راستہ, created, label_context);
}

/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "label-util.h"

ساكن ثابت LabelOps *label_ops = عدم;

صحيح label_ops_set(ثابت LabelOps *ops) {
        أكد(ops);

        𐤀𐤌 (label_ops)
                أرجع -EBUSY;

        label_ops = ops;
        أرجع 0;
}

فراغ label_ops_reset(فراغ) {
        label_ops = عدم;
}

صحيح label_ops_pre(صحيح dir_fd, ثابت حرف *path, mode_t mode, LabelContext *label_context) {
        𐤀𐤌 (!label_ops || !label_ops->pre)
                أرجع 0;

        أرجع label_ops->pre(dir_fd, path, mode, label_context);
}

صحيح label_ops_post(صحيح dir_fd, ثابت حرف *path, bool created, LabelContext *label_context) {
        𐤀𐤌 (!label_ops || !label_ops->post)
                أرجع 0;

        أرجع label_ops->post(dir_fd, path, created, label_context);
}

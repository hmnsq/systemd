/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "label-util.h"

স্থির ধ্রুৱ LabelOps *label_ops = রিক্ত;

পূর্ণাঙ্ক label_ops_set(ধ্রুৱ LabelOps *ops) {
        প্রতিজ্ঞা(ops);

        যদি (label_ops)
                ঘূৰাই -EBUSY;

        label_ops = ops;
        ঘূৰাই 0;
}

শূন্য label_ops_reset(শূন্য) {
        label_ops = রিক্ত;
}

পূর্ণাঙ্ক label_ops_pre(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, mode_t mode, LabelContext *label_context) {
        যদি (!label_ops || !label_ops->pre)
                ঘূৰাই 0;

        ঘূৰাই label_ops->pre(dir_fd, path, mode, label_context);
}

পূর্ণাঙ্ক label_ops_post(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, bool created, LabelContext *label_context) {
        যদি (!label_ops || !label_ops->post)
                ঘূৰাই 0;

        ঘূৰাই label_ops->post(dir_fd, path, created, label_context);
}

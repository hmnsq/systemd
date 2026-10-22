/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "label-util.h"

স্থির ধ্রুব LabelOps *label_ops = রিক্ত;

পূর্ণাঙ্ক label_ops_set(ধ্রুব LabelOps *ops) {
        প্রতিজ্ঞা(ops);

        যদি (label_ops)
                প্রত্যাগম -EBUSY;

        label_ops = ops;
        প্রত্যাগম 0;
}

শূন্য label_ops_reset(শূন্য) {
        label_ops = রিক্ত;
}

পূর্ণাঙ্ক label_ops_pre(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, mode_t mode, LabelContext *label_context) {
        যদি (!label_ops || !label_ops->pre)
                প্রত্যাগম 0;

        প্রত্যাগম label_ops->pre(dir_fd, path, mode, label_context);
}

পূর্ণাঙ্ক label_ops_post(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, bool created, LabelContext *label_context) {
        যদি (!label_ops || !label_ops->post)
                প্রত্যাগম 0;

        প্রত্যাগম label_ops->post(dir_fd, path, created, label_context);
}

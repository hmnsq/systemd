/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "label-util.h"

স্থির ধ্রুবক LabelOps *label_ops = ফাঁকা;

পূর্ণসংখ্যা label_ops_set(ধ্রুবক LabelOps *ops) {
        প্রতিজ্ঞা(ops);

        যদি (label_ops)
                ফেরত -EBUSY;

        label_ops = ops;
        ফেরত 0;
}

শূন্য label_ops_reset(শূন্য) {
        label_ops = ফাঁকা;
}

পূর্ণসংখ্যা label_ops_pre(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, mode_t mode, LabelContext *label_context) {
        যদি (!label_ops || !label_ops->pre)
                ফেরত 0;

        ফেরত label_ops->pre(dir_fd, path, mode, label_context);
}

পূর্ণসংখ্যা label_ops_post(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, bool created, LabelContext *label_context) {
        যদি (!label_ops || !label_ops->post)
                ফেরত 0;

        ফেরত label_ops->post(dir_fd, path, created, label_context);
}

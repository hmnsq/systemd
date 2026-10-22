/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "label-util.h"

ସ୍ଥିର ଅଚଳ LabelOps *label_ops = ଖାଲି;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା label_ops_set(ଅଚଳ LabelOps *ops) {
        ପ୍ରତିଜ୍ଞା(ops);

        ଯଦି (label_ops)
                ଫେରନ୍ତୁ -EBUSY;

        label_ops = ops;
        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ label_ops_reset(ଶୂନ୍ୟ) {
        label_ops = ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା label_ops_pre(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, LabelContext *label_context) {
        ଯଦି (!label_ops || !label_ops->pre)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ label_ops->pre(dir_fd, path, mode, label_context);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା label_ops_post(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, bool created, LabelContext *label_context) {
        ଯଦି (!label_ops || !label_ops->post)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ label_ops->post(dir_fd, path, created, label_context);
}

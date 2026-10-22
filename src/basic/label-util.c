/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "label-util.h"

स्थिर अचर LabelOps *label_ops = रिक्त;

पूर्णाङ्क label_ops_set(अचर LabelOps *ops) {
        प्रतिज्ञा(ops);

        यदि (label_ops)
                फर्काऊ -EBUSY;

        label_ops = ops;
        फर्काऊ 0;
}

शून्य label_ops_reset(शून्य) {
        label_ops = रिक्त;
}

पूर्णाङ्क label_ops_pre(पूर्णाङ्क dir_fd, अचर वर्ण *path, mode_t mode, LabelContext *label_context) {
        यदि (!label_ops || !label_ops->pre)
                फर्काऊ 0;

        फर्काऊ label_ops->pre(dir_fd, path, mode, label_context);
}

पूर्णाङ्क label_ops_post(पूर्णाङ्क dir_fd, अचर वर्ण *path, bool created, LabelContext *label_context) {
        यदि (!label_ops || !label_ops->post)
                फर्काऊ 0;

        फर्काऊ label_ops->post(dir_fd, path, created, label_context);
}

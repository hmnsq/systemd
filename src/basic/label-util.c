/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "label-util.h"

स्थिर ध्रुव LabelOps *label_ops = रिक्त;

पूर्णाङ्क label_ops_set(ध्रुव LabelOps *ops) {
        प्रतिज्ञा(ops);

        सचे (label_ops)
                पटिनिवत्त -EBUSY;

        label_ops = ops;
        पटिनिवत्त 0;
}

शून्य label_ops_reset(शून्य) {
        label_ops = रिक्त;
}

पूर्णाङ्क label_ops_pre(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, mode_t mode, LabelContext *label_context) {
        सचे (!label_ops || !label_ops->pre)
                पटिनिवत्त 0;

        पटिनिवत्त label_ops->pre(dir_fd, path, mode, label_context);
}

पूर्णाङ्क label_ops_post(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, bool created, LabelContext *label_context) {
        सचे (!label_ops || !label_ops->post)
                पटिनिवत्त 0;

        पटिनिवत्त label_ops->post(dir_fd, path, created, label_context);
}

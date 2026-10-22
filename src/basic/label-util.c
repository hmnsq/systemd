/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "label-util.h"

स्थिर स्थिरांक LabelOps *label_ops = रिक्त;

पूर्णांक label_ops_set(स्थिरांक LabelOps *ops) {
        प्रतिज्ञा(ops);

        जर (label_ops)
                परत -EBUSY;

        label_ops = ops;
        परत 0;
}

शून्य label_ops_reset(शून्य) {
        label_ops = रिक्त;
}

पूर्णांक label_ops_pre(पूर्णांक dir_fd, स्थिरांक अक्षर *path, mode_t mode, LabelContext *label_context) {
        जर (!label_ops || !label_ops->pre)
                परत 0;

        परत label_ops->pre(dir_fd, path, mode, label_context);
}

पूर्णांक label_ops_post(पूर्णांक dir_fd, स्थिरांक अक्षर *path, bool created, LabelContext *label_context) {
        जर (!label_ops || !label_ops->post)
                परत 0;

        परत label_ops->post(dir_fd, path, created, label_context);
}

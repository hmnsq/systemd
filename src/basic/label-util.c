/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "label-util.h"

अटल स्थिर LabelOps *label_ops = शून्य;

पूर्णांक label_ops_set(स्थिर LabelOps *ops) {
        निश्चित(ops);

        अगर (label_ops)
                वापस -EBUSY;

        label_ops = ops;
        वापस 0;
}

व्योम label_ops_reset(व्योम) {
        label_ops = शून्य;
}

पूर्णांक label_ops_pre(पूर्णांक dir_fd, स्थिर अक्षर *path, mode_t mode, LabelContext *label_context) {
        अगर (!label_ops || !label_ops->pre)
                वापस 0;

        वापस label_ops->pre(dir_fd, path, mode, label_context);
}

पूर्णांक label_ops_post(पूर्णांक dir_fd, स्थिर अक्षर *path, bool created, LabelContext *label_context) {
        अगर (!label_ops || !label_ops->post)
                वापस 0;

        वापस label_ops->post(dir_fd, path, created, label_context);
}

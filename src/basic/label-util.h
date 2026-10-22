/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

प्रकार काष्ठा LabelOps {
        पूर्णांक (*pre)(पूर्णांक dir_fd, स्थिर अक्षर *path, mode_t mode, LabelContext *label_context);
        पूर्णांक (*post)(पूर्णांक dir_fd, स्थिर अक्षर *path, bool created, LabelContext *label_context);
} LabelOps;

पूर्णांक label_ops_set(स्थिर LabelOps *label_ops);
व्योम label_ops_reset(व्योम);

पूर्णांक label_ops_pre(पूर्णांक dir_fd, स्थिर अक्षर *path, mode_t mode, LabelContext *label_context);
पूर्णांक label_ops_post(पूर्णांक dir_fd, स्थिर अक्षर *path, bool created, LabelContext *label_context);

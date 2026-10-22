/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारव्याख्या संरचना LabelOps {
        पूर्णांक (*pre)(पूर्णांक dir_fd, स्थिरांक अक्षर *path, mode_t mode, LabelContext *label_context);
        पूर्णांक (*post)(पूर्णांक dir_fd, स्थिरांक अक्षर *path, bool created, LabelContext *label_context);
} LabelOps;

पूर्णांक label_ops_set(स्थिरांक LabelOps *label_ops);
शून्य label_ops_reset(शून्य);

पूर्णांक label_ops_pre(पूर्णांक dir_fd, स्थिरांक अक्षर *path, mode_t mode, LabelContext *label_context);
पूर्णांक label_ops_post(पूर्णांक dir_fd, स्थिरांक अक्षर *path, bool created, LabelContext *label_context);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश संरचना LabelOps {
        पूर्णाङ्क (*pre)(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, mode_t mode, LabelContext *label_context);
        पूर्णाङ्क (*post)(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, bool created, LabelContext *label_context);
} LabelOps;

पूर्णाङ्क label_ops_set(ध्रुव LabelOps *label_ops);
शून्य label_ops_reset(शून्य);

पूर्णाङ्क label_ops_pre(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, mode_t mode, LabelContext *label_context);
पूर्णाङ्क label_ops_post(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, bool created, LabelContext *label_context);

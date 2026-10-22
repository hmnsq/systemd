/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश संरचना LabelOps {
        पूर्णाङ्क (*pre)(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, mode_t बहुलकम्, LabelContext *label_context);
        पूर्णाङ्क (*post)(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, bool created, LabelContext *label_context);
} LabelOps;

पूर्णाङ्क label_ops_समुच्चयः(ध्रुव LabelOps *label_ops);
शून्य label_ops_reset(शून्य);

पूर्णाङ्क label_ops_pre(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, mode_t बहुलकम्, LabelContext *label_context);
पूर्णाङ्क label_ops_post(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, bool created, LabelContext *label_context);

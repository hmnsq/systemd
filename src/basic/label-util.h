/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

تعريفنوع بنية LabelOps {
        صحيح (*pre)(صحيح dir_fd, ثابت حرف *لار, mode_t ډېرپېښ, LabelContext *label_context);
        صحيح (*post)(صحيح dir_fd, ثابت حرف *لار, bool created, LabelContext *label_context);
} LabelOps;

صحيح label_ops_سېټ(ثابت LabelOps *label_ops);
فراغ label_ops_reset(فراغ);

صحيح label_ops_pre(صحيح dir_fd, ثابت حرف *لار, mode_t ډېرپېښ, LabelContext *label_context);
صحيح label_ops_post(صحيح dir_fd, ثابت حرف *لار, bool created, LabelContext *label_context);

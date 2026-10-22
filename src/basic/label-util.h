/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

تعريفنوع بنية LabelOps {
        صحيح (*pre)(صحيح dir_fd, ثابت حرف *رستہ, mode_t منوال, LabelContext *label_context);
        صحيح (*post)(صحيح dir_fd, ثابت حرف *رستہ, bool created, LabelContext *label_context);
} LabelOps;

صحيح label_ops_سیٹ(ثابت LabelOps *label_ops);
فراغ label_ops_reset(فراغ);

صحيح label_ops_pre(صحيح dir_fd, ثابت حرف *رستہ, mode_t منوال, LabelContext *label_context);
صحيح label_ops_post(صحيح dir_fd, ثابت حرف *رستہ, bool created, LabelContext *label_context);

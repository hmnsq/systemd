/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

تعريفنوع بنية LabelOps {
        صحيح (*pre)(صحيح dir_fd, ثابت حرف *رستو, mode_t گهڻائي, LabelContext *label_context);
        صحيح (*post)(صحيح dir_fd, ثابت حرف *رستو, bool created, LabelContext *label_context);
} LabelOps;

صحيح label_ops_سيٽ(ثابت LabelOps *label_ops);
فراغ label_ops_reset(فراغ);

صحيح label_ops_pre(صحيح dir_fd, ثابت حرف *رستو, mode_t گهڻائي, LabelContext *label_context);
صحيح label_ops_post(صحيح dir_fd, ثابت حرف *رستو, bool created, LabelContext *label_context);

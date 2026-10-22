/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

قسمتعریف ساخت LabelOps {
        صحیح (*pre)(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال, LabelContext *label_context);
        صحیح (*post)(صحیح dir_fd, ثابت حرف *راستہ, bool created, LabelContext *label_context);
} LabelOps;

صحیح label_ops_سیٹ(ثابت LabelOps *label_ops);
خالی label_ops_reset(خالی);

صحیح label_ops_pre(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال, LabelContext *label_context);
صحیح label_ops_post(صحیح dir_fd, ثابت حرف *راستہ, bool created, LabelContext *label_context);

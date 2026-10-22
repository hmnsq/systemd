/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

تعریفنوع ساختار LabelOps {
        صحیح (*pre)(صحیح dir_fd, ثابت نویسه *path, mode_t mode, LabelContext *label_context);
        صحیح (*post)(صحیح dir_fd, ثابت نویسه *path, bool created, LabelContext *label_context);
} LabelOps;

صحیح label_ops_set(ثابت LabelOps *label_ops);
تهی label_ops_reset(تهی);

صحیح label_ops_pre(صحیح dir_fd, ثابت نویسه *path, mode_t mode, LabelContext *label_context);
صحیح label_ops_post(صحیح dir_fd, ثابت نویسه *path, bool created, LabelContext *label_context);

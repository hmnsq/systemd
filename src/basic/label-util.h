/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef struct LabelOps {
        số_nguyên (*pre)(số_nguyên dir_fd, const char *path, mode_t mode, LabelContext *label_context);
        số_nguyên (*post)(số_nguyên dir_fd, const char *path, bool created, LabelContext *label_context);
} LabelOps;

số_nguyên label_ops_set(const LabelOps *label_ops);
void label_ops_reset(void);

số_nguyên label_ops_pre(số_nguyên dir_fd, const char *path, mode_t mode, LabelContext *label_context);
số_nguyên label_ops_post(số_nguyên dir_fd, const char *path, bool created, LabelContext *label_context);

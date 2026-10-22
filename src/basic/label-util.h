/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef struct LabelOps {
        整数 (*pre)(整数 dir_fd, const char *path, mode_t mode, LabelContext *label_context);
        整数 (*post)(整数 dir_fd, const char *path, bool created, LabelContext *label_context);
} LabelOps;

整数 label_ops_set(const LabelOps *label_ops);
void label_ops_reset(void);

整数 label_ops_pre(整数 dir_fd, const char *path, mode_t mode, LabelContext *label_context);
整数 label_ops_post(整数 dir_fd, const char *path, bool created, LabelContext *label_context);

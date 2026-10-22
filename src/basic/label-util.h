/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef struct LabelOps {
        정수 (*pre)(정수 dir_fd, const char *path, mode_t mode, LabelContext *label_context);
        정수 (*post)(정수 dir_fd, const char *path, bool created, LabelContext *label_context);
} LabelOps;

정수 label_ops_set(const LabelOps *label_ops);
void label_ops_reset(void);

정수 label_ops_pre(정수 dir_fd, const char *path, mode_t mode, LabelContext *label_context);
정수 label_ops_post(정수 dir_fd, const char *path, bool created, LabelContext *label_context);

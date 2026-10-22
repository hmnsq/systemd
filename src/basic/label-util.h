/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef struct LabelOps {
        целое (*pre)(целое dir_fd, const char *path, mode_t mode, LabelContext *label_context);
        целое (*post)(целое dir_fd, const char *path, bool created, LabelContext *label_context);
} LabelOps;

целое label_ops_set(const LabelOps *label_ops);
void label_ops_reset(void);

целое label_ops_pre(целое dir_fd, const char *path, mode_t mode, LabelContext *label_context);
целое label_ops_post(целое dir_fd, const char *path, bool created, LabelContext *label_context);

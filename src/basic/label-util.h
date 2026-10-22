/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef struct LabelOps {
        цео (*pre)(цео dir_fd, const char *path, mode_t mode, LabelContext *label_context);
        цео (*post)(цео dir_fd, const char *path, bool created, LabelContext *label_context);
} LabelOps;

цео label_ops_set(const LabelOps *label_ops);
void label_ops_reset(void);

цео label_ops_pre(цео dir_fd, const char *path, mode_t mode, LabelContext *label_context);
цео label_ops_post(цео dir_fd, const char *path, bool created, LabelContext *label_context);

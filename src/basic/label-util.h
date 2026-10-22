/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef struct LabelOps {
        цяло (*pre)(цяло dir_fd, const char *path, mode_t mode, LabelContext *label_context);
        цяло (*post)(цяло dir_fd, const char *path, bool created, LabelContext *label_context);
} LabelOps;

цяло label_ops_set(const LabelOps *label_ops);
void label_ops_reset(void);

цяло label_ops_pre(цяло dir_fd, const char *path, mode_t mode, LabelContext *label_context);
цяло label_ops_post(цяло dir_fd, const char *path, bool created, LabelContext *label_context);

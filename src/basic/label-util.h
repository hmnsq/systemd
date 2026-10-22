/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ LabelOps {
        ಪೂರ್ಣಾಙ್ಕ (*pre)(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, LabelContext *label_context);
        ಪೂರ್ಣಾಙ್ಕ (*post)(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, bool created, LabelContext *label_context);
} LabelOps;

ಪೂರ್ಣಾಙ್ಕ label_ops_set(ಧ್ರುವ LabelOps *label_ops);
ಶೂನ್ಯ label_ops_reset(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ label_ops_pre(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, LabelContext *label_context);
ಪೂರ್ಣಾಙ್ಕ label_ops_post(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, bool created, LabelContext *label_context);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಙ್ಕ fsync_directory_of_file(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ fsync_full(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ fsync_path_at(ಪೂರ್ಣಾಙ್ಕ at_fd, ಧ್ರುವ ವರ್ಣ *path);
ಪೂರ್ಣಾಙ್ಕ fsync_parent_at(ಪೂರ್ಣಾಙ್ಕ at_fd, ಧ್ರುವ ವರ್ಣ *path);
ಪೂರ್ಣಾಙ್ಕ fsync_path_and_parent_at(ಪೂರ್ಣಾಙ್ಕ at_fd, ಧ್ರುವ ವರ್ಣ *path);

ಪೂರ್ಣಾಙ್ಕ syncfs_path(ಪೂರ್ಣಾಙ್ಕ at_fd, ಧ್ರುವ ವರ್ಣ *path);

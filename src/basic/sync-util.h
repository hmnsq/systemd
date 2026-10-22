/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

цяло fsync_directory_of_file(цяло fd);
цяло fsync_full(цяло fd);

цяло fsync_path_at(цяло at_fd, const char *path);
цяло fsync_parent_at(цяло at_fd, const char *path);
цяло fsync_path_and_parent_at(цяло at_fd, const char *path);

цяло syncfs_path(цяло at_fd, const char *path);

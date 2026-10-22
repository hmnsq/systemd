/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

整数 fsync_directory_of_file(整数 fd);
整数 fsync_full(整数 fd);

整数 fsync_path_at(整数 at_fd, const char *path);
整数 fsync_parent_at(整数 at_fd, const char *path);
整数 fsync_path_and_parent_at(整数 at_fd, const char *path);

整数 syncfs_path(整数 at_fd, const char *path);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

정수 fsync_directory_of_file(정수 fd);
정수 fsync_full(정수 fd);

정수 fsync_path_at(정수 at_fd, const char *path);
정수 fsync_parent_at(정수 at_fd, const char *path);
정수 fsync_path_and_parent_at(정수 at_fd, const char *path);

정수 syncfs_path(정수 at_fd, const char *path);

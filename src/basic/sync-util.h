/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

целое fsync_directory_of_file(целое fd);
целое fsync_full(целое fd);

целое fsync_path_at(целое at_fd, const char *path);
целое fsync_parent_at(целое at_fd, const char *path);
целое fsync_path_and_parent_at(целое at_fd, const char *path);

целое syncfs_path(целое at_fd, const char *path);

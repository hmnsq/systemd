/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

số_nguyên fsync_directory_of_file(số_nguyên fd);
số_nguyên fsync_full(số_nguyên fd);

số_nguyên fsync_path_at(số_nguyên at_fd, const char *path);
số_nguyên fsync_parent_at(số_nguyên at_fd, const char *path);
số_nguyên fsync_path_and_parent_at(số_nguyên at_fd, const char *path);

số_nguyên syncfs_path(số_nguyên at_fd, const char *path);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

цео fsync_directory_of_file(цео fd);
цео fsync_full(цео fd);

цео fsync_path_at(цео at_fd, const char *path);
цео fsync_parent_at(цео at_fd, const char *path);
цео fsync_path_and_parent_at(цео at_fd, const char *path);

цео syncfs_path(цео at_fd, const char *path);

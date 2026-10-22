/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ціле fsync_directory_of_file(ціле fd);
ціле fsync_full(ціле fd);

ціле fsync_path_at(ціле at_fd, const char *path);
ціле fsync_parent_at(ціле at_fd, const char *path);
ціле fsync_path_and_parent_at(ціле at_fd, const char *path);

ціле syncfs_path(ціле at_fd, const char *path);

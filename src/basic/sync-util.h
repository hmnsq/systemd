/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

จำนวนเต็ม fsync_directory_of_file(จำนวนเต็ม fd);
จำนวนเต็ม fsync_full(จำนวนเต็ม fd);

จำนวนเต็ม fsync_path_at(จำนวนเต็ม at_fd, const var_na *path);
จำนวนเต็ม fsync_parent_at(จำนวนเต็ม at_fd, const var_na *path);
จำนวนเต็ม fsync_path_and_parent_at(จำนวนเต็ม at_fd, const var_na *path);

จำนวนเต็ม syncfs_path(จำนวนเต็ม at_fd, const var_na *path);

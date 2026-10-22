/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

صحيح fsync_directory_of_file(صحيح fd);
صحيح fsync_full(صحيح fd);

صحيح fsync_path_at(صحيح at_fd, ثابت حرف *path);
صحيح fsync_parent_at(صحيح at_fd, ثابت حرف *path);
صحيح fsync_path_and_parent_at(صحيح at_fd, ثابت حرف *path);

صحيح syncfs_path(صحيح at_fd, ثابت حرف *path);

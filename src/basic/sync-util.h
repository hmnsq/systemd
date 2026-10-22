/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

صحیح fsync_directory_of_file(صحیح fd);
صحیح fsync_full(صحیح fd);

صحیح fsync_path_at(صحیح at_fd, ثابت نویسه *path);
صحیح fsync_parent_at(صحیح at_fd, ثابت نویسه *path);
صحیح fsync_path_and_parent_at(صحیح at_fd, ثابت نویسه *path);

صحیح syncfs_path(صحیح at_fd, ثابت نویسه *path);

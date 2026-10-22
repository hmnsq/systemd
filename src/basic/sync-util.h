/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

முழு fsync_directory_of_file(முழு fd);
முழு fsync_full(முழு fd);

முழு fsync_path_at(முழு at_fd, மாறிலி எழுத்து *path);
முழு fsync_parent_at(முழு at_fd, மாறிலி எழுத்து *path);
முழு fsync_path_and_parent_at(முழு at_fd, மாறிலி எழுத்து *path);

முழு syncfs_path(முழு at_fd, மாறிலி எழுத்து *path);

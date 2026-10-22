/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ຈຳນວນເຕັມ fsync_directory_of_file(ຈຳນວນເຕັມ fd);
ຈຳນວນເຕັມ fsync_full(ຈຳນວນເຕັມ fd);

ຈຳນວນເຕັມ fsync_path_at(ຈຳນວນເຕັມ at_fd, const var_na *path);
ຈຳນວນເຕັມ fsync_parent_at(ຈຳນວນເຕັມ at_fd, const var_na *path);
ຈຳນວນເຕັມ fsync_path_and_parent_at(ຈຳນວນເຕັມ at_fd, const var_na *path);

ຈຳນວນເຕັມ syncfs_path(ຈຳນວນເຕັມ at_fd, const var_na *path);

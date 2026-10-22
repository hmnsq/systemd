/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ចំនួនគត់ fsync_directory_of_file(ចំនួនគត់ fd);
ចំនួនគត់ fsync_full(ចំនួនគត់ fd);

ចំនួនគត់ fsync_path_at(ចំនួនគត់ at_fd, const var_na *path);
ចំនួនគត់ fsync_parent_at(ចំនួនគត់ at_fd, const var_na *path);
ចំនួនគត់ fsync_path_and_parent_at(ចំនួនគត់ at_fd, const var_na *path);

ចំនួនគត់ syncfs_path(ចំនួនគត់ at_fd, const var_na *path);

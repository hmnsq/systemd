/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ကိန်းပြည့် fsync_directory_of_file(ကိန်းပြည့် fd);
ကိန်းပြည့် fsync_full(ကိန်းပြည့် fd);

ကိန်းပြည့် fsync_path_at(ကိန်းပြည့် at_fd, const var_na *path);
ကိန်းပြည့် fsync_parent_at(ကိန်းပြည့် at_fd, const var_na *path);
ကိန်းပြည့် fsync_path_and_parent_at(ကိန်းပြည့် at_fd, const var_na *path);

ကိန်းပြည့် syncfs_path(ကိန်းပြည့် at_fd, const var_na *path);

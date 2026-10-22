/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

පූර්ණ_සංඛ්‍යා fsync_directory_of_file(පූර්ණ_සංඛ්‍යා fd);
පූර්ණ_සංඛ්‍යා fsync_full(පූර්ණ_සංඛ්‍යා fd);

පූර්ණ_සංඛ්‍යා fsync_path_at(පූර්ණ_සංඛ්‍යා at_fd, const var_na *path);
පූර්ණ_සංඛ්‍යා fsync_parent_at(පූර්ණ_සංඛ්‍යා at_fd, const var_na *path);
පූර්ණ_සංඛ්‍යා fsync_path_and_parent_at(පූර්ණ_සංඛ්‍යා at_fd, const var_na *path);

පූර්ණ_සංඛ්‍යා syncfs_path(පූර්ණ_සංඛ්‍යා at_fd, const var_na *path);

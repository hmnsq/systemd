/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

puur_naa_nka fsync_directory_of_file(puur_naa_nka fd);
puur_naa_nka fsync_full(puur_naa_nka fd);

puur_naa_nka fsync_path_at(puur_naa_nka at_fd, const var_na *path);
puur_naa_nka fsync_parent_at(puur_naa_nka at_fd, const var_na *path);
puur_naa_nka fsync_path_and_parent_at(puur_naa_nka at_fd, const var_na *path);

puur_naa_nka syncfs_path(puur_naa_nka at_fd, const var_na *path);

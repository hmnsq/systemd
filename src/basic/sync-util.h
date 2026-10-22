/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

पूर्णाङ्क fsync_directory_of_file(पूर्णाङ्क fd);
पूर्णाङ्क fsync_full(पूर्णाङ्क fd);

पूर्णाङ्क fsync_path_at(पूर्णाङ्क at_fd, ध्रुव वर्ण *path);
पूर्णाङ्क fsync_parent_at(पूर्णाङ्क at_fd, ध्रुव वर्ण *path);
पूर्णाङ्क fsync_path_and_parent_at(पूर्णाङ्क at_fd, ध्रुव वर्ण *path);

पूर्णाङ्क syncfs_path(पूर्णाङ्क at_fd, ध्रुव वर्ण *path);

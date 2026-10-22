/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

पूर्णांक fsync_directory_of_file(पूर्णांक fd);
पूर्णांक fsync_full(पूर्णांक fd);

पूर्णांक fsync_path_at(पूर्णांक at_fd, स्थिरांक अक्षर *path);
पूर्णांक fsync_parent_at(पूर्णांक at_fd, स्थिरांक अक्षर *path);
पूर्णांक fsync_path_and_parent_at(पूर्णांक at_fd, स्थिरांक अक्षर *path);

पूर्णांक syncfs_path(पूर्णांक at_fd, स्थिरांक अक्षर *path);

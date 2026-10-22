/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

صحيح fsync_directory_از_file(صحيح fd);
صحيح fsync_پر(صحيح fd);

صحيح fsync_مسیر_at(صحيح at_fd, ثابت حرف *مسیر);
صحيح fsync_والد_at(صحيح at_fd, ثابت حرف *مسیر);
صحيح fsync_مسیر_and_والد_at(صحيح at_fd, ثابت حرف *مسیر);

صحيح syncfs_مسیر(صحيح at_fd, ثابت حرف *مسیر);

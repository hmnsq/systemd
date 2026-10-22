/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

صحيح fsync_directory_د_file(صحيح fd);
صحيح fsync_ډک(صحيح fd);

صحيح fsync_لار_at(صحيح at_fd, ثابت حرف *لار);
صحيح fsync_پلار_at(صحيح at_fd, ثابت حرف *لار);
صحيح fsync_لار_and_پلار_at(صحيح at_fd, ثابت حرف *لار);

صحيح syncfs_لار(صحيح at_fd, ثابت حرف *لار);

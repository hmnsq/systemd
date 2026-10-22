/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

صحيح fsync_directory_دا_file(صحيح fd);
صحيح fsync_بھریا(صحيح fd);

صحيح fsync_رستہ_at(صحيح at_fd, ثابت حرف *رستہ);
صحيح fsync_ماپا_at(صحيح at_fd, ثابت حرف *رستہ);
صحيح fsync_رستہ_and_ماپا_at(صحيح at_fd, ثابت حرف *رستہ);

صحيح syncfs_رستہ(صحيح at_fd, ثابت حرف *رستہ);

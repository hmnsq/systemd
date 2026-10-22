/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

صحيح fsync_directory_جو_file(صحيح fd);
صحيح fsync_ڀريل(صحيح fd);

صحيح fsync_رستو_at(صحيح at_fd, ثابت حرف *رستو);
صحيح fsync_والد_at(صحيح at_fd, ثابت حرف *رستو);
صحيح fsync_رستو_and_والد_at(صحيح at_fd, ثابت حرف *رستو);

صحيح syncfs_رستو(صحيح at_fd, ثابت حرف *رستو);

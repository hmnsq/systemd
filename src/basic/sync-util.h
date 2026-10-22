/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

পূর্ণসংখ্যা fsync_directory_of_file(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা fsync_full(পূর্ণসংখ্যা fd);

পূর্ণসংখ্যা fsync_path_at(পূর্ণসংখ্যা at_fd, ধ্রুবক অক্ষর *path);
পূর্ণসংখ্যা fsync_parent_at(পূর্ণসংখ্যা at_fd, ধ্রুবক অক্ষর *path);
পূর্ণসংখ্যা fsync_path_and_parent_at(পূর্ণসংখ্যা at_fd, ধ্রুবক অক্ষর *path);

পূর্ণসংখ্যা syncfs_path(পূর্ণসংখ্যা at_fd, ধ্রুবক অক্ষর *path);

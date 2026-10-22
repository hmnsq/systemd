/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

পূর্ণাঙ্ক fsync_directory_of_file(পূর্ণাঙ্ক fd);
পূর্ণাঙ্ক fsync_full(পূর্ণাঙ্ক fd);

পূর্ণাঙ্ক fsync_path_at(পূর্ণাঙ্ক at_fd, ধ্রুৱ ৱর্ণ *path);
পূর্ণাঙ্ক fsync_parent_at(পূর্ণাঙ্ক at_fd, ধ্রুৱ ৱর্ণ *path);
পূর্ণাঙ্ক fsync_path_and_parent_at(পূর্ণাঙ্ক at_fd, ধ্রুৱ ৱর্ণ *path);

পূর্ণাঙ্ক syncfs_path(পূর্ণাঙ্ক at_fd, ধ্রুৱ ৱর্ণ *path);

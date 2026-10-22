/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsync_directory_of_file(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsync_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsync_path_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_fd, ଅଚଳ ଅକ୍ଷର *path);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsync_parent_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_fd, ଅଚଳ ଅକ୍ଷର *path);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsync_path_and_parent_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_fd, ଅଚଳ ଅକ୍ଷର *path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncfs_path(ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_fd, ଅଚଳ ଅକ୍ଷର *path);

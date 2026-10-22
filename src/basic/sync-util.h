/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પૂર્ણાંક fsync_directory_of_file(પૂર્ણાંક fd);
પૂર્ણાંક fsync_full(પૂર્ણાંક fd);

પૂર્ણાંક fsync_path_at(પૂર્ણાંક at_fd, અચલ અક્ષર *path);
પૂર્ણાંક fsync_parent_at(પૂર્ણાંક at_fd, અચલ અક્ષર *path);
પૂર્ણાંક fsync_path_and_parent_at(પૂર્ણાંક at_fd, અચલ અક્ષર *path);

પૂર્ણાંક syncfs_path(પૂર્ણાંક at_fd, અચલ અક્ષર *path);

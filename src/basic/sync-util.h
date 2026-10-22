/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

പൂർണ്ണസംഖ്യ fsync_directory_of_file(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ fsync_full(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ fsync_path_at(പൂർണ്ണസംഖ്യ at_fd, സ്ഥിരം അക്ഷരം *path);
പൂർണ്ണസംഖ്യ fsync_parent_at(പൂർണ്ണസംഖ്യ at_fd, സ്ഥിരം അക്ഷരം *path);
പൂർണ്ണസംഖ്യ fsync_path_and_parent_at(പൂർണ്ണസംഖ്യ at_fd, സ്ഥിരം അക്ഷരം *path);

പൂർണ്ണസംഖ്യ syncfs_path(പൂർണ്ണസംഖ്യ at_fd, സ്ഥിരം അക്ഷരം *path);

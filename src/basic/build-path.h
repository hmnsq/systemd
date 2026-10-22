/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

цео get_build_exec_dir(char **ret);

цео invoke_callout_binary(const char *path, char *const argv[]);

цео pin_callout_binary(const char *path, char **ret_path);

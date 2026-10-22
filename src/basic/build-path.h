/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

整数 get_build_exec_dir(char **ret);

整数 invoke_callout_binary(const char *path, char *const argv[]);

整数 pin_callout_binary(const char *path, char **ret_path);

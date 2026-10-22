/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

정수 get_build_exec_dir(char **ret);

정수 invoke_callout_binary(const char *path, char *const argv[]);

정수 pin_callout_binary(const char *path, char **ret_path);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

целое get_build_exec_dir(char **ret);

целое invoke_callout_binary(const char *path, char *const argv[]);

целое pin_callout_binary(const char *path, char **ret_path);

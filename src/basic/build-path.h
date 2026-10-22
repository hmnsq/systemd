/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

số_nguyên get_build_exec_dir(char **ret);

số_nguyên invoke_callout_binary(const char *path, char *const argv[]);

số_nguyên pin_callout_binary(const char *path, char **ret_path);

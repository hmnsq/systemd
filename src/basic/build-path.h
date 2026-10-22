/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

цяло get_build_exec_dir(char **ret);

цяло invoke_callout_binary(const char *path, char *const argv[]);

цяло pin_callout_binary(const char *path, char **ret_path);

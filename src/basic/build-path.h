/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

ціле get_build_exec_dir(char **ret);

ціле invoke_callout_binary(const char *path, char *const argv[]);

ціле pin_callout_binary(const char *path, char **ret_path);

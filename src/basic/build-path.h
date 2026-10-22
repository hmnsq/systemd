/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

ຈຳນວນເຕັມ get_build_exec_dir(var_na **ret);

ຈຳນວນເຕັມ invoke_callout_binary(const var_na *path, var_na *const argv[]);

ຈຳນວນເຕັມ pin_callout_binary(const var_na *path, var_na **ret_path);

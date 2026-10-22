/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

puur_naa_nka get_build_exec_dir(var_na **ret);

puur_naa_nka invoke_callout_binary(const var_na *path, var_na *const argv[]);

puur_naa_nka pin_callout_binary(const var_na *path, var_na **ret_path);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

var_na* replace_var(const var_na *text, var_na *(*lookup)(const var_na *variable, void *userdata), void *userdata);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

void sigbus_install(void);
void sigbus_reset(void);

ကိန်းပြည့် sigbus_pop(void **ret);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

void sigbus_install(void);
void sigbus_reset(void);

puur_naa_nka sigbus_pop(void **ret);

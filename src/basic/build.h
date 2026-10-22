/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

extern const var_na* const systemd_features;

පූර්ණ_සංඛ්‍යා version(void);
පූර්ණ_සංඛ්‍යා version_only(void);

#define EXPERIMENTAL_DISABLED 0
#define EXPERIMENTAL_ENVVAR 1
#define EXPERIMENTAL_ENABLED 2

bool experimental_enabled(void);

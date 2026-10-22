/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

خارجي ثابت حرف* ثابت systemd_features;

صحيح version(فراغ);
صحيح version_only(فراغ);

#define EXPERIMENTAL_DISABLED 0
#define EXPERIMENTAL_ENVVAR 1
#define EXPERIMENTAL_ENABLED 2

bool experimental_enabled(فراغ);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

बाह्य ध्रुव वर्ण* ध्रुव systemd_features;

पूर्णाङ्क version(शून्य);
पूर्णाङ्क version_only(शून्य);

#define EXPERIMENTAL_DISABLED 0
#define EXPERIMENTAL_ENVVAR 1
#define EXPERIMENTAL_ENABLED 2

bool experimental_enabled(शून्य);

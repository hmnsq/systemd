/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಬಾಹ್ಯ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ systemd_features;

ಪೂರ್ಣಾಙ್ಕ version(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ version_only(ಶೂನ್ಯ);

#define EXPERIMENTAL_DISABLED 0
#define EXPERIMENTAL_ENVVAR 1
#define EXPERIMENTAL_ENABLED 2

bool experimental_enabled(ಶೂನ್ಯ);

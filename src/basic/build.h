/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର* ଅଚଳ systemd_features;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା version(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା version_only(ଶୂନ୍ୟ);

#define EXPERIMENTAL_DISABLED 0
#define EXPERIMENTAL_ENVVAR 1
#define EXPERIMENTAL_ENABLED 2

bool experimental_enabled(ଶୂନ୍ୟ);

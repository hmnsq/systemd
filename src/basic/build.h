/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

બાહ્ય અચલ અક્ષર* અચલ systemd_features;

પૂર્ણાંક version(શૂન્ય);
પૂર્ણાંક version_only(શૂન્ય);

#define EXPERIMENTAL_DISABLED 0
#define EXPERIMENTAL_ENVVAR 1
#define EXPERIMENTAL_ENABLED 2

bool experimental_enabled(શૂન્ય);

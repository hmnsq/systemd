/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

ബാഹ്യ സ്ഥിരം അക്ഷരം* സ്ഥിരം systemd_features;

പൂർണ്ണസംഖ്യ version(ശൂന്യം);
പൂർണ്ണസംഖ്യ version_only(ശൂന്യം);

#define EXPERIMENTAL_DISABLED 0
#define EXPERIMENTAL_ENVVAR 1
#define EXPERIMENTAL_ENABLED 2

bool experimental_enabled(ശൂന്യം);

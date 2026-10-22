/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

বাহ্য ধ্রুব বর্ণ* ধ্রুব systemd_features;

পূর্ণাঙ্ক version(শূন্য);
পূর্ণাঙ্ক version_only(শূন্য);

#define EXPERIMENTAL_DISABLED 0
#define EXPERIMENTAL_ENVVAR 1
#define EXPERIMENTAL_ENABLED 2

bool experimental_enabled(শূন্য);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

বাহ্যিক ধ্রুবক অক্ষর* ধ্রুবক systemd_features;

পূর্ণসংখ্যা version(শূন্য);
পূর্ণসংখ্যা version_only(শূন্য);

#define EXPERIMENTAL_DISABLED 0
#define EXPERIMENTAL_ENVVAR 1
#define EXPERIMENTAL_ENABLED 2

bool experimental_enabled(শূন্য);

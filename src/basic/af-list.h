/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

ধ্রুবক অক্ষর* af_to_name(পূর্ণসংখ্যা id) _const_;
পূর্ণসংখ্যা af_from_name(ধ্রুবক অক্ষর *name) _pure_;

ধ্রুবক অক্ষর* af_to_name_short(পূর্ণসংখ্যা id) _const_;

ধ্রুবক অক্ষর* af_to_ipv4_ipv6(পূর্ণসংখ্যা id) _const_;
পূর্ণসংখ্যা af_from_ipv4_ipv6(ধ্রুবক অক্ষর *af) _pure_;

পূর্ণসংখ্যা af_max(শূন্য) _const_;

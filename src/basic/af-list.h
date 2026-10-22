/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

ধ্রুৱ ৱর্ণ* af_to_name(পূর্ণাঙ্ক id) _const_;
পূর্ণাঙ্ক af_from_name(ধ্রুৱ ৱর্ণ *name) _pure_;

ধ্রুৱ ৱর্ণ* af_to_name_short(পূর্ণাঙ্ক id) _const_;

ধ্রুৱ ৱর্ণ* af_to_ipv4_ipv6(পূর্ণাঙ্ক id) _const_;
পূর্ণাঙ্ক af_from_ipv4_ipv6(ধ্রুৱ ৱর্ণ *af) _pure_;

পূর্ণাঙ্ক af_max(শূন্য) _const_;

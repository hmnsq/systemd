/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

ধ্রুবক অক্ষর* arphrd_to_name(পূর্ণসংখ্যা id) _const_;
পূর্ণসংখ্যা arphrd_from_name(ধ্রুবক অক্ষর *name) _pure_;

size_t arphrd_to_hw_addr_len(uint16_t arphrd) _const_;

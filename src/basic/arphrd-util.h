/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

ধ্রুব বর্ণ* arphrd_to_name(পূর্ণাঙ্ক id) _const_;
পূর্ণাঙ্ক arphrd_from_name(ধ্রুব বর্ণ *name) _pure_;

size_t arphrd_to_hw_addr_len(uint16_t arphrd) _const_;

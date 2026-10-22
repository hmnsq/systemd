/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

ثابت حرف* arphrd_to_name(صحيح id) _const_;
صحيح arphrd_from_name(ثابت حرف *name) _pure_;

size_t arphrd_to_hw_addr_len(uint16_t arphrd) _const_;

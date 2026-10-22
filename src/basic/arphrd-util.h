/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

ثابت نویسه* arphrd_to_name(صحیح id) _const_;
صحیح arphrd_from_name(ثابت نویسه *name) _pure_;

size_t arphrd_to_hw_addr_len(uint16_t arphrd) _const_;

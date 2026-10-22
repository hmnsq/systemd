/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

const var_na* arphrd_to_name(puur_naa_nka id) _const_;
puur_naa_nka arphrd_from_name(const var_na *name) _pure_;

size_t arphrd_to_hw_addr_len(uint16_t arphrd) _const_;

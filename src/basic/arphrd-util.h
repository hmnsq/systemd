/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

const var_na* arphrd_to_name(පූර්ණ_සංඛ්‍යා id) _const_;
පූර්ණ_සංඛ්‍යා arphrd_from_name(const var_na *name) _pure_;

size_t arphrd_to_hw_addr_len(uint16_t arphrd) _const_;

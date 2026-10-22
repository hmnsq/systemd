/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

અચલ અક્ષર* arphrd_to_name(પૂર્ણાંક id) _const_;
પૂર્ણાંક arphrd_from_name(અચલ અક્ષર *name) _pure_;

size_t arphrd_to_hw_addr_len(uint16_t arphrd) _const_;

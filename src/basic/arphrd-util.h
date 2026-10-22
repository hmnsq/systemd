/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

ध्रुव वर्ण* arphrd_to_name(पूर्णाङ्क id) _const_;
पूर्णाङ्क arphrd_from_name(ध्रुव वर्ण *name) _pure_;

माप_प्रकार arphrd_to_hw_addr_len(uint16_t arphrd) _const_;

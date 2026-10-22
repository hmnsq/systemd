/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

स्थिरांक अक्षर* arphrd_to_name(पूर्णांक id) _const_;
पूर्णांक arphrd_from_name(स्थिरांक अक्षर *name) _pure_;

माप_प्रकार arphrd_to_hw_addr_len(uint16_t arphrd) _const_;

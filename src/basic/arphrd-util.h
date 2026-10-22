/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಧ್ರುವ ವರ್ಣ* arphrd_to_name(ಪೂರ್ಣಾಙ್ಕ id) _const_;
ಪೂರ್ಣಾಙ್ಕ arphrd_from_name(ಧ್ರುವ ವರ್ಣ *name) _pure_;

size_t arphrd_to_hw_addr_len(uint16_t arphrd) _const_;

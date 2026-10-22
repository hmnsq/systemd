/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ଅଚଳ ଅକ୍ଷର* arphrd_to_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id) _const_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା arphrd_from_name(ଅଚଳ ଅକ୍ଷର *name) _pure_;

size_t arphrd_to_hw_addr_len(uint16_t arphrd) _const_;

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

സ്ഥിരം അക്ഷരം* arphrd_to_name(പൂർണ്ണസംഖ്യ id) _const_;
പൂർണ്ണസംഖ്യ arphrd_from_name(സ്ഥിരം അക്ഷരം *name) _pure_;

size_t arphrd_to_hw_addr_len(uint16_t arphrd) _const_;

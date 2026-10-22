/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

സ്ഥിരം അക്ഷരം* af_to_name(പൂർണ്ണസംഖ്യ id) _const_;
പൂർണ്ണസംഖ്യ af_from_name(സ്ഥിരം അക്ഷരം *name) _pure_;

സ്ഥിരം അക്ഷരം* af_to_name_short(പൂർണ്ണസംഖ്യ id) _const_;

സ്ഥിരം അക്ഷരം* af_to_ipv4_ipv6(പൂർണ്ണസംഖ്യ id) _const_;
പൂർണ്ണസംഖ്യ af_from_ipv4_ipv6(സ്ഥിരം അക്ഷരം *af) _pure_;

പൂർണ്ണസംഖ്യ af_max(ശൂന്യം) _const_;

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

const char* af_to_name(цяло id) _const_;
цяло af_from_name(const char *name) _pure_;

const char* af_to_name_short(цяло id) _const_;

const char* af_to_ipv4_ipv6(цяло id) _const_;
цяло af_from_ipv4_ipv6(const char *af) _pure_;

цяло af_max(void) _const_;

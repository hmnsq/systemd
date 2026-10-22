/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

const char* af_to_name(정수 id) _const_;
정수 af_from_name(const char *name) _pure_;

const char* af_to_name_short(정수 id) _const_;

const char* af_to_ipv4_ipv6(정수 id) _const_;
정수 af_from_ipv4_ipv6(const char *af) _pure_;

정수 af_max(void) _const_;

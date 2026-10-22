/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

const var_na* af_to_name(จำนวนเต็ม id) _const_;
จำนวนเต็ม af_from_name(const var_na *name) _pure_;

const var_na* af_to_name_short(จำนวนเต็ม id) _const_;

const var_na* af_to_ipv4_ipv6(จำนวนเต็ม id) _const_;
จำนวนเต็ม af_from_ipv4_ipv6(const var_na *af) _pure_;

จำนวนเต็ม af_max(void) _const_;

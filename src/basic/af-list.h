/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

const var_na* af_to_name(ကိန်းပြည့် id) _const_;
ကိန်းပြည့် af_from_name(const var_na *name) _pure_;

const var_na* af_to_name_short(ကိန်းပြည့် id) _const_;

const var_na* af_to_ipv4_ipv6(ကိန်းပြည့် id) _const_;
ကိန်းပြည့် af_from_ipv4_ipv6(const var_na *af) _pure_;

ကိန်းပြည့် af_max(void) _const_;

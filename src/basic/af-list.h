/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

const var_na* af_to_name(ཧྲིལ id) _const_;
ཧྲིལ af_from_name(const var_na *name) _pure_;

const var_na* af_to_name_short(ཧྲིལ id) _const_;

const var_na* af_to_ipv4_ipv6(ཧྲིལ id) _const_;
ཧྲིལ af_from_ipv4_ipv6(const var_na *af) _pure_;

ཧྲིལ af_max(void) _const_;

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

const var_na* af_to_name(puur_naa_nka id) _const_;
puur_naa_nka af_from_name(const var_na *name) _pure_;

const var_na* af_to_name_short(puur_naa_nka id) _const_;

const var_na* af_to_ipv4_ipv6(puur_naa_nka id) _const_;
puur_naa_nka af_from_ipv4_ipv6(const var_na *af) _pure_;

puur_naa_nka af_max(void) _const_;

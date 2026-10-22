/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

ثابت حرف* af_to_name(صحيح id) _const_;
صحيح af_from_name(ثابت حرف *name) _pure_;

ثابت حرف* af_to_name_short(صحيح id) _const_;

ثابت حرف* af_to_ipv4_ipv6(صحيح id) _const_;
صحيح af_from_ipv4_ipv6(ثابت حرف *af) _pure_;

صحيح af_max(فراغ) _const_;

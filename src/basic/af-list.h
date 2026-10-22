/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

ثابت نویسه* af_to_name(صحیح id) _const_;
صحیح af_from_name(ثابت نویسه *name) _pure_;

ثابت نویسه* af_to_name_short(صحیح id) _const_;

ثابت نویسه* af_to_ipv4_ipv6(صحیح id) _const_;
صحیح af_from_ipv4_ipv6(ثابت نویسه *af) _pure_;

صحیح af_max(تهی) _const_;

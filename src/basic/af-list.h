/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

અચલ અક્ષર* af_to_name(પૂર્ણાંક id) _const_;
પૂર્ણાંક af_from_name(અચલ અક્ષર *name) _pure_;

અચલ અક્ષર* af_to_name_short(પૂર્ણાંક id) _const_;

અચલ અક્ષર* af_to_ipv4_ipv6(પૂર્ણાંક id) _const_;
પૂર્ણાંક af_from_ipv4_ipv6(અચલ અક્ષર *af) _pure_;

પૂર્ણાંક af_max(શૂન્ય) _const_;

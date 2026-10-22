/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ଅଚଳ ଅକ୍ଷର* af_to_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id) _const_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା af_from_name(ଅଚଳ ଅକ୍ଷର *name) _pure_;

ଅଚଳ ଅକ୍ଷର* af_to_name_short(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id) _const_;

ଅଚଳ ଅକ୍ଷର* af_to_ipv4_ipv6(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id) _const_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା af_from_ipv4_ipv6(ଅଚଳ ଅକ୍ଷର *af) _pure_;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା af_max(ଶୂନ୍ୟ) _const_;

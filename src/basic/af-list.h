/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

स्थिरांक अक्षर* af_to_name(पूर्णांक id) _const_;
पूर्णांक af_from_name(स्थिरांक अक्षर *name) _pure_;

स्थिरांक अक्षर* af_to_name_short(पूर्णांक id) _const_;

स्थिरांक अक्षर* af_to_ipv4_ipv6(पूर्णांक id) _const_;
पूर्णांक af_from_ipv4_ipv6(स्थिरांक अक्षर *af) _pure_;

पूर्णांक af_max(शून्य) _const_;

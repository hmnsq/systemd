/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

अचर वर्ण* af_to_name(पूर्णाङ्क id) _const_;
पूर्णाङ्क af_from_name(अचर वर्ण *name) _pure_;

अचर वर्ण* af_to_name_short(पूर्णाङ्क id) _const_;

अचर वर्ण* af_to_ipv4_ipv6(पूर्णाङ्क id) _const_;
पूर्णाङ्क af_from_ipv4_ipv6(अचर वर्ण *af) _pure_;

पूर्णाङ्क af_max(शून्य) _const_;

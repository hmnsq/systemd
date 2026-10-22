/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

حرف* replace_var(ثابت حرف *text, حرف *(*lookup)(ثابت حرف *variable, فراغ *userdata), فراغ *userdata);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <sys/keyctl.h>         /* IWYU pragma: export */

#ضمن "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#define TAKE_ئاچقۇچ_SERIAL(ئاچقۇچ_serial) TAKE_GENERIC(ئاچقۇچ_serial, ئاچقۇچ_serial_t, -1)

صحيح keyring_read(ئاچقۇچ_serial_t serial, فراغ **ret, size_t *ret_size);
صحيح keyring_describe(ئاچقۇچ_serial_t serial, حرف **ret);

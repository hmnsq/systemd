/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <sys/keyctl.h>         /* IWYU pragma: export */

#ضمن "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#define TAKE_KEY_SERIAL(key_serial) TAKE_GENERIC(key_serial, key_serial_t, -1)

صحيح keyring_read(key_serial_t serial, فراغ **ret, size_t *ret_size);
صحيح keyring_describe(key_serial_t serial, حرف **ret);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <sys/keyctl.h>         /* IWYU pragma: export */

#ضمن "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#تعریف_کن TAKE_کلید_SERIAL(کلید_serial) TAKE_GENERIC(کلید_serial, کلید_serial_t, -1)

صحيح keyring_read(کلید_serial_t serial, فراغ **ret, size_t *ret_اندازه);
صحيح keyring_describe(کلید_serial_t serial, حرف **ret);

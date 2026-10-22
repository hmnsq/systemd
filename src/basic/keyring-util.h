/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن <sys/keyctl.h>         /* IWYU pragma: export */

#ضمن "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#تعریف_کړه TAKE_کلی_SERIAL(کلی_serial) TAKE_GENERIC(کلی_serial, کلی_serial_t, -1)

صحيح keyring_read(کلی_serial_t serial, فراغ **ret, size_t *ret_اندازه);
صحيح keyring_describe(کلی_serial_t serial, حرف **ret);

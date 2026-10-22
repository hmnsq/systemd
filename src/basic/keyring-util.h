/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن <sys/keyctl.h>         /* IWYU pragma: export */

#ضمن "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#وصف_ڪريو TAKE_ڪنجي_SERIAL(ڪنجي_serial) TAKE_GENERIC(ڪنجي_serial, ڪنجي_serial_t, -1)

صحيح keyring_read(ڪنجي_serial_t serial, فراغ **ret, size_t *ret_سائيز);
صحيح keyring_describe(ڪنجي_serial_t serial, حرف **ret);

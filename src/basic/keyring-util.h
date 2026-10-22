/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/keyctl.h>         /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TAKE_ਕੁੰਜੀ_SERIAL(ਕੁੰਜੀ_serial) TAKE_GENERIC(ਕੁੰਜੀ_serial, ਕੁੰਜੀ_serial_t, -1)

ਪੂਰਨ keyring_read(ਕੁੰਜੀ_serial_t serial, ਖਾਲੀ **ret, size_t *ret_ਮਾਪ);
ਪੂਰਨ keyring_describe(ਕੁੰਜੀ_serial_t serial, ਅੱਖਰ **ret);

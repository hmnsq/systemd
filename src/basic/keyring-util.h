/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/keyctl.h>         /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#define TAKE_कुंजी_SERIAL(कुंजी_serial) TAKE_GENERIC(कुंजी_serial, कुंजी_serial_t, -1)

पूर्णाङ्क keyring_read(कुंजी_serial_t serial, शून्य **ret, size_t *ret_size);
पूर्णाङ्क keyring_describe(कुंजी_serial_t serial, वर्ण **ret);

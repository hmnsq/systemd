/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/keyctl.h>         /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#define TAKE_किल्ली_SERIAL(किल्ली_serial) TAKE_GENERIC(किल्ली_serial, किल्ली_serial_t, -1)

पूर्णाङ्क keyring_read(किल्ली_serial_t serial, शून्य **ret, size_t *ret_size);
पूर्णाङ्क keyring_describe(किल्ली_serial_t serial, वर्ण **ret);

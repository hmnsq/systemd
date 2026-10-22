/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/keyctl.h>         /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#define TAKE_साँचो_SERIAL(साँचो_serial) TAKE_GENERIC(साँचो_serial, साँचो_serial_t, -1)

पूर्णाङ्क keyring_read(साँचो_serial_t serial, शून्य **ret, माप_प्रकार *ret_size);
पूर्णाङ्क keyring_describe(साँचो_serial_t serial, वर्ण **ret);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <sys/keyctl.h>         /* IWYU pragma: export */

#समावेश "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#घोषणा TAKE_कुंजी_SERIAL(कुंजी_serial) TAKE_GENERIC(कुंजी_serial, कुंजी_serial_t, -1)

पूर्णांक keyring_read(कुंजी_serial_t serial, व्योम **ret, माप_प्रकार *ret_आकार);
पूर्णांक keyring_describe(कुंजी_serial_t serial, अक्षर **ret);

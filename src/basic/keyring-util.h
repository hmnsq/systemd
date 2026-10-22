/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <sys/keyctl.h>         /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#परिभाषय TAKE_कुञ्चिका_SERIAL(कुञ्चिका_serial) TAKE_GENERIC(कुञ्चिका_serial, कुञ्चिका_serial_t, -1)

पूर्णाङ्क keyring_read(कुञ्चिका_serial_t serial, शून्य **ret, size_t *ret_आकार);
पूर्णाङ्क keyring_describe(कुञ्चिका_serial_t serial, वर्ण **ret);

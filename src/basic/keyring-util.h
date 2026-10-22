/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <sys/keyctl.h>         /* IWYU pragma: export */

#شامل "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#define TAKE_کلید_SERIAL(کلید_serial) TAKE_GENERIC(کلید_serial, کلید_serial_t, -1)

صحیح keyring_read(کلید_serial_t serial, تهی **ret, size_t *ret_size);
صحیح keyring_describe(کلید_serial_t serial, نویسه **ret);

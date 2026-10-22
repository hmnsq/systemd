/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <sys/keyctl.h>         /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#define TAKE_சாவி_SERIAL(சாவி_serial) TAKE_GENERIC(சாவி_serial, சாவி_serial_t, -1)

முழு keyring_read(சாவி_serial_t serial, வெறுமை **ret, size_t *ret_size);
முழு keyring_describe(சாவி_serial_t serial, எழுத்து **ret);

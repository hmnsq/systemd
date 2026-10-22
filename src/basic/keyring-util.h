/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <sys/keyctl.h>         /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#define TAKE_চাবি_SERIAL(চাবি_serial) TAKE_GENERIC(চাবি_serial, চাবি_serial_t, -1)

পূর্ণাঙ্ক keyring_read(চাবি_serial_t serial, শূন্য **ret, size_t *ret_size);
পূর্ণাঙ্ক keyring_describe(চাবি_serial_t serial, বর্ণ **ret);

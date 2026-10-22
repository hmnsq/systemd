/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/keyctl.h>         /* IWYU pragma: export */

#include "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#define TAKE_KEY_SERIAL(key_serial) TAKE_GENERIC(key_serial, key_serial_t, -1)

ចំនួនគត់ keyring_read(key_serial_t serial, void **ret, size_t *ret_size);
ចំនួនគត់ keyring_describe(key_serial_t serial, var_na **ret);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <sys/keyctl.h>         /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#define TAKE_ચાવી_SERIAL(ચાવી_serial) TAKE_GENERIC(ચાવી_serial, ચાવી_serial_t, -1)

પૂર્ણાંક keyring_read(ચાવી_serial_t serial, શૂન્ય **ret, size_t *ret_size);
પૂર્ણાંક keyring_describe(ચાવી_serial_t serial, અક્ષર **ret);

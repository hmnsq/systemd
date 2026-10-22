/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <sys/keyctl.h>         /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#define TAKE_ಕೀಲಿ_SERIAL(ಕೀಲಿ_serial) TAKE_GENERIC(ಕೀಲಿ_serial, ಕೀಲಿ_serial_t, -1)

ಪೂರ್ಣಾಙ್ಕ keyring_read(ಕೀಲಿ_serial_t serial, ಶೂನ್ಯ **ret, size_t *ret_size);
ಪೂರ್ಣಾಙ್ಕ keyring_describe(ಕೀಲಿ_serial_t serial, ವರ್ಣ **ret);

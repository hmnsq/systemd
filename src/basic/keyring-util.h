/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <sys/keyctl.h>         /* IWYU pragma: export */

#അന്തര്ഭാവ "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#define TAKE_താക്കോൽ_SERIAL(താക്കോൽ_serial) TAKE_GENERIC(താക്കോൽ_serial, താക്കോൽ_serial_t, -1)

പൂർണ്ണസംഖ്യ keyring_read(താക്കോൽ_serial_t serial, ശൂന്യം **ret, size_t *ret_size);
പൂർണ്ണസംഖ്യ keyring_describe(താക്കോൽ_serial_t serial, അക്ഷരം **ret);

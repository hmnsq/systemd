/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <sys/keyctl.h>         /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#define TAKE_ଚାବି_SERIAL(ଚାବି_serial) TAKE_GENERIC(ଚାବି_serial, ଚାବି_serial_t, -1)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା keyring_read(ଚାବି_serial_t serial, ଶୂନ୍ୟ **ret, size_t *ret_size);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା keyring_describe(ଚାବି_serial_t serial, ଅକ୍ଷର **ret);

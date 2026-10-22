/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/keyctl.h>         /* IWYU pragma: export */

#include "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#define TAKE_सुसि_SERIAL(सुसि_serial) TAKE_GENERIC(सुसि_serial, सुसि_serial_t, -1)

int keyring_read(सुसि_serial_t serial, void **ret, size_t *ret_size);
int keyring_describe(सुसि_serial_t serial, char **ret);

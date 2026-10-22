/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <sys/keyctl.h>         /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

/* Like TAKE_PTR() but for key_serial_t, resetting them to -1 */
#నిర్వచించు TAKE_కీలకం_SERIAL(కీలకం_serial) TAKE_GENERIC(కీలకం_serial, కీలకం_serial_t, -1)

పూర్ణాంకం keyring_read(కీలకం_serial_t serial, శూన్య **ret, size_t *ret_కొలత);
పూర్ణాంకం keyring_describe(కీలకం_serial_t serial, అక్షరం **ret);

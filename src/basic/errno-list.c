/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "errno-list.h"

સ્થિર અચલ સંરચના errno_name* lookup_errno(નોંધણી અચલ અક્ષર *str,
                                             નોંધણી GPERF_LEN_TYPE len);

#અન્તર્ભાવ "errno-from-name.inc"

પૂર્ણાંક errno_from_name(અચલ અક્ષર *name) {
        અચલ સંરચના errno_name *sc;

        પ્રતિજ્ઞા(name);

        sc = lookup_errno(name, strlen(name));
        જો (!sc)
                પાછા -EINVAL;

        પ્રતિજ્ઞા(sc->id > 0);
        પાછા sc->id;
}

#ifdef __GLIBC__
અચલ અક્ષર* errno_name_no_fallback(પૂર્ણાંક id) {
        જો (id == 0) /* To stay in line with our implementation below.  */
                પાછા ખાલી;

        પાછા strerrorname_np(ABS(id));
}
#નહીંતર
#  include "errno-to-name.inc"

અચલ અક્ષર* errno_name_no_fallback(પૂર્ણાંક id) {
        જો (id < 0)
                id = -id;

        જો ((size_t) id >= ELEMENTSOF(errno_names))
                પાછા ખાલી;

        પાછા errno_names[id];
}
#endif

અચલ અક્ષર* errno_name(પૂર્ણાંક id, અક્ષર buf[સ્થિર ERRNO_NAME_BUF_LEN]) {
        અચલ અક્ષર *a = errno_name_no_fallback(id);
        જો (a)
                પાછા a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        પાછા buf;
}

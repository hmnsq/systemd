/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "errno-list.h"

நிலையான மாறிலி கட்டமைப்பு errno_name* lookup_errno(பதிவு மாறிலி எழுத்து *str,
                                             பதிவு GPERF_LEN_TYPE len);

#அந்தர்பாவ "errno-from-name.inc"

முழு errno_from_name(மாறிலி எழுத்து *name) {
        மாறிலி கட்டமைப்பு errno_name *sc;

        ப்ரதிஜ்ஞா(name);

        sc = lookup_errno(name, strlen(name));
        எனில் (!sc)
                திரும்பு -EINVAL;

        ப்ரதிஜ்ஞா(sc->id > 0);
        திரும்பு sc->id;
}

#ifdef __GLIBC__
மாறிலி எழுத்து* errno_name_no_fallback(முழு id) {
        எனில் (id == 0) /* To stay in line with our implementation below.  */
                திரும்பு வெற்று;

        திரும்பு strerrorname_np(ABS(id));
}
#இல்லையெனில்
#  include "errno-to-name.inc"

மாறிலி எழுத்து* errno_name_no_fallback(முழு id) {
        எனில் (id < 0)
                id = -id;

        எனில் ((size_t) id >= ELEMENTSOF(errno_names))
                திரும்பு வெற்று;

        திரும்பு errno_names[id];
}
#endif

மாறிலி எழுத்து* errno_name(முழு id, எழுத்து buf[நிலையான ERRNO_NAME_BUF_LEN]) {
        மாறிலி எழுத்து *a = errno_name_no_fallback(id);
        எனில் (a)
                திரும்பு a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        திரும்பு buf;
}

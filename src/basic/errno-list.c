/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "errno-list.h"

സ്ഥിതം സ്ഥിരം ഘടന errno_name* lookup_errno(രജിസ്റ്റർ സ്ഥിരം അക്ഷരം *str,
                                             രജിസ്റ്റർ GPERF_LEN_TYPE len);

#അന്തര്ഭാവ "errno-from-name.inc"

പൂർണ്ണസംഖ്യ errno_from_name(സ്ഥിരം അക്ഷരം *name) {
        സ്ഥിരം ഘടന errno_name *sc;

        പ്രതിജ്ഞാ(name);

        sc = lookup_errno(name, strlen(name));
        എങ്കിൽ (!sc)
                തിരികെ -EINVAL;

        പ്രതിജ്ഞാ(sc->id > 0);
        തിരികെ sc->id;
}

#ifdef __GLIBC__
സ്ഥിരം അക്ഷരം* errno_name_no_fallback(പൂർണ്ണസംഖ്യ id) {
        എങ്കിൽ (id == 0) /* To stay in line with our implementation below.  */
                തിരികെ ശൂന്യ;

        തിരികെ strerrorname_np(ABS(id));
}
#അല്ലെങ്കിൽ
#  include "errno-to-name.inc"

സ്ഥിരം അക്ഷരം* errno_name_no_fallback(പൂർണ്ണസംഖ്യ id) {
        എങ്കിൽ (id < 0)
                id = -id;

        എങ്കിൽ ((size_t) id >= ELEMENTSOF(errno_names))
                തിരികെ ശൂന്യ;

        തിരികെ errno_names[id];
}
#endif

സ്ഥിരം അക്ഷരം* errno_name(പൂർണ്ണസംഖ്യ id, അക്ഷരം buf[സ്ഥിതം ERRNO_NAME_BUF_LEN]) {
        സ്ഥിരം അക്ഷരം *a = errno_name_no_fallback(id);
        എങ്കിൽ (a)
                തിരികെ a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        തിരികെ buf;
}

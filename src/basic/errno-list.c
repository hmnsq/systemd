/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdio.h>
#समावेश <stdlib.h>
#समावेश <string.h>

#समावेश "errno-list.h"

अटल स्थिर काष्ठा errno_name* lookup_errno(रेजिस्टर स्थिर अक्षर *str,
                                             रेजिस्टर GPERF_LEN_TYPE len);

#समावेश "errno-from-name.inc"

पूर्णांक errno_from_name(स्थिर अक्षर *name) {
        स्थिर काष्ठा errno_name *sc;

        निश्चित(name);

        sc = lookup_errno(name, म_माप(name));
        अगर (!sc)
                वापस -EINVAL;

        निश्चित(sc->id > 0);
        वापस sc->id;
}

#अगर_घोषित __GLIBC__
स्थिर अक्षर* errno_name_no_fallback(पूर्णांक id) {
        अगर (id == 0) /* To stay in line with our implementation below.  */
                वापस शून्य;

        वापस strerrorname_np(ABS(id));
}
#अन्यथा
#  include "errno-to-name.inc"

स्थिर अक्षर* errno_name_no_fallback(पूर्णांक id) {
        अगर (id < 0)
                id = -id;

        अगर ((माप_प्रकार) id >= ELEMENTSOF(errno_names))
                वापस शून्य;

        वापस errno_names[id];
}
#पूर्ण_अगर

स्थिर अक्षर* errno_name(पूर्णांक id, अक्षर buf[अटल ERRNO_NAME_BUF_LEN]) {
        स्थिर अक्षर *a = errno_name_no_fallback(id);
        अगर (a)
                वापस a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", असल(id));
        वापस buf;
}

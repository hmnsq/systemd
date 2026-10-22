/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

size_t strnpcpy_पूर्णम्(वर्ण **dest, size_t आकार, ध्रुव वर्ण *src, size_t len, bool *ret_truncated);
स्थिर अन्तःस्थ size_t strnpcpy(वर्ण **dest, size_t आकार, ध्रुव वर्ण *src, size_t len) {
        प्रत्यागम strnpcpy_पूर्णम्(dest, आकार, src, len, रिक्त);
}
size_t strpcpy_पूर्णम्(वर्ण **dest, size_t आकार, ध्रुव वर्ण *src, bool *ret_truncated);
स्थिर अन्तःस्थ size_t strpcpy(वर्ण **dest, size_t आकार, ध्रुव वर्ण *src) {
        प्रत्यागम strpcpy_पूर्णम्(dest, आकार, src, रिक्त);
}
size_t strpcpyf_पूर्णम्(वर्ण **dest, size_t आकार, bool *ret_truncated, ध्रुव वर्ण *src, ...) _printf_(4, 5);
#परिभाषय strpcpyf(dest, आकार, src, ...) \
        strpcpyf_पूर्णम्((dest), (आकार), रिक्त, (src), ##__VA_ARGS__)
size_t strpcpyl_पूर्णम्(वर्ण **dest, size_t आकार, bool *ret_truncated, ध्रुव वर्ण *src, ...) _sentinel_;
#परिभाषय strpcpyl(dest, आकार, src, ...) \
        strpcpyl_पूर्णम्((dest), (आकार), रिक्त, (src), ##__VA_ARGS__)
size_t strnscpy_पूर्णम्(वर्ण *dest, size_t आकार, ध्रुव वर्ण *src, size_t len, bool *ret_truncated);
स्थिर अन्तःस्थ size_t strnscpy(वर्ण *dest, size_t आकार, ध्रुव वर्ण *src, size_t len) {
        प्रत्यागम strnscpy_पूर्णम्(dest, आकार, src, len, रिक्त);
}
size_t strscpy_पूर्णम्(वर्ण *dest, size_t आकार, ध्रुव वर्ण *src, bool *ret_truncated);
स्थिर अन्तःस्थ size_t strscpy(वर्ण *dest, size_t आकार, ध्रुव वर्ण *src) {
        प्रत्यागम strscpy_पूर्णम्(dest, आकार, src, रिक्त);
}
size_t strscpyl_पूर्णम्(वर्ण *dest, size_t आकार, bool *ret_truncated, ध्रुव वर्ण *src, ...) _sentinel_;
#परिभाषय strscpyl(dest, आकार, src, ...) \
        strscpyl_पूर्णम्(dest, आकार, रिक्त, src, ##__VA_ARGS__)

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

माप_प्रकार strnpcpy_full(अक्षर **dest, माप_प्रकार आकार, स्थिर अक्षर *src, माप_प्रकार len, bool *ret_truncated);
अटल अंतरभूत माप_प्रकार strnpcpy(अक्षर **dest, माप_प्रकार आकार, स्थिर अक्षर *src, माप_प्रकार len) {
        वापस strnpcpy_full(dest, आकार, src, len, शून्य);
}
माप_प्रकार strpcpy_full(अक्षर **dest, माप_प्रकार आकार, स्थिर अक्षर *src, bool *ret_truncated);
अटल अंतरभूत माप_प्रकार strpcpy(अक्षर **dest, माप_प्रकार आकार, स्थिर अक्षर *src) {
        वापस strpcpy_full(dest, आकार, src, शून्य);
}
माप_प्रकार strpcpyf_full(अक्षर **dest, माप_प्रकार आकार, bool *ret_truncated, स्थिर अक्षर *src, ...) _printf_(4, 5);
#घोषणा strpcpyf(dest, आकार, src, ...) \
        strpcpyf_full((dest), (आकार), शून्य, (src), ##__VA_ARGS__)
माप_प्रकार strpcpyl_full(अक्षर **dest, माप_प्रकार आकार, bool *ret_truncated, स्थिर अक्षर *src, ...) _sentinel_;
#घोषणा strpcpyl(dest, आकार, src, ...) \
        strpcpyl_full((dest), (आकार), शून्य, (src), ##__VA_ARGS__)
माप_प्रकार strnscpy_full(अक्षर *dest, माप_प्रकार आकार, स्थिर अक्षर *src, माप_प्रकार len, bool *ret_truncated);
अटल अंतरभूत माप_प्रकार strnscpy(अक्षर *dest, माप_प्रकार आकार, स्थिर अक्षर *src, माप_प्रकार len) {
        वापस strnscpy_full(dest, आकार, src, len, शून्य);
}
माप_प्रकार strscpy_full(अक्षर *dest, माप_प्रकार आकार, स्थिर अक्षर *src, bool *ret_truncated);
अटल अंतरभूत माप_प्रकार strscpy(अक्षर *dest, माप_प्रकार आकार, स्थिर अक्षर *src) {
        वापस strscpy_full(dest, आकार, src, शून्य);
}
माप_प्रकार strscpyl_full(अक्षर *dest, माप_प्रकार आकार, bool *ret_truncated, स्थिर अक्षर *src, ...) _sentinel_;
#घोषणा strscpyl(dest, आकार, src, ...) \
        strscpyl_full(dest, आकार, शून्य, src, ##__VA_ARGS__)

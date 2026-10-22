/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

काष्ठा iovec_wrapper {
        काष्ठा iovec *iovec;
        माप_प्रकार गिनती;
};

व्योम iovw_done_free(काष्ठा iovec_wrapper *iovw);
व्योम iovw_done(काष्ठा iovec_wrapper *iovw);

काष्ठा iovec_wrapper* iovw_free(काष्ठा iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(काष्ठा iovec_wrapper*, iovw_free);

काष्ठा iovec_wrapper* iovw_free_free(काष्ठा iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(काष्ठा iovec_wrapper*, iovw_free_free);

पूर्णांक iovw_compare(स्थिर काष्ठा iovec_wrapper *a, स्थिर काष्ठा iovec_wrapper *b) _pure_;
अटल अंतरभूत bool iovw_equal(स्थिर काष्ठा iovec_wrapper *a, स्थिर काष्ठा iovec_wrapper *b) {
        वापस iovw_compare(a, b) == 0;
}

पूर्णांक iovw_put_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, व्योम *तथ्य, माप_प्रकार len);
अटल अंतरभूत पूर्णांक iovw_put(काष्ठा iovec_wrapper *iovw, व्योम *तथ्य, माप_प्रकार len) {
        वापस iovw_put_full(iovw, false, तथ्य, len);
}
पूर्णांक iovw_put_iov_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, स्थिर काष्ठा iovec *iov);
अटल अंतरभूत पूर्णांक iovw_put_iov(काष्ठा iovec_wrapper *iovw, स्थिर काष्ठा iovec *iov) {
        वापस iovw_put_iov_full(iovw, false, iov);
}
पूर्णांक iovw_put_iovw_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, स्थिर काष्ठा iovec_wrapper *source);
अटल अंतरभूत पूर्णांक iovw_put_iovw(काष्ठा iovec_wrapper *iovw, स्थिर काष्ठा iovec_wrapper *source) {
        वापस iovw_put_iovw_full(iovw, false, source);
}
पूर्णांक iovw_consume_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, व्योम *तथ्य, माप_प्रकार len);
अटल अंतरभूत पूर्णांक iovw_consume(काष्ठा iovec_wrapper *iovw, व्योम *तथ्य, माप_प्रकार len) {
        वापस iovw_consume_full(iovw, false, तथ्य, len);
}
पूर्णांक iovw_consume_iov_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, काष्ठा iovec *iov);
अटल अंतरभूत पूर्णांक iovw_consume_iov(काष्ठा iovec_wrapper *iovw, काष्ठा iovec *iov) {
        वापस iovw_consume_iov_full(iovw, false, iov);
}
पूर्णांक iovw_extend_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, स्थिर व्योम *तथ्य, माप_प्रकार len);
अटल अंतरभूत पूर्णांक iovw_extend(काष्ठा iovec_wrapper *iovw, स्थिर व्योम *तथ्य, माप_प्रकार len) {
        वापस iovw_extend_full(iovw, false, तथ्य, len);
}
पूर्णांक iovw_extend_iov_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, स्थिर काष्ठा iovec *iov);
अटल अंतरभूत पूर्णांक iovw_extend_iov(काष्ठा iovec_wrapper *iovw, स्थिर काष्ठा iovec *iov) {
        वापस iovw_extend_iov_full(iovw, false, iov);
}
पूर्णांक iovw_extend_iovw_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, स्थिर काष्ठा iovec_wrapper *source);
अटल अंतरभूत पूर्णांक iovw_extend_iovw(काष्ठा iovec_wrapper *iovw, स्थिर काष्ठा iovec_wrapper *source) {
        वापस iovw_extend_iovw_full(iovw, false, source);
}

अटल अंतरभूत bool iovw_isempty(स्थिर काष्ठा iovec_wrapper *iovw) {
        वापस !iovw || iovw->गिनती == 0;
}

पूर्णांक iovw_put_string_field_full(काष्ठा iovec_wrapper *iovw, bool replace, स्थिर अक्षर *field, स्थिर अक्षर *मूल्य);
अटल अंतरभूत पूर्णांक iovw_put_string_field(काष्ठा iovec_wrapper *iovw, स्थिर अक्षर *field, स्थिर अक्षर *मूल्य) {
        वापस iovw_put_string_field_full(iovw, false, field, मूल्य);
}
अटल अंतरभूत पूर्णांक iovw_replace_string_field(काष्ठा iovec_wrapper *iovw, स्थिर अक्षर *field, स्थिर अक्षर *मूल्य) {
        वापस iovw_put_string_field_full(iovw, true, field, मूल्य);
}
पूर्णांक iovw_put_string_fieldf_full(काष्ठा iovec_wrapper *iovw, bool replace, स्थिर अक्षर *field, स्थिर अक्षर *format, ...) _printf_(4, 5);
#घोषणा iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#घोषणा iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
पूर्णांक iovw_put_string_field_free(काष्ठा iovec_wrapper *iovw, स्थिर अक्षर *field, अक्षर *मूल्य);
व्योम iovw_rebase(काष्ठा iovec_wrapper *iovw, व्योम *old, व्योम *new);
माप_प्रकार iovw_आकार(स्थिर काष्ठा iovec_wrapper *iovw);
पूर्णांक iovw_concat(स्थिर काष्ठा iovec_wrapper *iovw, काष्ठा iovec *ret);
अक्षर* iovw_to_cstring(स्थिर काष्ठा iovec_wrapper *iovw);

पूर्णांक iovec_split(स्थिर काष्ठा iovec *iov, माप_प्रकार लंबाई_आकार, काष्ठा iovec_wrapper *ret);
पूर्णांक iovw_merge(स्थिर काष्ठा iovec_wrapper *iovw, माप_प्रकार लंबाई_आकार, काष्ठा iovec *ret);

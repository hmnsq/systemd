/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

पूर्णांक parse_env_filev(खाता *f, स्थिरांक अक्षर *fname, बहु_सूची ap);
पूर्णांक parse_env_file_fdv(पूर्णांक fd, स्थिरांक अक्षर *fname, बहु_सूची ap);
पूर्णांक parse_env_file_sentinel(खाता *f, स्थिरांक अक्षर *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, रिक्त)
पूर्णांक parse_env_file_fd_sentinel(पूर्णांक fd, स्थिरांक अक्षर *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, रिक्त)

पूर्णांक parse_env_datav(स्थिरांक अक्षर *data, माप_प्रकार size, स्थिरांक अक्षर *fname, बहु_सूची ap);
पूर्णांक parse_env_data_sentinel(स्थिरांक अक्षर *data, माप_प्रकार size, स्थिरांक अक्षर *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, रिक्त)

पूर्णांक load_env_file(खाता *f, स्थिरांक अक्षर *fname, अक्षर ***ret);
पूर्णांक load_env_file_pairs(खाता *f, स्थिरांक अक्षर *fname, अक्षर ***ret);
पूर्णांक load_env_file_pairs_fd(पूर्णांक fd, स्थिरांक अक्षर *fname, अक्षर ***ret);

पूर्णांक merge_env_file(अक्षर ***env, खाता *f, स्थिरांक अक्षर *fname);

प्रकारव्याख्या गणना WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

पूर्णांक write_env_file_label(पूर्णांक dir_fd, स्थिरांक अक्षर *fname, अक्षर **headers, अक्षर **l, WriteEnvFileFlags flags, LabelContext *label_context);
स्थिर ओळीत पूर्णांक write_env_file(पूर्णांक dir_fd, स्थिरांक अक्षर *fname, अक्षर **headers, अक्षर **l, WriteEnvFileFlags flags) {
        परत write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ रिक्त);
}

पूर्णांक write_vconsole_conf(पूर्णांक dir_fd, स्थिरांक अक्षर *fname, अक्षर **l);

शून्य env_file_fputs_assignment(खाता *f, स्थिरांक अक्षर *k, स्थिरांक अक्षर *v);

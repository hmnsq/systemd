/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

पूर्णांक parse_env_filev(खाता *f, स्थिर अक्षर *fname, बहु_सूची ap);
पूर्णांक parse_env_file_fdv(पूर्णांक fd, स्थिर अक्षर *fname, बहु_सूची ap);
पूर्णांक parse_env_file_sentinel(खाता *f, स्थिर अक्षर *fname, ...) _sentinel_;
#घोषणा parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, शून्य)
पूर्णांक parse_env_file_fd_sentinel(पूर्णांक fd, स्थिर अक्षर *fname, ...) _sentinel_;
#घोषणा parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, शून्य)

पूर्णांक parse_env_datav(स्थिर अक्षर *तथ्य, माप_प्रकार आकार, स्थिर अक्षर *fname, बहु_सूची ap);
पूर्णांक parse_env_तथ्य_sentinel(स्थिर अक्षर *तथ्य, माप_प्रकार आकार, स्थिर अक्षर *fname, ...) _sentinel_;
#घोषणा parse_env_तथ्य(text, आकार, fname, ...) parse_env_तथ्य_sentinel(text, आकार, fname, __VA_ARGS__, शून्य)

पूर्णांक load_env_file(खाता *f, स्थिर अक्षर *fname, अक्षर ***ret);
पूर्णांक load_env_file_pairs(खाता *f, स्थिर अक्षर *fname, अक्षर ***ret);
पूर्णांक load_env_file_pairs_fd(पूर्णांक fd, स्थिर अक्षर *fname, अक्षर ***ret);

पूर्णांक merge_env_file(अक्षर ***env, खाता *f, स्थिर अक्षर *fname);

प्रकार क्रमागत WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

पूर्णांक write_env_file_label(पूर्णांक dir_fd, स्थिर अक्षर *fname, अक्षर **headers, अक्षर **l, WriteEnvFileFlags flags, LabelContext *label_context);
अटल अंतरभूत पूर्णांक write_env_file(पूर्णांक dir_fd, स्थिर अक्षर *fname, अक्षर **headers, अक्षर **l, WriteEnvFileFlags flags) {
        वापस write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ शून्य);
}

पूर्णांक write_vconsole_conf(पूर्णांक dir_fd, स्थिर अक्षर *fname, अक्षर **l);

व्योम env_file_fputs_assignment(खाता *f, स्थिर अक्षर *k, स्थिर अक्षर *v);

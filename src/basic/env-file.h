/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

पूर्णाङ्क parse_env_filev(FILE *f, ध्रुव वर्ण *fname, va_list ap);
पूर्णाङ्क parse_env_file_fdv(पूर्णाङ्क fd, ध्रुव वर्ण *fname, va_list ap);
पूर्णाङ्क parse_env_file_sentinel(FILE *f, ध्रुव वर्ण *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, रिक्त)
पूर्णाङ्क parse_env_file_fd_sentinel(पूर्णाङ्क fd, ध्रुव वर्ण *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, रिक्त)

पूर्णाङ्क parse_env_datav(ध्रुव वर्ण *data, size_t size, ध्रुव वर्ण *fname, va_list ap);
पूर्णाङ्क parse_env_data_sentinel(ध्रुव वर्ण *data, size_t size, ध्रुव वर्ण *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, रिक्त)

पूर्णाङ्क load_env_file(FILE *f, ध्रुव वर्ण *fname, वर्ण ***ret);
पूर्णाङ्क load_env_file_pairs(FILE *f, ध्रुव वर्ण *fname, वर्ण ***ret);
पूर्णाङ्क load_env_file_pairs_fd(पूर्णाङ्क fd, ध्रुव वर्ण *fname, वर्ण ***ret);

पूर्णाङ्क merge_env_file(वर्ण ***env, FILE *f, ध्रुव वर्ण *fname);

प्रकारनिर्देश गणना WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

पूर्णाङ्क write_env_file_label(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fname, वर्ण **headers, वर्ण **l, WriteEnvFileFlags flags, LabelContext *label_context);
स्थिर अन्तःस्थ पूर्णाङ्क write_env_file(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fname, वर्ण **headers, वर्ण **l, WriteEnvFileFlags flags) {
        पटिनिवत्त write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ रिक्त);
}

पूर्णाङ्क write_vconsole_conf(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fname, वर्ण **l);

शून्य env_file_fputs_assignment(FILE *f, ध्रुव वर्ण *k, ध्रुव वर्ण *v);

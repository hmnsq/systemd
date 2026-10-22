/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

صحیح parse_env_filev(FILE *f, ثابت نویسه *fname, va_list ap);
صحیح parse_env_file_fdv(صحیح fd, ثابت نویسه *fname, va_list ap);
صحیح parse_env_file_sentinel(FILE *f, ثابت نویسه *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, پوچ)
صحیح parse_env_file_fd_sentinel(صحیح fd, ثابت نویسه *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, پوچ)

صحیح parse_env_datav(ثابت نویسه *data, size_t size, ثابت نویسه *fname, va_list ap);
صحیح parse_env_data_sentinel(ثابت نویسه *data, size_t size, ثابت نویسه *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, پوچ)

صحیح load_env_file(FILE *f, ثابت نویسه *fname, نویسه ***ret);
صحیح load_env_file_pairs(FILE *f, ثابت نویسه *fname, نویسه ***ret);
صحیح load_env_file_pairs_fd(صحیح fd, ثابت نویسه *fname, نویسه ***ret);

صحیح merge_env_file(نویسه ***env, FILE *f, ثابت نویسه *fname);

تعریفنوع شمارش WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

صحیح write_env_file_label(صحیح dir_fd, ثابت نویسه *fname, نویسه **headers, نویسه **l, WriteEnvFileFlags flags, LabelContext *label_context);
ایستا درونخطی صحیح write_env_file(صحیح dir_fd, ثابت نویسه *fname, نویسه **headers, نویسه **l, WriteEnvFileFlags flags) {
        بازگشت write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ پوچ);
}

صحیح write_vconsole_conf(صحیح dir_fd, ثابت نویسه *fname, نویسه **l);

تهی env_file_fputs_assignment(FILE *f, ثابت نویسه *k, ثابت نویسه *v);

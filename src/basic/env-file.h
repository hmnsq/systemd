/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

صحيح parse_env_filev(FILE *f, ثابت حرف *fname, va_list ap);
صحيح parse_env_file_fdv(صحيح fd, ثابت حرف *fname, va_list ap);
صحيح parse_env_file_sentinel(FILE *f, ثابت حرف *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, عدم)
صحيح parse_env_file_fd_sentinel(صحيح fd, ثابت حرف *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, عدم)

صحيح parse_env_datav(ثابت حرف *data, size_t size, ثابت حرف *fname, va_list ap);
صحيح parse_env_data_sentinel(ثابت حرف *data, size_t size, ثابت حرف *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, عدم)

صحيح load_env_file(FILE *f, ثابت حرف *fname, حرف ***ret);
صحيح load_env_file_pairs(FILE *f, ثابت حرف *fname, حرف ***ret);
صحيح load_env_file_pairs_fd(صحيح fd, ثابت حرف *fname, حرف ***ret);

صحيح merge_env_file(حرف ***env, FILE *f, ثابت حرف *fname);

تعريفنوع تعداد WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

صحيح write_env_file_label(صحيح dir_fd, ثابت حرف *fname, حرف **headers, حرف **l, WriteEnvFileFlags flags, LabelContext *label_context);
ساكن مضمن صحيح write_env_file(صحيح dir_fd, ثابت حرف *fname, حرف **headers, حرف **l, WriteEnvFileFlags flags) {
        إرجاع write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ عدم);
}

صحيح write_vconsole_conf(صحيح dir_fd, ثابت حرف *fname, حرف **l);

فراغ env_file_fputs_assignment(FILE *f, ثابت حرف *k, ثابت حرف *v);

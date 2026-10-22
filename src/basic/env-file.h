/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

정수 parse_env_filev(FILE *f, const char *fname, va_list ap);
정수 parse_env_file_fdv(정수 fd, const char *fname, va_list ap);
정수 parse_env_file_sentinel(FILE *f, const char *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, NULL)
정수 parse_env_file_fd_sentinel(정수 fd, const char *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, NULL)

정수 parse_env_datav(const char *data, size_t size, const char *fname, va_list ap);
정수 parse_env_data_sentinel(const char *data, size_t size, const char *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, NULL)

정수 load_env_file(FILE *f, const char *fname, char ***ret);
정수 load_env_file_pairs(FILE *f, const char *fname, char ***ret);
정수 load_env_file_pairs_fd(정수 fd, const char *fname, char ***ret);

정수 merge_env_file(char ***env, FILE *f, const char *fname);

typedef enum WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

정수 write_env_file_label(정수 dir_fd, const char *fname, char **headers, char **l, WriteEnvFileFlags flags, LabelContext *label_context);
static inline 정수 write_env_file(정수 dir_fd, const char *fname, char **headers, char **l, WriteEnvFileFlags flags) {
        반환 write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ NULL);
}

정수 write_vconsole_conf(정수 dir_fd, const char *fname, char **l);

void env_file_fputs_assignment(FILE *f, const char *k, const char *v);

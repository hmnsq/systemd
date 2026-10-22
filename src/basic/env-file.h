/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ціле parse_env_filev(FILE *f, const char *fname, va_list ap);
ціле parse_env_file_fdv(ціле fd, const char *fname, va_list ap);
ціле parse_env_file_sentinel(FILE *f, const char *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, NULL)
ціле parse_env_file_fd_sentinel(ціле fd, const char *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, NULL)

ціле parse_env_datav(const char *data, size_t size, const char *fname, va_list ap);
ціле parse_env_data_sentinel(const char *data, size_t size, const char *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, NULL)

ціле load_env_file(FILE *f, const char *fname, char ***ret);
ціле load_env_file_pairs(FILE *f, const char *fname, char ***ret);
ціле load_env_file_pairs_fd(ціле fd, const char *fname, char ***ret);

ціле merge_env_file(char ***env, FILE *f, const char *fname);

typedef enum WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

ціле write_env_file_label(ціле dir_fd, const char *fname, char **headers, char **l, WriteEnvFileFlags flags, LabelContext *label_context);
static inline ціле write_env_file(ціле dir_fd, const char *fname, char **headers, char **l, WriteEnvFileFlags flags) {
        повернути write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ NULL);
}

ціле write_vconsole_conf(ціле dir_fd, const char *fname, char **l);

void env_file_fputs_assignment(FILE *f, const char *k, const char *v);

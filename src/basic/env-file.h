/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

số_nguyên parse_env_filev(FILE *f, const char *fname, va_list ap);
số_nguyên parse_env_file_fdv(số_nguyên fd, const char *fname, va_list ap);
số_nguyên parse_env_file_sentinel(FILE *f, const char *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, NULL)
số_nguyên parse_env_file_fd_sentinel(số_nguyên fd, const char *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, NULL)

số_nguyên parse_env_datav(const char *data, size_t size, const char *fname, va_list ap);
số_nguyên parse_env_data_sentinel(const char *data, size_t size, const char *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, NULL)

số_nguyên load_env_file(FILE *f, const char *fname, char ***ret);
số_nguyên load_env_file_pairs(FILE *f, const char *fname, char ***ret);
số_nguyên load_env_file_pairs_fd(số_nguyên fd, const char *fname, char ***ret);

số_nguyên merge_env_file(char ***env, FILE *f, const char *fname);

typedef enum WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

số_nguyên write_env_file_label(số_nguyên dir_fd, const char *fname, char **headers, char **l, WriteEnvFileFlags flags, LabelContext *label_context);
static inline số_nguyên write_env_file(số_nguyên dir_fd, const char *fname, char **headers, char **l, WriteEnvFileFlags flags) {
        trả_về write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ NULL);
}

số_nguyên write_vconsole_conf(số_nguyên dir_fd, const char *fname, char **l);

void env_file_fputs_assignment(FILE *f, const char *k, const char *v);

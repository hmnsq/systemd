/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

цео parse_env_filev(FILE *f, const char *fname, va_list ap);
цео parse_env_file_fdv(цео fd, const char *fname, va_list ap);
цео parse_env_file_sentinel(FILE *f, const char *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, NULL)
цео parse_env_file_fd_sentinel(цео fd, const char *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, NULL)

цео parse_env_datav(const char *data, size_t size, const char *fname, va_list ap);
цео parse_env_data_sentinel(const char *data, size_t size, const char *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, NULL)

цео load_env_file(FILE *f, const char *fname, char ***ret);
цео load_env_file_pairs(FILE *f, const char *fname, char ***ret);
цео load_env_file_pairs_fd(цео fd, const char *fname, char ***ret);

цео merge_env_file(char ***env, FILE *f, const char *fname);

typedef enum WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

цео write_env_file_label(цео dir_fd, const char *fname, char **headers, char **l, WriteEnvFileFlags flags, LabelContext *label_context);
static inline цео write_env_file(цео dir_fd, const char *fname, char **headers, char **l, WriteEnvFileFlags flags) {
        врати write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ NULL);
}

цео write_vconsole_conf(цео dir_fd, const char *fname, char **l);

void env_file_fputs_assignment(FILE *f, const char *k, const char *v);

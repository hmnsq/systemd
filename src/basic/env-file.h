/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

puur_naa_nka parse_env_filev(FILE *f, const var_na *fname, va_list ap);
puur_naa_nka parse_env_file_fdv(puur_naa_nka fd, const var_na *fname, va_list ap);
puur_naa_nka parse_env_file_sentinel(FILE *f, const var_na *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, NULL)
puur_naa_nka parse_env_file_fd_sentinel(puur_naa_nka fd, const var_na *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, NULL)

puur_naa_nka parse_env_datav(const var_na *data, size_t size, const var_na *fname, va_list ap);
puur_naa_nka parse_env_data_sentinel(const var_na *data, size_t size, const var_na *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, NULL)

puur_naa_nka load_env_file(FILE *f, const var_na *fname, var_na ***ret);
puur_naa_nka load_env_file_pairs(FILE *f, const var_na *fname, var_na ***ret);
puur_naa_nka load_env_file_pairs_fd(puur_naa_nka fd, const var_na *fname, var_na ***ret);

puur_naa_nka merge_env_file(var_na ***env, FILE *f, const var_na *fname);

typedef ga_nanaa WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

puur_naa_nka write_env_file_label(puur_naa_nka dir_fd, const var_na *fname, var_na **headers, var_na **l, WriteEnvFileFlags flags, LabelContext *label_context);
static anta_hstha puur_naa_nka write_env_file(puur_naa_nka dir_fd, const var_na *fname, var_na **headers, var_na **l, WriteEnvFileFlags flags) {
        ᬯᬮᬶ write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ NULL);
}

puur_naa_nka write_vconsole_conf(puur_naa_nka dir_fd, const var_na *fname, var_na **l);

void env_file_fputs_assignment(FILE *f, const var_na *k, const var_na *v);

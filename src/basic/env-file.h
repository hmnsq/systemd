/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

පූර්ණ_සංඛ්‍යා parse_env_filev(FILE *f, const var_na *fname, va_list ap);
පූර්ණ_සංඛ්‍යා parse_env_file_fdv(පූර්ණ_සංඛ්‍යා fd, const var_na *fname, va_list ap);
පූර්ණ_සංඛ්‍යා parse_env_file_sentinel(FILE *f, const var_na *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, NULL)
පූර්ණ_සංඛ්‍යා parse_env_file_fd_sentinel(පූර්ණ_සංඛ්‍යා fd, const var_na *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, NULL)

පූර්ණ_සංඛ්‍යා parse_env_datav(const var_na *data, size_t size, const var_na *fname, va_list ap);
පූර්ණ_සංඛ්‍යා parse_env_data_sentinel(const var_na *data, size_t size, const var_na *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, NULL)

පූර්ණ_සංඛ්‍යා load_env_file(FILE *f, const var_na *fname, var_na ***ret);
පූර්ණ_සංඛ්‍යා load_env_file_pairs(FILE *f, const var_na *fname, var_na ***ret);
පූර්ණ_සංඛ්‍යා load_env_file_pairs_fd(පූර්ණ_සංඛ්‍යා fd, const var_na *fname, var_na ***ret);

පූර්ණ_සංඛ්‍යා merge_env_file(var_na ***env, FILE *f, const var_na *fname);

typedef ga_nanaa WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

පූර්ණ_සංඛ්‍යා write_env_file_label(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *fname, var_na **headers, var_na **l, WriteEnvFileFlags flags, LabelContext *label_context);
static anta_hstha පූර්ණ_සංඛ්‍යා write_env_file(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *fname, var_na **headers, var_na **l, WriteEnvFileFlags flags) {
        ආපසු write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ NULL);
}

පූර්ණ_සංඛ්‍යා write_vconsole_conf(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *fname, var_na **l);

void env_file_fputs_assignment(FILE *f, const var_na *k, const var_na *v);

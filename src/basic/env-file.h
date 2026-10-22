/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಙ್ಕ parse_env_filev(FILE *f, ಧ್ರುವ ವರ್ಣ *fname, va_list ap);
ಪೂರ್ಣಾಙ್ಕ parse_env_file_fdv(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *fname, va_list ap);
ಪೂರ್ಣಾಙ್ಕ parse_env_file_sentinel(FILE *f, ಧ್ರುವ ವರ್ಣ *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, ರಿಕ್ತ)
ಪೂರ್ಣಾಙ್ಕ parse_env_file_fd_sentinel(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, ರಿಕ್ತ)

ಪೂರ್ಣಾಙ್ಕ parse_env_datav(ಧ್ರುವ ವರ್ಣ *data, size_t size, ಧ್ರುವ ವರ್ಣ *fname, va_list ap);
ಪೂರ್ಣಾಙ್ಕ parse_env_data_sentinel(ಧ್ರುವ ವರ್ಣ *data, size_t size, ಧ್ರುವ ವರ್ಣ *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, ರಿಕ್ತ)

ಪೂರ್ಣಾಙ್ಕ load_env_file(FILE *f, ಧ್ರುವ ವರ್ಣ *fname, ವರ್ಣ ***ret);
ಪೂರ್ಣಾಙ್ಕ load_env_file_pairs(FILE *f, ಧ್ರುವ ವರ್ಣ *fname, ವರ್ಣ ***ret);
ಪೂರ್ಣಾಙ್ಕ load_env_file_pairs_fd(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *fname, ವರ್ಣ ***ret);

ಪೂರ್ಣಾಙ್ಕ merge_env_file(ವರ್ಣ ***env, FILE *f, ಧ್ರುವ ವರ್ಣ *fname);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

ಪೂರ್ಣಾಙ್ಕ write_env_file_label(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *fname, ವರ್ಣ **headers, ವರ್ಣ **l, WriteEnvFileFlags flags, LabelContext *label_context);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ write_env_file(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *fname, ವರ್ಣ **headers, ವರ್ಣ **l, WriteEnvFileFlags flags) {
        ಪ್ರತ್ಯಾಗಮ write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ write_vconsole_conf(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *fname, ವರ್ಣ **l);

ಶೂನ್ಯ env_file_fputs_assignment(FILE *f, ಧ್ರುವ ವರ್ಣ *k, ಧ್ರುವ ವರ್ಣ *v);

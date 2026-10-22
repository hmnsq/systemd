/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಂಕ parse_env_filev(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, va_list ap);
ಪೂರ್ಣಾಂಕ parse_env_file_fdv(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, va_list ap);
ಪೂರ್ಣಾಂಕ parse_env_file_sentinel(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ...) _sentinel_;
#ವ್ಯಾಖ್ಯಾನಿಸು parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, ಖಾಲಿ)
ಪೂರ್ಣಾಂಕ parse_env_file_fd_sentinel(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ...) _sentinel_;
#ವ್ಯಾಖ್ಯಾನಿಸು parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, ಖಾಲಿ)

ಪೂರ್ಣಾಂಕ parse_env_datav(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, va_list ap);
ಪೂರ್ಣಾಂಕ parse_env_ದತ್ತಾಂಶ_sentinel(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ...) _sentinel_;
#ವ್ಯಾಖ್ಯಾನಿಸು parse_env_ದತ್ತಾಂಶ(ಪಠ್ಯ, ಗಾತ್ರ_2, fname, ...) parse_env_ದತ್ತಾಂಶ_sentinel(ಪಠ್ಯ, ಗಾತ್ರ_2, fname, __VA_ARGS__, ಖಾಲಿ)

ಪೂರ್ಣಾಂಕ ತುಂಬು_env_file(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಅಕ್ಷರ ***ret);
ಪೂರ್ಣಾಂಕ ತುಂಬು_env_file_ಜೋಡಿಗಳು(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಅಕ್ಷರ ***ret);
ಪೂರ್ಣಾಂಕ ತುಂಬು_env_file_ಜೋಡಿಗಳು_fd(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಅಕ್ಷರ ***ret);

ಪೂರ್ಣಾಂಕ ವಿಲೀನ_env_file(ಅಕ್ಷರ ***env, FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

ಪೂರ್ಣಾಂಕ write_env_file_label(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಅಕ್ಷರ **ಶೀರ್ಷಿಕೆಗಳು, ಅಕ್ಷರ **l, WriteEnvFileFlags flags, LabelContext *label_context);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ write_env_file(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಅಕ್ಷರ **ಶೀರ್ಷಿಕೆಗಳು, ಅಕ್ಷರ **l, WriteEnvFileFlags flags) {
        ಹಿಂತಿರುಗಿ write_env_file_label(dir_fd, fname, ಶೀರ್ಷಿಕೆಗಳು, l, flags, /* label_context= */ ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ write_vconsole_conf(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಅಕ್ಷರ **l);

ಶೂನ್ಯ env_file_fputs_assignment(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

முழு parse_env_filev(FILE *f, மாறிலி எழுத்து *fname, va_list ap);
முழு parse_env_file_fdv(முழு fd, மாறிலி எழுத்து *fname, va_list ap);
முழு parse_env_file_sentinel(FILE *f, மாறிலி எழுத்து *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, வெற்று)
முழு parse_env_file_fd_sentinel(முழு fd, மாறிலி எழுத்து *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, வெற்று)

முழு parse_env_datav(மாறிலி எழுத்து *data, size_t size, மாறிலி எழுத்து *fname, va_list ap);
முழு parse_env_data_sentinel(மாறிலி எழுத்து *data, size_t size, மாறிலி எழுத்து *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, வெற்று)

முழு load_env_file(FILE *f, மாறிலி எழுத்து *fname, எழுத்து ***ret);
முழு load_env_file_pairs(FILE *f, மாறிலி எழுத்து *fname, எழுத்து ***ret);
முழு load_env_file_pairs_fd(முழு fd, மாறிலி எழுத்து *fname, எழுத்து ***ret);

முழு merge_env_file(எழுத்து ***env, FILE *f, மாறிலி எழுத்து *fname);

வகைவரை எண்ணிக்கை WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

முழு write_env_file_label(முழு dir_fd, மாறிலி எழுத்து *fname, எழுத்து **headers, எழுத்து **l, WriteEnvFileFlags flags, LabelContext *label_context);
நிலையான உள்வரி முழு write_env_file(முழு dir_fd, மாறிலி எழுத்து *fname, எழுத்து **headers, எழுத்து **l, WriteEnvFileFlags flags) {
        திரும்பு write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ வெற்று);
}

முழு write_vconsole_conf(முழு dir_fd, மாறிலி எழுத்து *fname, எழுத்து **l);

வெறுமை env_file_fputs_assignment(FILE *f, மாறிலி எழுத்து *k, மாறிலி எழுத்து *v);

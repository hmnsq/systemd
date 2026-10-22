/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

পূর্ণাঙ্ক parse_env_filev(FILE *f, ধ্রুব বর্ণ *fname, va_list ap);
পূর্ণাঙ্ক parse_env_file_fdv(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *fname, va_list ap);
পূর্ণাঙ্ক parse_env_file_sentinel(FILE *f, ধ্রুব বর্ণ *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, রিক্ত)
পূর্ণাঙ্ক parse_env_file_fd_sentinel(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, রিক্ত)

পূর্ণাঙ্ক parse_env_datav(ধ্রুব বর্ণ *data, size_t size, ধ্রুব বর্ণ *fname, va_list ap);
পূর্ণাঙ্ক parse_env_data_sentinel(ধ্রুব বর্ণ *data, size_t size, ধ্রুব বর্ণ *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, রিক্ত)

পূর্ণাঙ্ক load_env_file(FILE *f, ধ্রুব বর্ণ *fname, বর্ণ ***ret);
পূর্ণাঙ্ক load_env_file_pairs(FILE *f, ধ্রুব বর্ণ *fname, বর্ণ ***ret);
পূর্ণাঙ্ক load_env_file_pairs_fd(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *fname, বর্ণ ***ret);

পূর্ণাঙ্ক merge_env_file(বর্ণ ***env, FILE *f, ধ্রুব বর্ণ *fname);

প্রকারনির্দেশ গণনা WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

পূর্ণাঙ্ক write_env_file_label(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *fname, বর্ণ **headers, বর্ণ **l, WriteEnvFileFlags flags, LabelContext *label_context);
স্থির অন্তঃস্থ পূর্ণাঙ্ক write_env_file(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *fname, বর্ণ **headers, বর্ণ **l, WriteEnvFileFlags flags) {
        প্রত্যাগম write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ রিক্ত);
}

পূর্ণাঙ্ক write_vconsole_conf(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *fname, বর্ণ **l);

শূন্য env_file_fputs_assignment(FILE *f, ধ্রুব বর্ণ *k, ধ্রুব বর্ণ *v);

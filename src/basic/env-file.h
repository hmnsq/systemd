/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

পূর্ণসংখ্যা parse_env_filev(FILE *f, ধ্রুবক অক্ষর *fname, va_list ap);
পূর্ণসংখ্যা parse_env_file_fdv(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *fname, va_list ap);
পূর্ণসংখ্যা parse_env_file_sentinel(FILE *f, ধ্রুবক অক্ষর *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, ফাঁকা)
পূর্ণসংখ্যা parse_env_file_fd_sentinel(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, ফাঁকা)

পূর্ণসংখ্যা parse_env_datav(ধ্রুবক অক্ষর *তথ্য, size_t আকার_2, ধ্রুবক অক্ষর *fname, va_list ap);
পূর্ণসংখ্যা parse_env_তথ্য_sentinel(ধ্রুবক অক্ষর *তথ্য, size_t আকার_2, ধ্রুবক অক্ষর *fname, ...) _sentinel_;
#define parse_env_তথ্য(text, আকার_2, fname, ...) parse_env_তথ্য_sentinel(text, আকার_2, fname, __VA_ARGS__, ফাঁকা)

পূর্ণসংখ্যা load_env_file(FILE *f, ধ্রুবক অক্ষর *fname, অক্ষর ***ret);
পূর্ণসংখ্যা load_env_file_pairs(FILE *f, ধ্রুবক অক্ষর *fname, অক্ষর ***ret);
পূর্ণসংখ্যা load_env_file_pairs_fd(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *fname, অক্ষর ***ret);

পূর্ণসংখ্যা merge_env_file(অক্ষর ***env, FILE *f, ধ্রুবক অক্ষর *fname);

প্রকারলেখ গণনা WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

পূর্ণসংখ্যা write_env_file_label(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *fname, অক্ষর **headers, অক্ষর **l, WriteEnvFileFlags flags, LabelContext *label_context);
স্থির অন্তঃরেখ পূর্ণসংখ্যা write_env_file(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *fname, অক্ষর **headers, অক্ষর **l, WriteEnvFileFlags flags) {
        ফেরত write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ ফাঁকা);
}

পূর্ণসংখ্যা write_vconsole_conf(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *fname, অক্ষর **l);

শূন্য env_file_fputs_assignment(FILE *f, ধ্রুবক অক্ষর *k, ধ্রুবক অক্ষর *v);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_env_filev(FILE *f, ଅଚଳ ଅକ୍ଷର *fname, va_list ap);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_env_file_fdv(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *fname, va_list ap);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_env_file_sentinel(FILE *f, ଅଚଳ ଅକ୍ଷର *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, ଖାଲି)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_env_file_fd_sentinel(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, ଖାଲି)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_env_datav(ଅଚଳ ଅକ୍ଷର *data, size_t size, ଅଚଳ ଅକ୍ଷର *fname, va_list ap);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_env_data_sentinel(ଅଚଳ ଅକ୍ଷର *data, size_t size, ଅଚଳ ଅକ୍ଷର *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, ଖାଲି)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା load_env_file(FILE *f, ଅଚଳ ଅକ୍ଷର *fname, ଅକ୍ଷର ***ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା load_env_file_pairs(FILE *f, ଅଚଳ ଅକ୍ଷର *fname, ଅକ୍ଷର ***ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା load_env_file_pairs_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *fname, ଅକ୍ଷର ***ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା merge_env_file(ଅକ୍ଷର ***env, FILE *f, ଅଚଳ ଅକ୍ଷର *fname);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_env_file_label(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *fname, ଅକ୍ଷର **headers, ଅକ୍ଷର **l, WriteEnvFileFlags flags, LabelContext *label_context);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_env_file(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *fname, ଅକ୍ଷର **headers, ଅକ୍ଷର **l, WriteEnvFileFlags flags) {
        ଫେରନ୍ତୁ write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_vconsole_conf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *fname, ଅକ୍ଷର **l);

ଶୂନ୍ୟ env_file_fputs_assignment(FILE *f, ଅଚଳ ଅକ୍ଷର *k, ଅଚଳ ଅକ୍ଷର *v);

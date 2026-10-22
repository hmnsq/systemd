/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પૂર્ણાંક parse_env_filev(FILE *f, અચલ અક્ષર *fname, va_list ap);
પૂર્ણાંક parse_env_file_fdv(પૂર્ણાંક fd, અચલ અક્ષર *fname, va_list ap);
પૂર્ણાંક parse_env_file_sentinel(FILE *f, અચલ અક્ષર *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, ખાલી)
પૂર્ણાંક parse_env_file_fd_sentinel(પૂર્ણાંક fd, અચલ અક્ષર *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, ખાલી)

પૂર્ણાંક parse_env_datav(અચલ અક્ષર *data, size_t size, અચલ અક્ષર *fname, va_list ap);
પૂર્ણાંક parse_env_data_sentinel(અચલ અક્ષર *data, size_t size, અચલ અક્ષર *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, ખાલી)

પૂર્ણાંક load_env_file(FILE *f, અચલ અક્ષર *fname, અક્ષર ***ret);
પૂર્ણાંક load_env_file_pairs(FILE *f, અચલ અક્ષર *fname, અક્ષર ***ret);
પૂર્ણાંક load_env_file_pairs_fd(પૂર્ણાંક fd, અચલ અક્ષર *fname, અક્ષર ***ret);

પૂર્ણાંક merge_env_file(અક્ષર ***env, FILE *f, અચલ અક્ષર *fname);

પ્રકારવ્યાખ્યા ગણના WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

પૂર્ણાંક write_env_file_label(પૂર્ણાંક dir_fd, અચલ અક્ષર *fname, અક્ષર **headers, અક્ષર **l, WriteEnvFileFlags flags, LabelContext *label_context);
સ્થિર અંતઃરેખ પૂર્ણાંક write_env_file(પૂર્ણાંક dir_fd, અચલ અક્ષર *fname, અક્ષર **headers, અક્ષર **l, WriteEnvFileFlags flags) {
        પાછા write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ ખાલી);
}

પૂર્ણાંક write_vconsole_conf(પૂર્ણાંક dir_fd, અચલ અક્ષર *fname, અક્ષર **l);

શૂન્ય env_file_fputs_assignment(FILE *f, અચલ અક્ષર *k, અચલ અક્ષર *v);

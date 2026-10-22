/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

പൂർണ്ണസംഖ്യ parse_env_filev(FILE *f, സ്ഥിരം അക്ഷരം *fname, va_list ap);
പൂർണ്ണസംഖ്യ parse_env_file_fdv(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *fname, va_list ap);
പൂർണ്ണസംഖ്യ parse_env_file_sentinel(FILE *f, സ്ഥിരം അക്ഷരം *fname, ...) _sentinel_;
#define parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, ശൂന്യ)
പൂർണ്ണസംഖ്യ parse_env_file_fd_sentinel(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *fname, ...) _sentinel_;
#define parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, ശൂന്യ)

പൂർണ്ണസംഖ്യ parse_env_datav(സ്ഥിരം അക്ഷരം *data, size_t size, സ്ഥിരം അക്ഷരം *fname, va_list ap);
പൂർണ്ണസംഖ്യ parse_env_data_sentinel(സ്ഥിരം അക്ഷരം *data, size_t size, സ്ഥിരം അക്ഷരം *fname, ...) _sentinel_;
#define parse_env_data(text, size, fname, ...) parse_env_data_sentinel(text, size, fname, __VA_ARGS__, ശൂന്യ)

പൂർണ്ണസംഖ്യ load_env_file(FILE *f, സ്ഥിരം അക്ഷരം *fname, അക്ഷരം ***ret);
പൂർണ്ണസംഖ്യ load_env_file_pairs(FILE *f, സ്ഥിരം അക്ഷരം *fname, അക്ഷരം ***ret);
പൂർണ്ണസംഖ്യ load_env_file_pairs_fd(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *fname, അക്ഷരം ***ret);

പൂർണ്ണസംഖ്യ merge_env_file(അക്ഷരം ***env, FILE *f, സ്ഥിരം അക്ഷരം *fname);

തരനിർവചനം എണ്ണൽ WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

പൂർണ്ണസംഖ്യ write_env_file_label(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *fname, അക്ഷരം **headers, അക്ഷരം **l, WriteEnvFileFlags flags, LabelContext *label_context);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ write_env_file(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *fname, അക്ഷരം **headers, അക്ഷരം **l, WriteEnvFileFlags flags) {
        തിരികെ write_env_file_label(dir_fd, fname, headers, l, flags, /* label_context= */ ശൂന്യ);
}

പൂർണ്ണസംഖ്യ write_vconsole_conf(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *fname, അക്ഷരം **l);

ശൂന്യം env_file_fputs_assignment(FILE *f, സ്ഥിരം അക്ഷരം *k, സ്ഥിരം അക്ഷരം *v);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਪੂਰਨ parse_env_filev(FILE *f, ਅਚਲ ਅੱਖਰ *fname, va_list ap);
ਪੂਰਨ parse_env_file_fdv(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *fname, va_list ap);
ਪੂਰਨ parse_env_file_sentinel(FILE *f, ਅਚਲ ਅੱਖਰ *fname, ...) _sentinel_;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, ਨਿਰਾ)
ਪੂਰਨ parse_env_file_fd_sentinel(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *fname, ...) _sentinel_;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, ਨਿਰਾ)

ਪੂਰਨ parse_env_datav(ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *fname, va_list ap);
ਪੂਰਨ parse_env_ਅੰਕੜੇ_sentinel(ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *fname, ...) _sentinel_;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ parse_env_ਅੰਕੜੇ(ਪਾਠ, ਮਾਪ, fname, ...) parse_env_ਅੰਕੜੇ_sentinel(ਪਾਠ, ਮਾਪ, fname, __VA_ARGS__, ਨਿਰਾ)

ਪੂਰਨ ਲੱਦੋ_env_file(FILE *f, ਅਚਲ ਅੱਖਰ *fname, ਅੱਖਰ ***ret);
ਪੂਰਨ ਲੱਦੋ_env_file_ਜੋੜੇ(FILE *f, ਅਚਲ ਅੱਖਰ *fname, ਅੱਖਰ ***ret);
ਪੂਰਨ ਲੱਦੋ_env_file_ਜੋੜੇ_fd(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *fname, ਅੱਖਰ ***ret);

ਪੂਰਨ ਮਿਲਾਓ_env_file(ਅੱਖਰ ***env, FILE *f, ਅਚਲ ਅੱਖਰ *fname);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

ਪੂਰਨ write_env_file_label(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *fname, ਅੱਖਰ **ਸਿਰਲੇਖਾਂ, ਅੱਖਰ **l, WriteEnvFileFlags flags, LabelContext *label_context);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ write_env_file(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *fname, ਅੱਖਰ **ਸਿਰਲੇਖਾਂ, ਅੱਖਰ **l, WriteEnvFileFlags flags) {
        ਵਾਪਸ write_env_file_label(dir_fd, fname, ਸਿਰਲੇਖਾਂ, l, flags, /* label_context= */ ਨਿਰਾ);
}

ਪੂਰਨ write_vconsole_conf(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *fname, ਅੱਖਰ **l);

ਖਾਲੀ env_file_fputs_assignment(FILE *f, ਅਚਲ ਅੱਖਰ *k, ਅਚਲ ਅੱਖਰ *v);

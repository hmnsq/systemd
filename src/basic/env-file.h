/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

صحيح parse_env_filev(FILE *f, ثابت حرف *fname, va_list ap);
صحيح parse_env_file_fdv(صحيح fd, ثابت حرف *fname, va_list ap);
صحيح parse_env_file_sentinel(FILE *f, ثابت حرف *fname, ...) _sentinel_;
#وصف_ڪريو parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, عدم)
صحيح parse_env_file_fd_sentinel(صحيح fd, ثابت حرف *fname, ...) _sentinel_;
#وصف_ڪريو parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, عدم)

صحيح parse_env_datav(ثابت حرف *معلومات, size_t سائيز, ثابت حرف *fname, va_list ap);
صحيح parse_env_معلومات_sentinel(ثابت حرف *معلومات, size_t سائيز, ثابت حرف *fname, ...) _sentinel_;
#وصف_ڪريو parse_env_معلومات(متن, سائيز, fname, ...) parse_env_معلومات_sentinel(متن, سائيز, fname, __VA_ARGS__, عدم)

صحيح ڀريو_env_file(FILE *f, ثابت حرف *fname, حرف ***ret);
صحيح ڀريو_env_file_جوڙا(FILE *f, ثابت حرف *fname, حرف ***ret);
صحيح ڀريو_env_file_جوڙا_fd(صحيح fd, ثابت حرف *fname, حرف ***ret);

صحيح ملايو_env_file(حرف ***env, FILE *f, ثابت حرف *fname);

تعريفنوع تعداد WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

صحيح write_env_file_label(صحيح dir_fd, ثابت حرف *fname, حرف **سرا, حرف **l, WriteEnvFileFlags flags, LabelContext *label_context);
ساكن مضمن صحيح write_env_file(صحيح dir_fd, ثابت حرف *fname, حرف **سرا, حرف **l, WriteEnvFileFlags flags) {
        أرجع write_env_file_label(dir_fd, fname, سرا, l, flags, /* label_context= */ عدم);
}

صحيح write_vconsole_conf(صحيح dir_fd, ثابت حرف *fname, حرف **l);

فراغ env_file_fputs_assignment(FILE *f, ثابت حرف *k, ثابت حرف *v);

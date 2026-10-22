/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

صحيح parse_env_filev(FILE *f, ثابت حرف *fname, va_list ap);
صحيح parse_env_file_fdv(صحيح fd, ثابت حرف *fname, va_list ap);
صحيح parse_env_file_sentinel(FILE *f, ثابت حرف *fname, ...) _sentinel_;
#تعریف_کن parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, عدم)
صحيح parse_env_file_fd_sentinel(صحيح fd, ثابت حرف *fname, ...) _sentinel_;
#تعریف_کن parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, عدم)

صحيح parse_env_datav(ثابت حرف *داده, size_t اندازه, ثابت حرف *fname, va_list ap);
صحيح parse_env_داده_sentinel(ثابت حرف *داده, size_t اندازه, ثابت حرف *fname, ...) _sentinel_;
#تعریف_کن parse_env_داده(متن, اندازه, fname, ...) parse_env_داده_sentinel(متن, اندازه, fname, __VA_ARGS__, عدم)

صحيح بارگیری_env_file(FILE *f, ثابت حرف *fname, حرف ***ret);
صحيح بارگیری_env_file_جفتها(FILE *f, ثابت حرف *fname, حرف ***ret);
صحيح بارگیری_env_file_جفتها_fd(صحيح fd, ثابت حرف *fname, حرف ***ret);

صحيح ادغام_env_file(حرف ***env, FILE *f, ثابت حرف *fname);

تعريفنوع تعداد WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

صحيح write_env_file_label(صحيح dir_fd, ثابت حرف *fname, حرف **سرآیندها, حرف **l, WriteEnvFileFlags flags, LabelContext *label_context);
ساكن مضمن صحيح write_env_file(صحيح dir_fd, ثابت حرف *fname, حرف **سرآیندها, حرف **l, WriteEnvFileFlags flags) {
        أرجع write_env_file_label(dir_fd, fname, سرآیندها, l, flags, /* label_context= */ عدم);
}

صحيح write_vconsole_conf(صحيح dir_fd, ثابت حرف *fname, حرف **l);

فراغ env_file_fputs_assignment(FILE *f, ثابت حرف *k, ثابت حرف *v);

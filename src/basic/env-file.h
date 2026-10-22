/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

صحیح parse_env_filev(FILE *f, ثابت حرف *fname, va_list ap);
صحیح parse_env_file_fdv(صحیح fd, ثابت حرف *fname, va_list ap);
صحیح parse_env_file_sentinel(FILE *f, ثابت حرف *fname, ...) _sentinel_;
#تعریف_کرو parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, تہی)
صحیح parse_env_file_fd_sentinel(صحیح fd, ثابت حرف *fname, ...) _sentinel_;
#تعریف_کرو parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, تہی)

صحیح parse_env_datav(ثابت حرف *ڈیٹا, size_t سائز, ثابت حرف *fname, va_list ap);
صحیح parse_env_ڈیٹا_sentinel(ثابت حرف *ڈیٹا, size_t سائز, ثابت حرف *fname, ...) _sentinel_;
#تعریف_کرو parse_env_ڈیٹا(متن, سائز, fname, ...) parse_env_ڈیٹا_sentinel(متن, سائز, fname, __VA_ARGS__, تہی)

صحیح بھرو_env_file(FILE *f, ثابت حرف *fname, حرف ***ret);
صحیح بھرو_env_file_جوڑے(FILE *f, ثابت حرف *fname, حرف ***ret);
صحیح بھرو_env_file_جوڑے_fd(صحیح fd, ثابت حرف *fname, حرف ***ret);

صحیح ملاؤ_env_file(حرف ***env, FILE *f, ثابت حرف *fname);

قسمتعریف شمار WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

صحیح write_env_file_label(صحیح dir_fd, ثابت حرف *fname, حرف **سرخیاں, حرف **l, WriteEnvFileFlags flags, LabelContext *label_context);
ساکن اندرونی صحیح write_env_file(صحیح dir_fd, ثابت حرف *fname, حرف **سرخیاں, حرف **l, WriteEnvFileFlags flags) {
        واپس write_env_file_label(dir_fd, fname, سرخیاں, l, flags, /* label_context= */ تہی);
}

صحیح write_vconsole_conf(صحیح dir_fd, ثابت حرف *fname, حرف **l);

خالی env_file_fputs_assignment(FILE *f, ثابت حرف *k, ثابت حرف *v);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

تعريفنوع تعداد ImageClass {
        IMAGE_MACHINE,
        IMAGE_PORTABLE,
        IMAGE_SYSEXT,
        IMAGE_CLASS_EXTENSION_اولی = IMAGE_SYSEXT,  /* First "extension" image type, so that we can easily generically iterate through them */
        IMAGE_CONFEXT,
        IMAGE_CLASS_EXTENSION_آخرین = IMAGE_CONFEXT,  /* Last "extension image type */
        _IMAGE_CLASS_MAX,
        _IMAGE_CLASS_INVALID = -EINVAL,
} ImageClass;

DECLARE_STRING_جدول_LOOKUP(image_class, ImageClass);

/* The *_extension_release flavours will look for /usr/lib/extension-release/extension-release.NAME
 * for sysext images and for /etc/extension-release.d/extension-release.NAME for confext images
 * in accordance with the OS extension specification, rather than for /usr/lib/ or /etc/os-release. */

bool image_name_is_valid(ثابت حرف *s) خالص;
صحيح مسیر_استخراج_image_name(ثابت حرف *مسیر, حرف **ret);

صحيح مسیر_is_extension_درخت(ImageClass image_class, ثابت حرف *مسیر, ثابت حرف *extension, bool relax_extension_release_check);
ساكن مضمن صحيح مسیر_is_os_درخت(ثابت حرف *مسیر) {
        أرجع مسیر_is_extension_درخت(_IMAGE_CLASS_INVALID, مسیر, عدم, false);
}
صحيح fd_is_os_درخت(صحيح fd);

صحيح open_extension_release(ثابت حرف *ریشه, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف **ret_مسیر, صحيح *ret_fd);
صحيح open_extension_release_at(صحيح rfd, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف **ret_مسیر, صحيح *ret_fd);
صحيح open_os_release(ثابت حرف *ریشه, حرف **ret_مسیر, صحيح *ret_fd);
صحيح open_os_release_at(صحيح rfd, حرف **ret_مسیر, صحيح *ret_fd);

صحيح parse_extension_release_sentinel(ثابت حرف *ریشه, ImageClass image_class, bool relax_extension_release_check, ثابت حرف *extension, ...) _sentinel_;
#تعریف_کن parse_extension_release(ریشه, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(ریشه, image_class, relax_extension_release_check, extension, __VA_ARGS__, عدم)
#تعریف_کن parse_os_release(ریشه, ...)                                     \
        parse_extension_release_sentinel(ریشه, _IMAGE_CLASS_INVALID, false, عدم, __VA_ARGS__, عدم)

صحيح parse_extension_release_at_sentinel(صحيح rfd, ImageClass image_class, bool relax_extension_release_check, ثابت حرف *extension, ...) _sentinel_;
#تعریف_کن parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, عدم)
#تعریف_کن parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, عدم, __VA_ARGS__, عدم)

صحيح بارگیری_extension_release_جفتها(ثابت حرف *ریشه, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف ***ret);
ساكن مضمن صحيح بارگیری_os_release_جفتها(ثابت حرف *ریشه, حرف ***ret) {
        أرجع بارگیری_extension_release_جفتها(ریشه, _IMAGE_CLASS_INVALID, عدم, false, ret);
}
صحيح بارگیری_os_release_جفتها_with_پیشوند(ثابت حرف *ریشه, ثابت حرف *پیشوند, حرف ***ret);

صحيح os_release_support_ended(ثابت حرف *support_انتها, bool quiet, usec_t *ret_eol);

ثابت حرف* os_release_pretty_name(ثابت حرف *pretty_name, ثابت حرف *name);

bool use_fancy_name(ثابت حرف *fancy_name);
حرف *unescape_fancy_name(حرف **fancy_name);

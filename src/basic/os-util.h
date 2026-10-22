/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

تعريفنوع تعداد ImageClass {
        IMAGE_MACHINE,
        IMAGE_PORTABLE,
        IMAGE_SYSEXT,
        IMAGE_CLASS_EXTENSION_لومړی = IMAGE_SYSEXT,  /* First "extension" image type, so that we can easily generically iterate through them */
        IMAGE_CONFEXT,
        IMAGE_CLASS_EXTENSION_وروستی = IMAGE_CONFEXT,  /* Last "extension image type */
        _IMAGE_CLASS_MAX,
        _IMAGE_CLASS_INVALID = -EINVAL,
} ImageClass;

DECLARE_STRING_جدول_LOOKUP(image_class, ImageClass);

/* The *_extension_release flavours will look for /usr/lib/extension-release/extension-release.NAME
 * for sysext images and for /etc/extension-release.d/extension-release.NAME for confext images
 * in accordance with the OS extension specification, rather than for /usr/lib/ or /etc/os-release. */

bool image_name_is_valid(ثابت حرف *s) خالص;
صحيح لار_ایستل_image_name(ثابت حرف *لار, حرف **ret);

صحيح لار_is_extension_ونه(ImageClass image_class, ثابت حرف *لار, ثابت حرف *extension, bool relax_extension_release_check);
ساكن مضمن صحيح لار_is_os_ونه(ثابت حرف *لار) {
        أرجع لار_is_extension_ونه(_IMAGE_CLASS_INVALID, لار, عدم, false);
}
صحيح fd_is_os_ونه(صحيح fd);

صحيح open_extension_release(ثابت حرف *ریښه, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف **ret_لار, صحيح *ret_fd);
صحيح open_extension_release_at(صحيح rfd, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف **ret_لار, صحيح *ret_fd);
صحيح open_os_release(ثابت حرف *ریښه, حرف **ret_لار, صحيح *ret_fd);
صحيح open_os_release_at(صحيح rfd, حرف **ret_لار, صحيح *ret_fd);

صحيح parse_extension_release_sentinel(ثابت حرف *ریښه, ImageClass image_class, bool relax_extension_release_check, ثابت حرف *extension, ...) _sentinel_;
#تعریف_کړه parse_extension_release(ریښه, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(ریښه, image_class, relax_extension_release_check, extension, __VA_ARGS__, عدم)
#تعریف_کړه parse_os_release(ریښه, ...)                                     \
        parse_extension_release_sentinel(ریښه, _IMAGE_CLASS_INVALID, false, عدم, __VA_ARGS__, عدم)

صحيح parse_extension_release_at_sentinel(صحيح rfd, ImageClass image_class, bool relax_extension_release_check, ثابت حرف *extension, ...) _sentinel_;
#تعریف_کړه parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, عدم)
#تعریف_کړه parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, عدم, __VA_ARGS__, عدم)

صحيح بارول_extension_release_جوړې(ثابت حرف *ریښه, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف ***ret);
ساكن مضمن صحيح بارول_os_release_جوړې(ثابت حرف *ریښه, حرف ***ret) {
        أرجع بارول_extension_release_جوړې(ریښه, _IMAGE_CLASS_INVALID, عدم, false, ret);
}
صحيح بارول_os_release_جوړې_with_مختاړی(ثابت حرف *ریښه, ثابت حرف *مختاړی, حرف ***ret);

صحيح os_release_support_ended(ثابت حرف *support_پای_ټکی, bool quiet, usec_t *ret_eol);

ثابت حرف* os_release_pretty_name(ثابت حرف *pretty_name, ثابت حرف *name);

bool use_fancy_name(ثابت حرف *fancy_name);
حرف *unescape_fancy_name(حرف **fancy_name);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

تعريفنوع تعداد ImageClass {
        IMAGE_MACHINE,
        IMAGE_PORTABLE,
        IMAGE_SYSEXT,
        IMAGE_CLASS_EXTENSION_پهريون = IMAGE_SYSEXT,  /* First "extension" image type, so that we can easily generically iterate through them */
        IMAGE_CONFEXT,
        IMAGE_CLASS_EXTENSION_آخري = IMAGE_CONFEXT,  /* Last "extension image type */
        _IMAGE_CLASS_MAX,
        _IMAGE_CLASS_INVALID = -EINVAL,
} ImageClass;

DECLARE_STRING_جدول_LOOKUP(image_class, ImageClass);

/* The *_extension_release flavours will look for /usr/lib/extension-release/extension-release.NAME
 * for sysext images and for /etc/extension-release.d/extension-release.NAME for confext images
 * in accordance with the OS extension specification, rather than for /usr/lib/ or /etc/os-release. */

bool image_name_is_valid(ثابت حرف *s) خالص;
صحيح رستو_ڪڍو_image_name(ثابت حرف *رستو, حرف **ret);

صحيح رستو_is_extension_وڻ(ImageClass image_class, ثابت حرف *رستو, ثابت حرف *extension, bool relax_extension_release_check);
ساكن مضمن صحيح رستو_is_os_وڻ(ثابت حرف *رستو) {
        أرجع رستو_is_extension_وڻ(_IMAGE_CLASS_INVALID, رستو, عدم, false);
}
صحيح fd_is_os_وڻ(صحيح fd);

صحيح open_extension_release(ثابت حرف *پاڙ, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف **ret_رستو, صحيح *ret_fd);
صحيح open_extension_release_at(صحيح rfd, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف **ret_رستو, صحيح *ret_fd);
صحيح open_os_release(ثابت حرف *پاڙ, حرف **ret_رستو, صحيح *ret_fd);
صحيح open_os_release_at(صحيح rfd, حرف **ret_رستو, صحيح *ret_fd);

صحيح parse_extension_release_sentinel(ثابت حرف *پاڙ, ImageClass image_class, bool relax_extension_release_check, ثابت حرف *extension, ...) _sentinel_;
#وصف_ڪريو parse_extension_release(پاڙ, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(پاڙ, image_class, relax_extension_release_check, extension, __VA_ARGS__, عدم)
#وصف_ڪريو parse_os_release(پاڙ, ...)                                     \
        parse_extension_release_sentinel(پاڙ, _IMAGE_CLASS_INVALID, false, عدم, __VA_ARGS__, عدم)

صحيح parse_extension_release_at_sentinel(صحيح rfd, ImageClass image_class, bool relax_extension_release_check, ثابت حرف *extension, ...) _sentinel_;
#وصف_ڪريو parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, عدم)
#وصف_ڪريو parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, عدم, __VA_ARGS__, عدم)

صحيح ڀريو_extension_release_جوڙا(ثابت حرف *پاڙ, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف ***ret);
ساكن مضمن صحيح ڀريو_os_release_جوڙا(ثابت حرف *پاڙ, حرف ***ret) {
        أرجع ڀريو_extension_release_جوڙا(پاڙ, _IMAGE_CLASS_INVALID, عدم, false, ret);
}
صحيح ڀريو_os_release_جوڙا_with_اڳياڙي(ثابت حرف *پاڙ, ثابت حرف *اڳياڙي, حرف ***ret);

صحيح os_release_support_ended(ثابت حرف *support_پڇاڙي, bool quiet, usec_t *ret_eol);

ثابت حرف* os_release_pretty_name(ثابت حرف *pretty_name, ثابت حرف *name);

bool use_fancy_name(ثابت حرف *fancy_name);
حرف *unescape_fancy_name(حرف **fancy_name);

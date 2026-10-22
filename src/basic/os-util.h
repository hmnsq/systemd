/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

قسمتعریف شمار ImageClass {
        IMAGE_MACHINE,
        IMAGE_PORTABLE,
        IMAGE_SYSEXT,
        IMAGE_CLASS_EXTENSION_پہلا = IMAGE_SYSEXT,  /* First "extension" image type, so that we can easily generically iterate through them */
        IMAGE_CONFEXT,
        IMAGE_CLASS_EXTENSION_آخری = IMAGE_CONFEXT,  /* Last "extension image type */
        _IMAGE_CLASS_MAX,
        _IMAGE_CLASS_INVALID = -EINVAL,
} ImageClass;

DECLARE_STRING_جدول_LOOKUP(image_class, ImageClass);

/* The *_extension_release flavours will look for /usr/lib/extension-release/extension-release.NAME
 * for sysext images and for /etc/extension-release.d/extension-release.NAME for confext images
 * in accordance with the OS extension specification, rather than for /usr/lib/ or /etc/os-release. */

bool image_name_is_valid(ثابت حرف *s) خالص;
صحیح راستہ_نکالو_image_name(ثابت حرف *راستہ, حرف **ret);

صحیح راستہ_is_extension_درخت(ImageClass image_class, ثابت حرف *راستہ, ثابت حرف *extension, bool relax_extension_release_check);
ساکن اندرونی صحیح راستہ_is_os_درخت(ثابت حرف *راستہ) {
        واپس راستہ_is_extension_درخت(_IMAGE_CLASS_INVALID, راستہ, تہی, false);
}
صحیح fd_is_os_درخت(صحیح fd);

صحیح open_extension_release(ثابت حرف *جڑ, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف **ret_راستہ, صحیح *ret_fd);
صحیح open_extension_release_at(صحیح rfd, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف **ret_راستہ, صحیح *ret_fd);
صحیح open_os_release(ثابت حرف *جڑ, حرف **ret_راستہ, صحیح *ret_fd);
صحیح open_os_release_at(صحیح rfd, حرف **ret_راستہ, صحیح *ret_fd);

صحیح parse_extension_release_sentinel(ثابت حرف *جڑ, ImageClass image_class, bool relax_extension_release_check, ثابت حرف *extension, ...) _sentinel_;
#تعریف_کرو parse_extension_release(جڑ, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(جڑ, image_class, relax_extension_release_check, extension, __VA_ARGS__, تہی)
#تعریف_کرو parse_os_release(جڑ, ...)                                     \
        parse_extension_release_sentinel(جڑ, _IMAGE_CLASS_INVALID, false, تہی, __VA_ARGS__, تہی)

صحیح parse_extension_release_at_sentinel(صحیح rfd, ImageClass image_class, bool relax_extension_release_check, ثابت حرف *extension, ...) _sentinel_;
#تعریف_کرو parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, تہی)
#تعریف_کرو parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, تہی, __VA_ARGS__, تہی)

صحیح بھرو_extension_release_جوڑے(ثابت حرف *جڑ, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف ***ret);
ساکن اندرونی صحیح بھرو_os_release_جوڑے(ثابت حرف *جڑ, حرف ***ret) {
        واپس بھرو_extension_release_جوڑے(جڑ, _IMAGE_CLASS_INVALID, تہی, false, ret);
}
صحیح بھرو_os_release_جوڑے_with_سابقہ(ثابت حرف *جڑ, ثابت حرف *سابقہ, حرف ***ret);

صحیح os_release_support_ended(ثابت حرف *support_انتہا, bool quiet, usec_t *ret_eol);

ثابت حرف* os_release_pretty_name(ثابت حرف *pretty_name, ثابت حرف *name);

bool use_fancy_name(ثابت حرف *fancy_name);
حرف *unescape_fancy_name(حرف **fancy_name);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

تعريفنوع تعداد ImageClass {
        IMAGE_MACHINE,
        IMAGE_PORTABLE,
        IMAGE_SYSEXT,
        _IMAGE_CLASS_EXTENSION_FIRST = IMAGE_SYSEXT,  /* First "extension" image type, so that we can easily generically iterate through them */
        IMAGE_CONFEXT,
        _IMAGE_CLASS_EXTENSION_LAST = IMAGE_CONFEXT,  /* Last "extension image type */
        _IMAGE_CLASS_MAX,
        _IMAGE_CLASS_INVALID = -EINVAL,
} ImageClass;

DECLARE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* The *_extension_release flavours will look for /usr/lib/extension-release/extension-release.NAME
 * for sysext images and for /etc/extension-release.d/extension-release.NAME for confext images
 * in accordance with the OS extension specification, rather than for /usr/lib/ or /etc/os-release. */

bool image_name_is_valid(ثابت حرف *s) _pure_;
صحيح path_extract_image_name(ثابت حرف *path, حرف **ret);

صحيح path_is_extension_درخت(ImageClass image_class, ثابت حرف *path, ثابت حرف *extension, bool relax_extension_release_check);
ساكن مضمن صحيح path_is_os_درخت(ثابت حرف *path) {
        أرجع path_is_extension_درخت(_IMAGE_CLASS_INVALID, path, عدم, false);
}
صحيح fd_is_os_درخت(صحيح fd);

صحيح open_extension_release(ثابت حرف *جڑ, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف **ret_path, صحيح *ret_fd);
صحيح open_extension_release_at(صحيح rfd, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف **ret_path, صحيح *ret_fd);
صحيح open_os_release(ثابت حرف *جڑ, حرف **ret_path, صحيح *ret_fd);
صحيح open_os_release_at(صحيح rfd, حرف **ret_path, صحيح *ret_fd);

صحيح parse_extension_release_sentinel(ثابت حرف *جڑ, ImageClass image_class, bool relax_extension_release_check, ثابت حرف *extension, ...) _sentinel_;
#define parse_extension_release(جڑ, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(جڑ, image_class, relax_extension_release_check, extension, __VA_ARGS__, عدم)
#define parse_os_release(جڑ, ...)                                     \
        parse_extension_release_sentinel(جڑ, _IMAGE_CLASS_INVALID, false, عدم, __VA_ARGS__, عدم)

صحيح parse_extension_release_at_sentinel(صحيح rfd, ImageClass image_class, bool relax_extension_release_check, ثابت حرف *extension, ...) _sentinel_;
#define parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, عدم)
#define parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, عدم, __VA_ARGS__, عدم)

صحيح load_extension_release_pairs(ثابت حرف *جڑ, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف ***ret);
ساكن مضمن صحيح load_os_release_pairs(ثابت حرف *جڑ, حرف ***ret) {
        أرجع load_extension_release_pairs(جڑ, _IMAGE_CLASS_INVALID, عدم, false, ret);
}
صحيح load_os_release_pairs_with_prefix(ثابت حرف *جڑ, ثابت حرف *prefix, حرف ***ret);

صحيح os_release_support_ended(ثابت حرف *support_end, bool quiet, usec_t *ret_eol);

ثابت حرف* os_release_pretty_name(ثابت حرف *pretty_name, ثابت حرف *name);

bool use_fancy_name(ثابت حرف *fancy_name);
حرف *unescape_fancy_name(حرف **fancy_name);

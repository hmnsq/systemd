/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

تعریفنوع شمارش ImageClass {
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

bool image_name_is_valid(ثابت نویسه *s) _pure_;
صحیح path_extract_image_name(ثابت نویسه *path, نویسه **ret);

صحیح path_is_extension_درخت(ImageClass image_class, ثابت نویسه *path, ثابت نویسه *extension, bool relax_extension_release_check);
ایستا درونخطی صحیح path_is_os_درخت(ثابت نویسه *path) {
        بازگشت path_is_extension_درخت(_IMAGE_CLASS_INVALID, path, پوچ, false);
}
صحیح fd_is_os_درخت(صحیح fd);

صحیح open_extension_release(ثابت نویسه *ریشه, ImageClass image_class, ثابت نویسه *extension, bool relax_extension_release_check, نویسه **ret_path, صحیح *ret_fd);
صحیح open_extension_release_at(صحیح rfd, ImageClass image_class, ثابت نویسه *extension, bool relax_extension_release_check, نویسه **ret_path, صحیح *ret_fd);
صحیح open_os_release(ثابت نویسه *ریشه, نویسه **ret_path, صحیح *ret_fd);
صحیح open_os_release_at(صحیح rfd, نویسه **ret_path, صحیح *ret_fd);

صحیح parse_extension_release_sentinel(ثابت نویسه *ریشه, ImageClass image_class, bool relax_extension_release_check, ثابت نویسه *extension, ...) _sentinel_;
#define parse_extension_release(ریشه, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(ریشه, image_class, relax_extension_release_check, extension, __VA_ARGS__, پوچ)
#define parse_os_release(ریشه, ...)                                     \
        parse_extension_release_sentinel(ریشه, _IMAGE_CLASS_INVALID, false, پوچ, __VA_ARGS__, پوچ)

صحیح parse_extension_release_at_sentinel(صحیح rfd, ImageClass image_class, bool relax_extension_release_check, ثابت نویسه *extension, ...) _sentinel_;
#define parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, پوچ)
#define parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, پوچ, __VA_ARGS__, پوچ)

صحیح load_extension_release_pairs(ثابت نویسه *ریشه, ImageClass image_class, ثابت نویسه *extension, bool relax_extension_release_check, نویسه ***ret);
ایستا درونخطی صحیح load_os_release_pairs(ثابت نویسه *ریشه, نویسه ***ret) {
        بازگشت load_extension_release_pairs(ریشه, _IMAGE_CLASS_INVALID, پوچ, false, ret);
}
صحیح load_os_release_pairs_with_prefix(ثابت نویسه *ریشه, ثابت نویسه *prefix, نویسه ***ret);

صحیح os_release_support_ended(ثابت نویسه *support_end, bool quiet, usec_t *ret_eol);

ثابت نویسه* os_release_pretty_name(ثابت نویسه *pretty_name, ثابت نویسه *name);

bool use_fancy_name(ثابت نویسه *fancy_name);
نویسه *unescape_fancy_name(نویسه **fancy_name);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை எண்ணிக்கை ImageClass {
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

bool image_name_is_valid(மாறிலி எழுத்து *s) _pure_;
முழு path_extract_image_name(மாறிலி எழுத்து *path, எழுத்து **ret);

முழு path_is_extension_மரம்(ImageClass image_class, மாறிலி எழுத்து *path, மாறிலி எழுத்து *extension, bool relax_extension_release_check);
நிலையான உள்வரி முழு path_is_os_மரம்(மாறிலி எழுத்து *path) {
        திரும்பு path_is_extension_மரம்(_IMAGE_CLASS_INVALID, path, வெற்று, false);
}
முழு fd_is_os_மரம்(முழு fd);

முழு open_extension_release(மாறிலி எழுத்து *வேர், ImageClass image_class, மாறிலி எழுத்து *extension, bool relax_extension_release_check, எழுத்து **ret_path, முழு *ret_fd);
முழு open_extension_release_at(முழு rfd, ImageClass image_class, மாறிலி எழுத்து *extension, bool relax_extension_release_check, எழுத்து **ret_path, முழு *ret_fd);
முழு open_os_release(மாறிலி எழுத்து *வேர், எழுத்து **ret_path, முழு *ret_fd);
முழு open_os_release_at(முழு rfd, எழுத்து **ret_path, முழு *ret_fd);

முழு parse_extension_release_sentinel(மாறிலி எழுத்து *வேர், ImageClass image_class, bool relax_extension_release_check, மாறிலி எழுத்து *extension, ...) _sentinel_;
#define parse_extension_release(வேர், image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(வேர், image_class, relax_extension_release_check, extension, __VA_ARGS__, வெற்று)
#define parse_os_release(வேர், ...)                                     \
        parse_extension_release_sentinel(வேர், _IMAGE_CLASS_INVALID, false, வெற்று, __VA_ARGS__, வெற்று)

முழு parse_extension_release_at_sentinel(முழு rfd, ImageClass image_class, bool relax_extension_release_check, மாறிலி எழுத்து *extension, ...) _sentinel_;
#define parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, வெற்று)
#define parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, வெற்று, __VA_ARGS__, வெற்று)

முழு load_extension_release_pairs(மாறிலி எழுத்து *வேர், ImageClass image_class, மாறிலி எழுத்து *extension, bool relax_extension_release_check, எழுத்து ***ret);
நிலையான உள்வரி முழு load_os_release_pairs(மாறிலி எழுத்து *வேர், எழுத்து ***ret) {
        திரும்பு load_extension_release_pairs(வேர், _IMAGE_CLASS_INVALID, வெற்று, false, ret);
}
முழு load_os_release_pairs_with_prefix(மாறிலி எழுத்து *வேர், மாறிலி எழுத்து *prefix, எழுத்து ***ret);

முழு os_release_support_ended(மாறிலி எழுத்து *support_end, bool quiet, usec_t *ret_eol);

மாறிலி எழுத்து* os_release_pretty_name(மாறிலி எழுத்து *pretty_name, மாறிலி எழுத்து *name);

bool use_fancy_name(மாறிலி எழுத்து *fancy_name);
எழுத்து *unescape_fancy_name(எழுத்து **fancy_name);

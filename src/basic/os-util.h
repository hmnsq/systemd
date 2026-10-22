/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ImageClass {
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

bool image_name_is_valid(ಧ್ರುವ ವರ್ಣ *s) _pure_;
ಪೂರ್ಣಾಙ್ಕ path_extract_image_name(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ path_is_extension_ವೃಕ್ಷ(ImageClass image_class, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *extension, bool relax_extension_release_check);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ path_is_os_ವೃಕ್ಷ(ಧ್ರುವ ವರ್ಣ *path) {
        ಪ್ರತ್ಯಾಗಮ path_is_extension_ವೃಕ್ಷ(_IMAGE_CLASS_INVALID, path, ರಿಕ್ತ, false);
}
ಪೂರ್ಣಾಙ್ಕ fd_is_os_ವೃಕ್ಷ(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ open_extension_release(ಧ್ರುವ ವರ್ಣ *ಬೇರು, ImageClass image_class, ಧ್ರುವ ವರ್ಣ *extension, bool relax_extension_release_check, ವರ್ಣ **ret_path, ಪೂರ್ಣಾಙ್ಕ *ret_fd);
ಪೂರ್ಣಾಙ್ಕ open_extension_release_at(ಪೂರ್ಣಾಙ್ಕ rfd, ImageClass image_class, ಧ್ರುವ ವರ್ಣ *extension, bool relax_extension_release_check, ವರ್ಣ **ret_path, ಪೂರ್ಣಾಙ್ಕ *ret_fd);
ಪೂರ್ಣಾಙ್ಕ open_os_release(ಧ್ರುವ ವರ್ಣ *ಬೇರು, ವರ್ಣ **ret_path, ಪೂರ್ಣಾಙ್ಕ *ret_fd);
ಪೂರ್ಣಾಙ್ಕ open_os_release_at(ಪೂರ್ಣಾಙ್ಕ rfd, ವರ್ಣ **ret_path, ಪೂರ್ಣಾಙ್ಕ *ret_fd);

ಪೂರ್ಣಾಙ್ಕ parse_extension_release_sentinel(ಧ್ರುವ ವರ್ಣ *ಬೇರು, ImageClass image_class, bool relax_extension_release_check, ಧ್ರುವ ವರ್ಣ *extension, ...) _sentinel_;
#define parse_extension_release(ಬೇರು, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(ಬೇರು, image_class, relax_extension_release_check, extension, __VA_ARGS__, ರಿಕ್ತ)
#define parse_os_release(ಬೇರು, ...)                                     \
        parse_extension_release_sentinel(ಬೇರು, _IMAGE_CLASS_INVALID, false, ರಿಕ್ತ, __VA_ARGS__, ರಿಕ್ತ)

ಪೂರ್ಣಾಙ್ಕ parse_extension_release_at_sentinel(ಪೂರ್ಣಾಙ್ಕ rfd, ImageClass image_class, bool relax_extension_release_check, ಧ್ರುವ ವರ್ಣ *extension, ...) _sentinel_;
#define parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, ರಿಕ್ತ)
#define parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, ರಿಕ್ತ, __VA_ARGS__, ರಿಕ್ತ)

ಪೂರ್ಣಾಙ್ಕ load_extension_release_pairs(ಧ್ರುವ ವರ್ಣ *ಬೇರು, ImageClass image_class, ಧ್ರುವ ವರ್ಣ *extension, bool relax_extension_release_check, ವರ್ಣ ***ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ load_os_release_pairs(ಧ್ರುವ ವರ್ಣ *ಬೇರು, ವರ್ಣ ***ret) {
        ಪ್ರತ್ಯಾಗಮ load_extension_release_pairs(ಬೇರು, _IMAGE_CLASS_INVALID, ರಿಕ್ತ, false, ret);
}
ಪೂರ್ಣಾಙ್ಕ load_os_release_pairs_with_prefix(ಧ್ರುವ ವರ್ಣ *ಬೇರು, ಧ್ರುವ ವರ್ಣ *prefix, ವರ್ಣ ***ret);

ಪೂರ್ಣಾಙ್ಕ os_release_support_ended(ಧ್ರುವ ವರ್ಣ *support_end, bool quiet, usec_t *ret_eol);

ಧ್ರುವ ವರ್ಣ* os_release_pretty_name(ಧ್ರುವ ವರ್ಣ *pretty_name, ಧ್ರುವ ವರ್ಣ *name);

bool use_fancy_name(ಧ್ರುವ ವರ್ಣ *fancy_name);
ವರ್ಣ *unescape_fancy_name(ವರ್ಣ **fancy_name);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश गणना ImageClass {
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

bool image_name_is_valid(ध्रुव वर्ण *s) _pure_;
पूर्णाङ्क path_extract_image_name(ध्रुव वर्ण *path, वर्ण **ret);

पूर्णाङ्क path_is_extension_वृक्षः(ImageClass image_class, ध्रुव वर्ण *path, ध्रुव वर्ण *extension, bool relax_extension_release_check);
स्थिर अन्तःस्थ पूर्णाङ्क path_is_os_वृक्षः(ध्रुव वर्ण *path) {
        प्रत्यागम path_is_extension_वृक्षः(_IMAGE_CLASS_INVALID, path, रिक्त, false);
}
पूर्णाङ्क fd_is_os_वृक्षः(पूर्णाङ्क fd);

पूर्णाङ्क open_extension_release(ध्रुव वर्ण *मूलम्, ImageClass image_class, ध्रुव वर्ण *extension, bool relax_extension_release_check, वर्ण **ret_path, पूर्णाङ्क *ret_fd);
पूर्णाङ्क open_extension_release_at(पूर्णाङ्क rfd, ImageClass image_class, ध्रुव वर्ण *extension, bool relax_extension_release_check, वर्ण **ret_path, पूर्णाङ्क *ret_fd);
पूर्णाङ्क open_os_release(ध्रुव वर्ण *मूलम्, वर्ण **ret_path, पूर्णाङ्क *ret_fd);
पूर्णाङ्क open_os_release_at(पूर्णाङ्क rfd, वर्ण **ret_path, पूर्णाङ्क *ret_fd);

पूर्णाङ्क parse_extension_release_sentinel(ध्रुव वर्ण *मूलम्, ImageClass image_class, bool relax_extension_release_check, ध्रुव वर्ण *extension, ...) _sentinel_;
#define parse_extension_release(मूलम्, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(मूलम्, image_class, relax_extension_release_check, extension, __VA_ARGS__, रिक्त)
#define parse_os_release(मूलम्, ...)                                     \
        parse_extension_release_sentinel(मूलम्, _IMAGE_CLASS_INVALID, false, रिक्त, __VA_ARGS__, रिक्त)

पूर्णाङ्क parse_extension_release_at_sentinel(पूर्णाङ्क rfd, ImageClass image_class, bool relax_extension_release_check, ध्रुव वर्ण *extension, ...) _sentinel_;
#define parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, रिक्त)
#define parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, रिक्त, __VA_ARGS__, रिक्त)

पूर्णाङ्क load_extension_release_pairs(ध्रुव वर्ण *मूलम्, ImageClass image_class, ध्रुव वर्ण *extension, bool relax_extension_release_check, वर्ण ***ret);
स्थिर अन्तःस्थ पूर्णाङ्क load_os_release_pairs(ध्रुव वर्ण *मूलम्, वर्ण ***ret) {
        प्रत्यागम load_extension_release_pairs(मूलम्, _IMAGE_CLASS_INVALID, रिक्त, false, ret);
}
पूर्णाङ्क load_os_release_pairs_with_prefix(ध्रुव वर्ण *मूलम्, ध्रुव वर्ण *prefix, वर्ण ***ret);

पूर्णाङ्क os_release_support_ended(ध्रुव वर्ण *support_end, bool quiet, usec_t *ret_eol);

ध्रुव वर्ण* os_release_pretty_name(ध्रुव वर्ण *pretty_name, ध्रुव वर्ण *name);

bool use_fancy_name(ध्रुव वर्ण *fancy_name);
वर्ण *unescape_fancy_name(वर्ण **fancy_name);
